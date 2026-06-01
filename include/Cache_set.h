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
    replacement_Policy - the type of replacement policy this set will use
*/
class Cache_set{
private:
    int set_Size;
    int capacity;
    Cache_types::Replacement_Policy replacement_Policy;

public:
    Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    virtual ~Cache_set();
    std::string virtual toString() = 0;
    Cache_types::Miss_Type virtual lookup(Cache_types::Operation op, int address) = 0;
};

#endif