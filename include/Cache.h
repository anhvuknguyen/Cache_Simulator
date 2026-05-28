#ifndef CACHE
#define CACHE

#include "Cache_set.h"
#include "Types.h"
#include <string> 
#include <vector>

class Cache{
private:
    Cache_types::Mapping_Technique mapping_Technique;
    Cache_types::Replacement_Policy replacement_Policy;
    int cache_Size;
    int num_Lines;
    int num_Sets;
    int num_Blocks;
    int num_TagBits;
    int num_IndexBits;
    int num_OffsetBits;
    std::vector<Cache_set> cacheArr;
public:
    Cache(int setSize, int numSets, int numBlocks, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy);
    std::string toString();
};

#endif