#ifndef DIRECT_CACHE_SET_H
#define DIRECT_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"

class Direct_Cache_set : public Cache_set{
private:
    Cache_line* line;
public:
    Direct_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    ~Direct_Cache_set() override;
    std::string toString() override;

    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};

#endif