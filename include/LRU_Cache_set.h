#ifndef LRU_CACHE_SET_H
#define LRU_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <vector>

class LRU_Cache_set : public Cache_set{
private:
    std::vector<Cache_line> queue;
    int size;
    int capacity;
public:
    LRU_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
};

#endif