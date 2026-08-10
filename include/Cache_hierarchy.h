#ifndef CACHE_HIERARCHY_H
#define CACHE_HIERARCHY_H

#include "Cache.h"
#include "Types.h"
#include <cstdlib>
#include <vector>


class Cache_hierarchy{
private:
    //Array of Caches
    std::vector<std::unique_ptr<Cache>> hierarchy;
    int size;

    //Data; each index corresponds with the index in Cache
    std::vector<int> num_lines_per_set_v;
    std::vector<int> num_sets_v;
    std::vector<int> num_blocks_v;
    std::vector<Cache_types::Mapping_Technique> mapping_Techniques;
    std::vector<Cache_types::Replacement_Policy> replacement_Policies;
public:
    Cache_hierarchy(int num_caches, std::vector<int> num_lines_v_in, std::vector<int> num_sets_v_in, std::vector<int> num_blocks_v_in, std::vector<Cache_types::Mapping_Technique> mapping_Techniques_in, std::vector<Cache_types::Replacement_Policy> replacement_Policies_in);
    int access(Cache_types::Operation op, unsigned int address);
    void reset();
};

#endif