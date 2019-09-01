#include <stdbool.h>
#include <stdint.h>

enum Instruction_Type{EXE, BRANCH, LOAD, STORE};
typedef enum Instruction_Type Instruction_Type;

// Instruction Format
typedef struct Instruction
{
    uint64_t PC; // Program Counter of the instruction

    Instruction_Type instr_type; // Instruction type

    uint64_t load_or_store_addr; // load or store address (for LOAD and STORE instructions)

    bool taken; // If the instruction is a branch, what is the real direction (not the predicted).
                // You should reply on this field to determine the correctness of your predictions.
}Instruction;

typedef struct TraceParser
{
    FILE *fd; // file descriptor for the trace file

    Instruction *cur_instr; // current instruction
}TraceParser;
