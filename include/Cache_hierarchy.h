#ifndef CACHE_HIERARCHY_H
#define CACHE_HIERARCHY_H

#include "Cache.h"
#include "Types.h"
#include <vector>

class Cache_hierarchy{
private:
    std::vector<Cache> hierarchy;
    int num_Caches;

    std::vector<Cache_types::Mapping_Technique> mapping_Techniques;
    std::vector<Cache_types::Replacement_Policy> replacement_Policies;
    std::vector<int> num_lines_v;
    std::vector<int> num_sets_v;
    std::vector<int> num_blocks_v;
public:
    Cache_hierarchy(std::vector<int> num_lines_v_in, std::vector<int> num_sets_v_in, std::vector<int> num_blocks_v_in, std::vector<Cache_types::Mapping_Technique> mapping_Techniques_in, std::vector<Cache_types::Replacement_Policy> replacement_Policies_in);
    ~Cache_hierarchy();
    int access(Cache_types::Operation op, unsigned int address);
    void reset();
};

#endif