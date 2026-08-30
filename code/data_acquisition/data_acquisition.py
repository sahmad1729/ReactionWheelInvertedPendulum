import asyncio
import serial_asyncio
import serial
import time
import csv
import keyboard
import numpy as np
import pandas as pd 
import matplotlib.pyplot as plt
from collections import deque

class Motor:  
  
  def __init__(self, port='COM3', baudrate=921600):
    self.port = port
    self.baudrate = baudrate
    self.sampling_interval = 10 # In milli seconds (Should match value in ARR)
    self.input_speed = 0
    self.output_speed = 0
    self.file_name = None
    self.is_logging = False
    self.reader = None
    self.writer = None
    self.data_buffer = deque()
    self.PACKET_SIZE = 1
    self.PPR = 1024*2 #Should match MCU value
    self.PULSES_TO_DEG = 360/self.PPR
    self.START_ANGLE = 180.0
    self.START_POSITION = (self.START_ANGLE)*(self.PPR/360.0)
    self.START_POSITION_DEG = self.START_POSITION * self.PULSES_TO_DEG

  async def setup_serial(self):
    try:
      self.reader, self.writer = await serial_asyncio.open_serial_connection(
        url=self.port,
        baudrate=self.baudrate,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE
        )
    except Exception as e:
      print(e)
      raise

  async def _send_command(self, command_byte):
    if self.writer:
      self.writer.write(command_byte)
      await self.writer.drain()

  async def _start_motor(self):
    data = (1).to_bytes(1, byteorder='little', signed=False)
    await self._send_command(data)

  async def _stop_motor(self):
    data = (2).to_bytes(1, byteorder='little', signed=False)
    await self._send_command(data)

  async def _read_serial_data(self):
    try:
      print("reading")
      while self.is_logging:
        data = await asyncio.wait_for(self.reader.read(self.PACKET_SIZE), timeout=10.0)
        if data:
          for d in data:
            self.data_buffer.append(d)
        await asyncio.sleep(0)
    except Exception as e:
      print("reading exception")
      self.is_logging=False
    finally:
      print("reading done")

  async def _save_data(self):
    try:
      with open(self.file_name, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(['Theta', 'Theta_dot', 'input speed', 'output speed'])
        data_collected = [0xAA]
        escape_next = False
        stop_next = False
        prev_position_deg = self.START_POSITION_DEG
        while self.is_logging:
          start = time.perf_counter()
          while self.data_buffer:
            byte = self.data_buffer.popleft()
            if byte == 0xAA:
              if len(data_collected) == 7:
                direction = 1 if data_collected[0] == 0 else -1
                position = ((data_collected[2] << 8) | data_collected[1])
                position_deg = (position % self.PPR)*self.PULSES_TO_DEG
                diff = abs(position_deg - prev_position_deg)
                if diff > 180:
                  diff = min([360.0 - position_deg, 360 - prev_position_deg])
                speed = direction*(diff / (self.sampling_interval/1000))
                rpm = speed * (1/6) #deg per sec to rpm --- 1rev/min = 1deg * (1rev/360deg) * (1/sec) * (60sec/min)
                output_speed = (data_collected[4] << 8) | data_collected[3]
                if output_speed & 0x8000:
                  output_speed = output_speed - 0x10000
                input_speed = (data_collected[6] << 8) | data_collected[5]
                if input_speed & 0x8000:
                  input_speed = input_speed - 0x10000
                prev_position_deg = position_deg
                #print(f'Angle: {position_deg}\nInput Speed: {input_speed}\n')
                csvwriter.writerow([position_deg, rpm, input_speed, (1/6)*output_speed])
              data_collected = []
              escape_next = False
              stop_next = True
              continue
            if byte == 0xAB:
              escape_next = True
              continue
            if escape_next:
              escape_next = False
              if byte == 0x00:
                byte = 0xAB
              elif byte == 0x01:
                byte = 0xAA
              else:
                data_collected = []
                continue
            data_collected.append(byte)
          await asyncio.sleep(0)
    except Exception as e:
      print("saving exception")
      print(e)
      self.is_logging = False
    finally:
      print("saving done")

  async def _keyboard_monitor(self):
    while self.is_logging:
      if keyboard.is_pressed(' '):
        print("Stop key pressed")
        await self._stop_motor()
        self.is_logging = False
        break
      await asyncio.sleep(0.1)
    print("keyboard monitor turning off")

  async def start_logging(self, file_name, duration=5):
    self.file_name = file_name
    self.is_logging = True
    tasks = [
      asyncio.create_task(self._read_serial_data()),
      asyncio.create_task(self._save_data()),
      asyncio.create_task(self._keyboard_monitor())
      ]
    try:
      await self._start_motor()
      s = time.perf_counter()
      #await asyncio.sleep(duration)
    finally:
      #self.is_logging = False
      await asyncio.gather(*tasks, return_exceptions=True)
      print(time.perf_counter()-s)
      print("...done...")

  def plot_data(self):
    try: 
      # Plot the data
      data = pd.read_csv(self.file_name)
      theta = data['Theta'].values
      theta_dot = data['Theta_dot'].values
      input_speeds = data['input speed'].values
      output_speeds = data['output speed'].values
      print(len(input_speeds), len(output_speeds))
      time_step = self.sampling_interval/1000  # 10 ms in seconds
      time = np.arange(len(input_speeds)) * time_step

      fig, axs = plt.subplots(3, 1, figsize=(12, 6))
      axs[0].plot(time, input_speeds, label='Input Speed', color='b')
      axs[0].plot(time, output_speeds, label='Output Speed', color='r')
      axs[0].set_xlabel('Time (s)')
      axs[0].set_ylabel('Speed')
      #axs[0].set_title('Motor Speed Control')
      axs[0].legend()
      axs[0].grid(True)
      
      axs[1].plot(time, theta, label='Theta', color='g')
      axs[1].set_xlabel('Time (s)')
      axs[1].set_ylabel('Theta')
      #axs[1].set_title('Theta and Theta Dot')
      axs[1].legend()
      axs[1].grid(True)

      axs[2].plot(time, theta_dot, label='Theta Dot', color='m')
      axs[2].set_xlabel('Time (s)')
      axs[2].set_ylabel('Theta Dot')
      #axs[1].set_title('Theta and Theta Dot')
      axs[2].legend()
      axs[2].grid(True)
      
      plt.tight_layout()
      plt.show()
    except Exception as e:
      print(e)
     
async def main():
  motor = Motor()
  await motor.setup_serial()
  while True:
    try:
      # i = int(input("Enter Command: "))
      i = 1   
      if i == 1:
        await motor.start_logging('data.csv')
        motor.plot_data()
        break     
      elif i == -1:
        break
    except ValueError:
      print("Invalid Input")
    except Exception as e:
      print(e)
      break
    await asyncio.sleep(0.1)
  print("Closing ...")

if __name__ == "__main__":
  asyncio.run(main())
      
    
