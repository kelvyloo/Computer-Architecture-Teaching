#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "Cache_Blk.h"
#include "Request.h"

/* Cache */
typedef struct Set
{
    Cache_Block **ways; // Block ways within a set
}Set;

typedef struct Cache
{
    unsigned num_blocks;
    
    Cache_Block *blocks; // All cache blocks

    /* Set-Associative Information */
    unsigned num_sets; // Number of sets
    unsigned num_ways; // Number of ways within a set

    unsigned set_shift;
    unsigned set_mask; // To extract set index
    unsigned tag_shift; // To extract tag

    Set *sets; // All the sets of a cache
    
}Cache;

// Function Definitions
Cache *initCache();
bool accessBlock(Request *req, uint64_t access_time);

#endif
