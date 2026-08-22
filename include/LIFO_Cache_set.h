#ifndef LIFO_CACHE_SET_H
#define LIFO_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <stack>
#include <unordered_map>

class LIFO_Cache_set : public Cache_set{
private:
    std::stack<int> *lineStack;
    std::unordered_map<int,Cache_line> *lineMap;
    std::string toString_Tool(std::stack<int> copy);
public:
    LIFO_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    ~LIFO_Cache_set() override;
    std::string toString() override;

    void set_DirtyBit(int tag) override;
    void clear_DirtyBit(int tag) override;

    Cache_types::Miss_Type lookup(int tag) override;
    Cache_types::Evict_Return_T evict() override;
    int insert(int tag) override;
    void reset() override;
};


#endif