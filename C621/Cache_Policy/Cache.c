#include "Cache.h"

/* Constants */
const unsigned block_size = 64; // Size of a cache line (in Bytes)
// TODO, you should try different size of cache, for example, 512KB, 1MB, 2MB
const unsigned cache_size = 1; // Size of a cache (in KB)
// TODO, you should try different association configurations, for example 4, 8, 16
const unsigned assoc = 16;

Cache *initCache()
{
    Cache *cache = (Cache *)malloc(sizeof(Cache));

    cache->blk_mask = block_size - 1;

    unsigned num_blocks = cache_size * 1024 / block_size;
    cache->num_blocks = num_blocks;
//    printf("Num of blocks: %u\n", cache->num_blocks);

    // Initialize all cache blocks
    cache->blocks = (Cache_Block *)malloc(num_blocks * sizeof(Cache_Block));
    
    int i;
    for (i = 0; i < num_blocks; i++)
    {
        cache->blocks[i].tag = UINTMAX_MAX; 
        cache->blocks[i].valid = false;
        cache->blocks[i].dirty = false;
        cache->blocks[i].when_touched = 0;
        cache->blocks[i].frequency = 0;
    }

    // Initialize Set-way variables
    unsigned num_sets = cache_size * 1024 / (block_size * assoc);
    cache->num_sets = num_sets;
    cache->num_ways = assoc;
//    printf("Num of sets: %u\n", cache->num_sets);

    unsigned set_shift = log2(block_size);
    cache->set_shift = set_shift;
//    printf("Set shift: %u\n", cache->set_shift);

    unsigned set_mask = num_sets - 1;
    cache->set_mask = set_mask;
//    printf("Set mask: %u\n", cache->set_mask);

    unsigned tag_shift = set_shift + log2(num_sets);
    cache->tag_shift = tag_shift;
//    printf("Tag shift: %u\n", cache->tag_shift);

    // Initialize Sets
    cache->sets = (Set *)malloc(num_sets * sizeof(Set));
    for (i = 0; i < num_sets; i++)
    {
        cache->sets[i].ways = (Cache_Block **)malloc(assoc * sizeof(Cache_Block *));
    }

    // Combine sets and blocks
    for (i = 0; i < num_blocks; i++)
    {
        Cache_Block *blk = &(cache->blocks[i]);
        
        uint32_t set = i / assoc;
        uint32_t way = i % assoc;

        blk->set = set;
        blk->way = way;

        cache->sets[set].ways[way] = blk;
    }

    return cache;
}

bool accessBlock(Cache *cache, Request *req, uint64_t access_time)
{
    bool hit = false;

    uint64_t blk_aligned_addr = blkAlign(req->load_or_store_addr, cache->blk_mask);

    Cache_Block *blk = findBlock(cache, blk_aligned_addr);
   
    if (blk != NULL) 
    {
        hit = true;

        // TODO, cache->policy->upgrade(blk, access_time)
	// TODO, if LOAD, dirty
    }

    return hit;
}

// Helper Functions
uint64_t blkAlign(uint64_t addr, uint64_t mask)
{
    return addr & ~mask;
}

Cache_Block *findBlock(Cache *cache, uint64_t addr)
{
    printf("Addr: %"PRIu64"\n", addr);

    // Extract tag
    uint64_t tag = addr >> cache->tag_shift;
    printf("Tag: %"PRIu64"\n", tag);

    // Extract set index
    uint64_t set_idx = (addr >> cache->set_shift) & cache->set_mask;
    printf("Set: %"PRIu64"\n", set_idx);

    Cache_Block **ways = cache->sets[set_idx].ways;
    int i;
    for (i = 0; i < cache->num_ways; i++)
    {
        if (tag == ways[i]->tag && ways[i]->valid == true)
        {
            return ways[i];
        }
    }

    return NULL;
}
