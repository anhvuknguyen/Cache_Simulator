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
    set - vector of unique Cache_line pointers
*/
class Cache_set{
private:
    int set_Size;
    int capacity;
    Cache_types::Replacement_Policy replacement_Policy;
};

#endif