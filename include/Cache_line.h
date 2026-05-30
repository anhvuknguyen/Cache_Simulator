#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <string> 

/*
Cache_line class: 
    tag - tag bits of address currently occupying cache line
    validBit - identifies if cache line has been filled
    dirtyBit - identifies edits in the cache line for write backs
*/
class Cache_line{
private:
    int tag;
    bool validBit;
    bool dirtyBit;
public:
    Cache_line();
    void fill(int t, bool valid, bool dirty);
    void setTag(int t);
    void setValidBit(bool valid);
    void setDirtyBit(bool dirty);
    int getTag();
    bool getValidBit();
    bool getDirtyBit();
    std::string toString();
};

class Cache_line_FIFO : public Cache_line{
private:
    bool isHead;
public:
    Cache_line_FIFO();
};

#endif