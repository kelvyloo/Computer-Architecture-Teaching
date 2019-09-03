#ifndef __BRANCH_PREDICTOR_HH__
#define __BRANCH_PREDICTOR_HH__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Instruction.h"

// Predictor type
// #define TWO_BIT_LOCAL
#define BI_MODE

// saturating counter
typedef struct Sat_Counter
{
    uint8_t max_val;
    uint8_t counter;
}Sat_Counter;

typedef struct Branch_Predictor
{
    #ifdef TWO_BIT_LOCAL
    unsigned local_predictor_sets; // Number of entries in a local predictor
    unsigned index_mask;

    Sat_Counter *local_counters;
    #endif

    #ifdef BI_MODE
    unsigned choice_counter_size;
    unsigned taken_counter_size;
    unsigned not_taken_counter_size;

    Sat_Counter *choice_counters;
    Sat_Counter *taken_counters;
    Sat_Counter *not_taken_counters;
    #endif
}Branch_Predictor;

// Initialization function
Branch_Predictor *initBranchPredictor();

// Counter functions
void initSatCounter(Sat_Counter *sat_counter);
void incrementCounter(Sat_Counter *sat_counter);
void decrementCounter(Sat_Counter *sat_counter);

// Branch predictor functions
bool predict(Branch_Predictor *branch_predictor, Instruction *instr);

unsigned getLocalIndex(uint64_t branch_addr, unsigned index_mask);
bool getPrediction(uint8_t counter);

// Utility
int checkPowerofTwo(unsigned x);

#endif
