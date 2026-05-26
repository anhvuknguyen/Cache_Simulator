#include "Cache_line.h"
#include <cstdlib>

//Constructor
Cache_line::Cache_line() : tag(0), validBit(false), dirtyBit(false){}

//Fill - use when editing cache line
void Cache_line::fill(int t, bool valid, bool dirty){
    tag = t;
    validBit = valid;
    dirtyBit = dirty;
}