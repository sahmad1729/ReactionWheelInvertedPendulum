#ifndef AGENT_H
#define AGENT_H

#include "main.h"

// Architecture constants
#define INPUT_SIZE 3
#define HIDDEN_SIZE 16
#define OUTPUT_SIZE 17
//#define OUTPUT_SIZE 9

int16_t forward_pass(const float inputs[INPUT_SIZE]);

#endif
