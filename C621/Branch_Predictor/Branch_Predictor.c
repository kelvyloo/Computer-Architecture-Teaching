#include "Branch_Predictor.h"

const unsigned instShiftAmt = 2; // Number of bits to shift a PC by

// You can play around with these settings.
const unsigned localPredictorSize = 2048;
const unsigned localCounterBits = 2;

Branch_Predictor *initBranchPredictor()
{
    Branch_Predictor *branch_predictor = (Branch_Predictor *)malloc(sizeof(Branch_Predictor));
    branch_predictor->local_predictor_sets = localPredictorSize / localCounterBits;
    assert(checkPowerofTwo(branch_predictor->local_predictor_sets));

    branch_predictor->index_mask = branch_predictor->local_predictor_sets - 1;

    // Initialize sat counters
    branch_predictor->local_counters = (Sat_Counter *)malloc(branch_predictor->local_predictor_sets *
                                                             sizeof(Sat_Counter));

    int i = 0;
    for (i; i < branch_predictor->local_predictor_sets; i++)
    {
        initSatCounter(&(branch_predictor->local_counters[i]));
    }
}

inline void initSatCounter(Sat_Counter *sat_counter)
{
    sat_counter->counter = 0;
    sat_counter->max_val = (1 << localCounterBits) - 1;
//    printf("%"PRIu8"\n", sat_counter->max_val);
}

void incrementCounter(Sat_Counter *sat_counter)
{

}

// Branch Predictor functions
inline unsigned getLocalIndex(uint64_t branch_addr, Branch_Predictor *branch_predictor)
{
    return (branch_addr >> instShiftAmt) & branch_predictor->index_mask;
}

inline bool getPrediction(uint8_t counter)
{
    return (counter >> (localCounterBits - 1));
}

int checkPowerofTwo(unsigned x)
{
    //checks whether a number is zero or not
    if (x == 0)
    {
        return 0;
    }

    //true till x is not equal to 1
    while( x != 1)
    {
        //checks whether a number is divisible by 2
        if(x % 2 != 0)
        {
            return 0;
        }
        x /= 2;
    }
    return 1;
}
