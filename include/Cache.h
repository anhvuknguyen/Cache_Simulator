#ifndef CACHE
#define CACHE

#include "Cache_set.h"
#include <string> 
#include <vector>

class Cache{
private:
    int cache_size;
    int num_lines;
    int num_sets;
    int num_Blocks;
    int num_tagBits;
    int num_indexBits;
    int num_offsetBits;
    std::vector<Cache_set> cacheArr;
public:
    Cache(int setSize, int numSets, int numBlocks);
    std::string toString();
};

#endif