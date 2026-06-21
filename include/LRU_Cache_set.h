#ifndef LRU_CACHE_SET_H
#define LRU_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <list>
#include <unordered_map>

class LRU_Cache_set : public Cache_set{
private:
    std::list<Cache_line> *lineList;
    std::unordered_map<int,std::list<Cache_line>::iterator> *lineMap;
public:
    LRU_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~LRU_Cache_set() override;
    std::string toString() override;

    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif