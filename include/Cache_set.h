#ifndef CACHE_SET_H
#define CACHE_SET_H

#include "Cache_line.h"

/*
Cache_set class: 
    set_size - size of set
        -> Direct-Mapping/Fully Associative : set_size = 1
        -> K-way Set-Associative : set_size = K
    set - array of Cache_lines of size N
*/
class Cache_set{
private:
    int set_size;
    Cache_line* set;
public:
    Cache_set(int size);
    Cache_set(const Cache_set &other);
    ~Cache_set();
    Cache_set& operator=(const Cache_set &other);
    Cache_line& getLine(int index);
    bool contains(int tag);
};

#endif