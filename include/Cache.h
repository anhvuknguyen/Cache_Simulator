#ifndef CACHE
#define CACHE

#include "Cache_set.h"
#include "Types.h"
#include "LRU_Cache_set.h"
#include <string> 
#include <vector>
#include <set>

class Cache{
private:
    Cache_types::Mapping_Technique mapping_Technique;
    Cache_types::Replacement_Policy replacement_Policy;
    int cache_Size;
    int num_Lines;
    int num_Sets;
    int num_Blocks;
    
    //Address Split
    int num_TagBits;
    int num_IndexBits;
    int num_OffsetBits;

    //Cache Stats
    int hit_Count;
    int miss_Count;
    int eviction_Count;
    int compulsory_Miss_Count;
    int capacity_Miss_Count;
    int conflict_Miss_Count;

    //Actual Array of Sets
    std::vector<std::unique_ptr<Cache_set>> cacheArr;

    //Miss Classification Structures
    std::set<int> blockSet;   //For Compulsory misses
    LRU_Cache_set *shadowCache; //For Capacity misses; every miss that is neither a compulsory miss nor a miss in the shadow cache is a conflict miss 

    void validateInput(int setSize, int numSets, int numBlocks, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy);
    std::unique_ptr<Cache_set> cacheFactory(int setSize, Cache_types::Replacement_Policy repPolicy);
    void classifyMiss(unsigned int address, Cache_types::Miss_Type shadowMiss_T);
public:
    Cache(int setSize, int numSets, int numBlocks, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy);
    std::string viewCache();
    std::string viewShadowCache();
    std::string getStats();
    int access(Cache_types::Operation op, unsigned int address);
};

#endif