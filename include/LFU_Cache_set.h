#ifndef LFU_CACHE_SET_H
#define LFU_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <utility>
#include <list>
#include <unordered_map>

class LFU_Cache_set : public Cache_set{
private:
    std::list<Cache_line> *lineList;
    std::unordered_map<int,std::pair<int,std::list<Cache_line>::iterator>> *lineMap;    //Pair: Frequency, Position in List
public:
    LFU_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~LFU_Cache_set() override;
    std::string toString() override;

    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif