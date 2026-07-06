#ifndef LIFO_CACHE_SET_H
#define LIFO_CACHE_SET_H

#include "Cache_set.h"
#include "Cache_line.h"
#include "Types.h"
#include <stack>
#include <unordered_map>

class LIFO_Cache_set : public Cache_set{
private:
    std::stack<Cache_line> *lineStack;
    std::unordered_map<int,Cache_line> *lineMap;
    std::string toString_Tool(std::stack<Cache_line> copy);
public:
    LIFO_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy);
    ~LIFO_Cache_set() override;
    std::string toString() override;

    Cache_types::Miss_Type lookup(int tag) override;
    int evict() override;
    int insert(int tag) override;
    void reset() override;
};


#endif