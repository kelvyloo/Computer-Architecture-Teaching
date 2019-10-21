#include "Branch_Predictor.h"

const unsigned instShiftAmt = 2; // Number of bits to shift a PC by

// You can play around with these settings.
const unsigned localPredictorSize = 65536;
const unsigned localCounterBits = 2;
const unsigned localHistoryTableSize = 16384; 
const unsigned globalPredictorSize = 16384;
const unsigned globalCounterBits = 2;
const unsigned choicePredictorSize = 16384; // Keep this the same as globalPredictorSize.
const unsigned choiceCounterBits = 2;

// gShare const variables
const unsigned global_history_register_size = 15;
const unsigned pattern_history_table_size   = 32768; // 2 ^ number of ghr bits
const unsigned pht_counter_bits             = 2;

Branch_Predictor *initBranchPredictor()
{
    Branch_Predictor *branch_predictor = (Branch_Predictor *)malloc(sizeof(Branch_Predictor));

    #ifdef TWO_BIT_LOCAL
    branch_predictor->local_predictor_sets = localPredictorSize;
    assert(checkPowerofTwo(branch_predictor->local_predictor_sets));

    branch_predictor->index_mask = branch_predictor->local_predictor_sets - 1;

    // Initialize sat counters
    branch_predictor->local_counters =
        (Sat_Counter *)malloc(branch_predictor->local_predictor_sets * sizeof(Sat_Counter));

    int i = 0;
    for (i; i < branch_predictor->local_predictor_sets; i++)
    {
        initSatCounter(&(branch_predictor->local_counters[i]), localCounterBits);
    }
    #endif

    #ifdef TOURNAMENT
    assert(checkPowerofTwo(localPredictorSize));
    assert(checkPowerofTwo(localHistoryTableSize));
    assert(checkPowerofTwo(globalPredictorSize));
    assert(checkPowerofTwo(choicePredictorSize));
    assert(globalPredictorSize == choicePredictorSize);

    branch_predictor->local_predictor_size = localPredictorSize;
    branch_predictor->local_history_table_size = localHistoryTableSize;
    branch_predictor->global_predictor_size = globalPredictorSize;
    branch_predictor->choice_predictor_size = choicePredictorSize;
   
    // Initialize local counters 
    branch_predictor->local_counters =
        (Sat_Counter *)malloc(localPredictorSize * sizeof(Sat_Counter));

    int i = 0;
    for (i; i < localPredictorSize; i++)
    {
        initSatCounter(&(branch_predictor->local_counters[i]), localCounterBits);
    }

    branch_predictor->local_predictor_mask = localPredictorSize - 1;

    // Initialize local history table
    branch_predictor->local_history_table = 
        (unsigned *)malloc(localHistoryTableSize * sizeof(unsigned));

    for (i = 0; i < localHistoryTableSize; i++)
    {
        branch_predictor->local_history_table[i] = 0;
    }

    branch_predictor->local_history_table_mask = localHistoryTableSize - 1;

    // Initialize global counters
    branch_predictor->global_counters = 
        (Sat_Counter *)malloc(globalPredictorSize * sizeof(Sat_Counter));

    for (i = 0; i < globalPredictorSize; i++)
    {
        initSatCounter(&(branch_predictor->global_counters[i]), globalCounterBits);
    }

    branch_predictor->global_history_mask = globalPredictorSize - 1;

    // Initialize choice counters
    branch_predictor->choice_counters = 
        (Sat_Counter *)malloc(choicePredictorSize * sizeof(Sat_Counter));

    for (i = 0; i < choicePredictorSize; i++)
    {
        initSatCounter(&(branch_predictor->choice_counters[i]), choiceCounterBits);
    }

    branch_predictor->choice_history_mask = choicePredictorSize - 1;

    // global history register
    branch_predictor->global_history = 0;

    // We assume choice predictor size is always equal to global predictor size.
    branch_predictor->history_register_mask = choicePredictorSize - 1;
    #endif

    #ifdef GSHARE
    assert(checkPowerofTwo(pattern_history_table_size));

    branch_predictor->global_history_register = 0;

    // Initialize pattern_history_table
    branch_predictor->pattern_history_table = 
        (Sat_Counter *)malloc(pattern_history_table_size * sizeof(Sat_Counter));

    for (int i = 0; i < pattern_history_table_size; i++)
        initSatCounter(&(branch_predictor->pattern_history_table[i]), pht_counter_bits);

    #endif

    #ifdef PERCEPTRON

    // Init global history, index, and y output
    branch_predictor->global_history = 0;
    branch_predictor->y = 0;

    // Initialize perceptron table
    for (int i = 0; i < NUM_PERCEPTRONS; i++) {
        branch_predictor->bias[i] = 0;

        for (int j = 0; j < N_WEIGHTS; j++)
            branch_predictor->perceptrons[i][j];
    }

    #endif

    return branch_predictor;
}

// sat counter functions
inline void initSatCounter(Sat_Counter *sat_counter, unsigned counter_bits)
{
    sat_counter->counter_bits = counter_bits;
    sat_counter->max_val = (1 << counter_bits) - 1;
    sat_counter->counter = sat_counter->max_val;
}

inline void incrementCounter(Sat_Counter *sat_counter)
{
    if (sat_counter->counter < sat_counter->max_val)
    {
        ++sat_counter->counter;
    }
}

inline void decrementCounter(Sat_Counter *sat_counter)
{
    if (sat_counter->counter > 0) 
    {
        --sat_counter->counter;
    }
}

// Branch Predictor functions
bool predict(Branch_Predictor *branch_predictor, Instruction *instr)
{
    uint64_t branch_address = instr->PC;

    #ifdef TWO_BIT_LOCAL    
    // Step one, get prediction
    unsigned local_index = getIndex(branch_address, 
                                    branch_predictor->index_mask);

    bool prediction = getPrediction(&(branch_predictor->local_counters[local_index]));

    // Step two, update counter
    if (instr->taken)
    {
        // printf("Correct: %u -> ", branch_predictor->local_counters[local_index].counter);
        incrementCounter(&(branch_predictor->local_counters[local_index]));
        // printf("%u\n", branch_predictor->local_counters[local_index].counter);
    }
    else
    {
        // printf("Incorrect: %u -> ", branch_predictor->local_counters[local_index].counter);
        decrementCounter(&(branch_predictor->local_counters[local_index]));
        // printf("%u\n", branch_predictor->local_counters[local_index].counter);
    }

    return prediction == instr->taken;
    #endif

    #ifdef TOURNAMENT
    // Step one, get local prediction.
    unsigned local_history_table_idx = getIndex(branch_address,
                                           branch_predictor->local_history_table_mask);
    
    unsigned local_predictor_idx = 
        branch_predictor->local_history_table[local_history_table_idx] & 
        branch_predictor->local_predictor_mask;

    bool local_prediction = 
        getPrediction(&(branch_predictor->local_counters[local_predictor_idx]));

    // Step two, get global prediction.
    unsigned global_predictor_idx = 
        branch_predictor->global_history & branch_predictor->global_history_mask;

    bool global_prediction = 
        getPrediction(&(branch_predictor->global_counters[global_predictor_idx]));

    // Step three, get choice prediction.
    unsigned choice_predictor_idx = 
        branch_predictor->global_history & branch_predictor->choice_history_mask;

    bool choice_prediction = 
        getPrediction(&(branch_predictor->choice_counters[choice_predictor_idx]));


    // Step four, final prediction.
    bool final_prediction;
    if (choice_prediction)
    {
        final_prediction = global_prediction;
    }
    else
    {
        final_prediction = local_prediction;
    }

    bool prediction_correct = final_prediction == instr->taken;
    // Step five, update counters
    if (local_prediction != global_prediction)
    {
        if (local_prediction == instr->taken)
        {
            // Should be more favorable towards local predictor.
            decrementCounter(&(branch_predictor->choice_counters[choice_predictor_idx]));
        }
        else if (global_prediction == instr->taken)
        {
            // Should be more favorable towards global predictor.
            incrementCounter(&(branch_predictor->choice_counters[choice_predictor_idx]));
        }
    }

    if (instr->taken)
    {
        incrementCounter(&(branch_predictor->global_counters[global_predictor_idx]));
        incrementCounter(&(branch_predictor->local_counters[local_predictor_idx]));
    }
    else
    {
        decrementCounter(&(branch_predictor->global_counters[global_predictor_idx]));
        decrementCounter(&(branch_predictor->local_counters[local_predictor_idx]));
    }

    // Step six, update global history register
    branch_predictor->global_history = branch_predictor->global_history << 1 | instr->taken;
    branch_predictor->local_history_table[local_history_table_idx] = branch_predictor->local_history_table[local_history_table_idx] << 1 | instr->taken;
    // exit(0);
    //
    return prediction_correct;
    #endif

    #ifdef GSHARE
    /* Update the global history table with whether the branch taken or !taken */
    branch_predictor->global_history_register &= ((1 << global_history_register_size) - 1);
    branch_predictor->global_history_register  = branch_predictor->global_history_register << 1;
    branch_predictor->global_history_register |= instr->taken;

    /* XOR global history reg with PC */
    uint64_t pht_index = branch_predictor->global_history_register ^ instr->PC;
    pht_index &= ((1 << global_history_register_size) - 1);

    bool predictor = getPrediction(&branch_predictor->pattern_history_table[pht_index]);

    if (instr->taken)
        incrementCounter(&branch_predictor->pattern_history_table[pht_index]);
    else
        decrementCounter(&branch_predictor->pattern_history_table[pht_index]);

    return (predictor == instr->taken);

    #endif

    #ifdef PERCEPTRON
    int index, x, j, t;
    bool prediction;

    // 1.) Hash branch address to index perceptron table
    index = instr->PC % NUM_PERCEPTRONS;
    branch_predictor->y = branch_predictor->bias[index];
    j = 1;

    // 2. ) ith perceptron fetched
    for (int i = 0; i < N_WEIGHTS; i++) {
        x = (branch_predictor->global_history & j) ? 1 : -1;

        // 3.) y calculated as dot product P and GHR
        branch_predictor->y += branch_predictor->perceptrons[index][i] * x;
        
        j = j << 1;
    }
    // 4.) Branch predicted as taken if positive else !taken
    prediction = (branch_predictor->y >= 0) ? true : false;

    // 5.) Train perceptrons with actual outcome
    t = (instr->taken) ? 1 : -1;

    if (instr->taken != prediction || 
        (branch_predictor->y < THETA && 
         branch_predictor->y > -1*THETA)) {

        //if (branch_predictor->bias[index] > MIN_WEIGHT &&
        //    branch_predictor->bias[index] < MAX_WEIGHT)
        //    branch_predictor->bias[index] += t;
        
        j = 1;

        for (int i = 0; i < N_WEIGHTS; i++) {
            x = ((branch_predictor->global_history & j) && instr->taken) ? 1 : -1;
            j = j << 1;

            // 6.) P written back into ith entry of table
            if (branch_predictor->perceptrons[index][i] > MIN_WEIGHT && 
                branch_predictor->perceptrons[index][i] < MAX_WEIGHT)
                branch_predictor->perceptrons[index][i] += x;
        }
    }
    // Update global history reg
    branch_predictor->global_history  = branch_predictor->global_history << 1;
    branch_predictor->global_history |= instr->taken;
    
    return prediction;

    #endif
}

inline unsigned getIndex(uint64_t branch_addr, unsigned index_mask)
{
    return (branch_addr >> instShiftAmt) & index_mask;
}

inline bool getPrediction(Sat_Counter *sat_counter)
{
    uint8_t counter = sat_counter->counter;
    unsigned counter_bits = sat_counter->counter_bits;

    // MSB determins the direction
    return (counter >> (counter_bits - 1));
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
