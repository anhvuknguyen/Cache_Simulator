#ifndef CACHE_SET_H
#define CACHE_SET_H

#include "Cache_line.h"
#include "Types.h"
#include <string> 
#include <memory>
#include <vector>

/*
Cache_set class: 
    set_size - size of set
        -> Direct-Mapping : set_size = 1
        -> Fully Associative : set_size = N
        -> K-way Set-Associative : set_size = K
    capacity - how many cache lines are filled in the set
    replacement_Policy - the type of replacement policy this set will use

    lookup() - will return either hit or miss. Cache class will handle miss classification
*/
class Cache_set{
private:
    int set_Size;
    int capacity;
    Cache_types::Replacement_Policy replacement_Policy;

public:
    Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    bool isFull();
    void incrementCapacity();
    void decrementCapacity();
    int get_set_Size();
    virtual ~Cache_set();
    std::string virtual toString() = 0;

    Cache_types::Miss_Type virtual lookup(int tag) = 0;
    int virtual evict() = 0;
    int virtual insert(int tag) = 0;
};

#endif