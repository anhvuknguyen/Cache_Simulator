#ifndef BELADY_CACHE_SET_H
#define BELADY_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <list>
#include <unordered_map>
#include <vector>

class Belady_Cache_set: public Cache_set{
private:
    std::list<Cache_line> *lineList;
    std::unordered_map<int,std::list<Cache_line>::iterator> *lineMap;
    std::vector<pair<Cache_types::Operation,int>> *traceList;
public:
    Belady_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~Belady_Cache_set() override;
    std::string toString() override;

    int loadTrace(std::string traceFile);
    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif