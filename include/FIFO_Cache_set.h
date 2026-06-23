#ifndef FIFO_CACHE_SET_H
#define FIFO_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <queue>
#include <unordered_map>

class FIFO_Cache_set : public Cache_set{
private:
    std::queue<Cache_line> *lineQueue;
    std::unordered_map<int,Cache_line> *lineMap;
    std::string toString_Tool(std::queue<Cache_line> copy);
public:
    FIFO_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    ~FIFO_Cache_set() override;
    std::string toString() override;

    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};


#endif