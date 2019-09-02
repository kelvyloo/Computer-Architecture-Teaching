#include "Trace.h"

// You can play around with these settings.
const unsigned localPredictorSize = 2048;
const unsigned localCounterBits = 2;

typedef struct Branch_Predictor
{
    unsigned localPredictorSets;

}Branch_Predictor;

Branch_Predictor *initBranchPredictor()
{
    Branch_Predictor *branch_predictor = (Branch_Predictor *)malloc(sizeof(Branch_Predictor));
    branch_predictor->localPredictorSets = localPredictorSize / localCounterBits;
}
