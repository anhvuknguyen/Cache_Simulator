#include "Direct_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;
using namespace Cache_types;

Direct_Cache_set::Direct_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    line = new Cache_line();
}

Direct_Cache_set::~Direct_Cache_set(){
    delete line;
}

string Direct_Cache_set::toString(){
    return line->toString()+"\n";
}

void Direct_Cache_set::set_DirtyBit(int tag){
    line->setDirtyBit(1);
}

void Direct_Cache_set::clear_DirtyBit(int tag){
    line->setDirtyBit(0);
}

Miss_Type Direct_Cache_set::contains(int tag){
    return lookup(tag);
}

Miss_Type Direct_Cache_set::lookup(int tag){
    if(line->getTag() == tag && line->getValidBit()==1){
        return Miss_Type::Hit;
    }
    else{
        return Miss_Type::Miss;
    }
}

Cache_types::Evict_Return_T Direct_Cache_set::evict(){
    if(isFull()){
        int tag = line->getTag();
        int dirtyBit = line->getDirtyBit();
        
        line->fill(0, 0, 0);
        incrementEvictions();
        decrementCapacity();
        return {tag,dirtyBit};
    } 
    else{
        return {-1,-1};
    }
}

int Direct_Cache_set::insert(int tag){
    if(line->getTag()!=tag || line->getValidBit()==0){
        line->fill(tag,1,0);
        incrementCapacity();
        return 1;
    }
    else{
        return -1;
    }
}

void Direct_Cache_set::reset(){
    Cache_set::reset();
    line->fill(0,false,false);
}