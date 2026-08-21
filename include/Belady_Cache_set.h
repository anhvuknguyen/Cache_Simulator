#ifndef BELADY_CACHE_SET_H
#define BELADY_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <cstdlib>
#include <utility>
#include <list>
#include <unordered_map>
#include <vector>

class Belady_Cache_set: public Cache_set{
private:
    std::list<Cache_line> *lineList;
    std::unordered_map<int,std::list<Cache_line>::iterator> *lineMap;
    std::vector<std::pair<Cache_types::Operation,int>> *traceList;
public:
    Belady_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~Belady_Cache_set() override;
    std::string toString() override;

    void set_DirtyBit(int tag) override;
    void clear_DirtyBit(int tag) override;

    void addFutureTag(Cache_types::Operation op, int tag) override;
    void resetTraceList() override;

    Cache_types::Miss_Type contains(int tag) override;
    Cache_types::Miss_Type lookup(int tag) override;
    Cache_types::Evict_Return_T evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif