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
    Cache_types::Write_Strategy write_Strategy;
    int cache_Size;
    int num_Lines;
    int num_Sets;
    int num_Blocks;
    
    //Address Split
    int num_TagBits;
    int num_IndexBits;
    int num_OffsetBits;

    //Cache Stats
    int reads;
    int read_hit_Count;
    int read_miss_Count;
    int writes;
    int write_hit_Count;
    int write_miss_Count;
    int writebacks_received;
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
public:
    Cache(int setSize, int numSets, int numBlocks, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy, Cache_types::Write_Strategy writeStrat);
    ~Cache();
    std::string viewCache();
    std::string viewShadowCache();
    Cache_types::Cache_level_stats getStats();

    int getReadHits();
    int getWriteHits();
    Cache_types::Write_Strategy getWriteStrat();
    Cache_types::Replacement_Policy getReplacementPolicy();

    void decompose(unsigned int address, int& offset, int& index, int& tag);
    void recompose(unsigned int& address, int index, int tag);
    int belady_loadFile(std::string traceFile);
    void setDirtyBit(int index,int tag);

    Cache_types::Miss_Type levelContains(int index,int tag);
    Cache_types::Miss_Type levelLookup(int index, int tag);
    Cache_types::Evict_Return_T levelEvict(int index);
    void levelInsert(int index, int tag);
    Cache_types::Miss_Type insertToShadowCache(unsigned int address);

    void incrementReads();
    void incrementWrites();
    void incrementReadHit();
    void incrementReadMiss();
    void incrementWriteHit();
    void incrementWriteMiss();
    void incrementWritebacksReceived();

    void classifyMiss(unsigned int address, Cache_types::Miss_Type shadowMiss_T);
    bool indexIsFull(int index);
    void reset();
};

#endif