// rl_forward_pass.c

#include "agent.h"
#include "weights.h"

// ReLU activation function
float relu(float x) {
    return x > 0 ? x : 0;
}

int8_t actionToRPM[17] = {-40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40};

// Static arrays to reduce stack usage
static float layer0[HIDDEN_SIZE] = {0};
static float layer1[HIDDEN_SIZE] = {0};
static float layer2[HIDDEN_SIZE] = {0};
static float output[OUTPUT_SIZE] = {0};

// Forward pass function
int16_t forward_pass(const float inputs[INPUT_SIZE]) {
    
    float sum = 0;
    // Input Layer computation
    for (int8_t i = 0; i < HIDDEN_SIZE; i++) {
        sum = input_bias[i];
        for (int8_t j = 0; j < INPUT_SIZE; j++) {
            sum += input_weights[i][j] * inputs[j];
        }
        layer0[i] = relu(sum);
    }
    
    // Layer 1 computation
    for (int8_t i = 0; i < HIDDEN_SIZE; i++) {
        sum = hidden1_bias[i];
        for (int8_t j = 0; j < HIDDEN_SIZE; j++) {
            sum += hidden1_weights[i][j] * layer0[j];
        }
        layer1[i] = relu(sum);
    }

    // Layer 2 computation
    for (int8_t i = 0; i < HIDDEN_SIZE; i++) {
        sum = hidden2_bias[i];
        for (int8_t j = 0; j < HIDDEN_SIZE; j++) {
            sum += hidden2_weights[i][j] * layer1[j];
        }
        layer2[i] = relu(sum);
    }

    // Output layer computation with max index tracking
    int8_t max_index = 0;
    for (int8_t i = 0; i < OUTPUT_SIZE; i++) {
        sum = output_bias[i];
        for (int8_t j = 0; j < HIDDEN_SIZE; j++) {
            sum += output_weights[i][j] * layer2[j];
        }
        output[i] = sum;
        if (sum > output[max_index]) {
            max_index = i;
        }
    }
    return actionToRPM[max_index]*10;
}
