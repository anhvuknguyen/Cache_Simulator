#ifndef CACHE_HIERARCHY_H
#define CACHE_HIERARCHY_H

#include "Cache.h"
#include "Types.h"
#include <cstdlib>
#include <vector>
#include <string>

class Cache_hierarchy{
private:
    //Array of Caches
    std::vector<std::unique_ptr<Cache>> hierarchy;
    int hierarchy_size;
    int memory_accesses;
    
    //Each index corresponds with level
    std::vector<Cache_types::Level_config> level_config_v;

    //Helper Functions
    int hierarchicalLookup(unsigned int address);
    void hierarchicalInsert(int nMisses, unsigned int address);
public:
    Cache_hierarchy(int num_caches, std::vector<Cache_types::Level_config> level_config_v_in);
    int belady_loadFile(std::string traceFile);
    int access(Cache_types::Operation op, unsigned int address);
    std::string getStats();
    std::string viewCache();
    void reset();
};

#endif