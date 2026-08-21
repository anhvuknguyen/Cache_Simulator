#ifndef RANDOM_CACHE_SET_H
#define RANDOM_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <vector>

class Random_Cache_set : public Cache_set{
private:
    std::vector<Cache_line> *lineArr;
public:
    Random_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~Random_Cache_set() override;
    std::string toString() override;

    void set_DirtyBit(int tag) override;
    void clear_DirtyBit(int tag) override;

    Cache_types::Miss_Type contains(int tag) override;
    Cache_types::Miss_Type lookup(int tag) override;
    Cache_types::Evict_Return_T evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif