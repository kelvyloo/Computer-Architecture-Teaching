#include "Trace.h"
#include "Cache.h"

extern TraceParser *initTraceParser(const char * mem_file);
extern bool getRequest(TraceParser *mem_trace);

extern Cache* initCache();
extern bool accessBlock(Cache *cache, Request *req, uint64_t access_time);

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

    uint64_t cycles = 0;
    while (getRequest(mem_trace))
    {
        // Step one, accessBlock()
        if (accessBlock(cache, mem_trace->cur_req, cycles))
        {
            // Cache hit!
        }
        else
        {
            // Cache miss!
            // Step two, insertBlock()
        }

        exit(0);
        ++num_of_reqs;
        ++cycles;
    }
}
