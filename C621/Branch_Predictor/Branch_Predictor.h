#ifndef __BRANCH_PREDICTOR_HH__
#define __BRANCH_PREDICTOR_HH__

#include "Instruction.h"

typedef struct Branch_Predictor
{
    unsigned localPredictorSets; // Number of entries in a local predictor


}Branch_Predictor;

/*
Branch_Predictor *initBranchPredictor()
{
    Branch_Predictor *branch_predictor = (Branch_Predictor *)malloc(sizeof(Branch_Predictor));
    branch_predictor->localPredictorSets = localPredictorSize / localCounterBits;
}
*/

// Define functions
int checkPowerofTwo(unsigned x);

#endif
