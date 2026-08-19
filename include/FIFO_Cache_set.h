#ifndef FIFO_CACHE_SET_H
#define FIFO_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <queue>
#include <unordered_map>

class FIFO_Cache_set : public Cache_set{
private:
    std::queue<int> *lineQueue;                     //Queue of tags
    std::unordered_map<int,Cache_line> *lineMap;    //Actually holds the information
    std::string toString_Tool(std::queue<int> copy);
public:
    FIFO_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    ~FIFO_Cache_set() override;
    std::string toString() override;

    void set_DirtyBit(int tag) override;
    void clear_DirtyBit(int tag) override;

    Cache_types::Miss_Type contains(int tag) override;
    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};


#endif