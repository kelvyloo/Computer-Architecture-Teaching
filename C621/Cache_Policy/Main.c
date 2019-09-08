#include "Trace.h"
#include "Cache.h"

extern TraceParser *initTraceParser(const char * mem_file);
extern bool getRequest(TraceParser *mem_trace);

extern Cache* initCache();

int main(int argc, const char *argv[])
{	
    if (argc != 2)
    {
        printf("Usage: %s %s\n", argv[0], "<mem-file>");

        return 0;
    }

    // Initialize a CPU trace parser
    TraceParser *mem_trace = initTraceParser(argv[1]);

    // Initialize a Cache
    Cache *cache = initCache();
    
    // Running the trace
    uint64_t num_of_reqs = 0;

    while (getRequest(mem_trace))
    {
        // Step one, accessBlock()

        // Step two, insertBlock() if there is a miss

        ++num_of_reqs;
    }
}
