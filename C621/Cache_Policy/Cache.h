#ifndef __CACHE_H__
#define __CACHE_H__

#include "Cache_Blk.h"
#include "Request.h"

/* Cache */
typedef struct Cache
{
    unsigned num_blocks;
    
    Cache_Block *blocks; // All cache blocks
}Cache;

Cache *initCache();

#endif
