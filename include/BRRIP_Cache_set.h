#ifndef BRRIP_CACHE_SET_H
#define BRRIP_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <vector>
#include <utility>
#include <unordered_map>

class BRRIP_Cache_set : public Cache_set{
private:
    std::vector<Cache_types::SRRIP_Pair> *lineList;
    int findTag(int tag);
    int findOldest();
    void incrementAges();
    int insert_at;
public:
    BRRIP_Cache_set(int setSize,Cache_types::Replacement_Policy repPolicy);
    ~BRRIP_Cache_set() override;
    std::string toString() override;

    void set_DirtyBit(int tag) override;
    void clear_DirtyBit(int tag) override;

    Cache_types::Miss_Type lookup(int tag) override;
    Cache_types::Evict_Return_T evict() override;
    int insert(unsigned int address, int tag) override;
    void reset() override;
};
#endif