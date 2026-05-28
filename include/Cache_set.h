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
        -> Direct-Mapping/Fully Associative : set_size = 1
        -> K-way Set-Associative : set_size = K
    replacement_Policy - the type of replacement policy this set will use
    set - vector of unique Cache_line pointers
*/
class Cache_set{
private:
    int set_Size;
    Cache_types::Replacement_Policy replacement_Policy;
    std::vector<Cache_line*> set;

    Cache_line* cacheLineFactory(Cache_types::Replacement_Policy repPolicy);
public:
    Cache_set(int size,Cache_types::Replacement_Policy repPolicy);
    // Cache_set(const Cache_set &other);
    // ~Cache_set();
    // Cache_set& operator=(const Cache_set &other);
    Cache_line& getLine(int index);
    // bool contains(int tag);
    std::string toString();
};

#endif