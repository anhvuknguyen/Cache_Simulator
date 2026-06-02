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

Miss_Type Direct_Cache_set::lookup(int tag){
    if(line->getTag() == tag){
        return Miss_Type::Hit;
    }
    else{
        return Miss_Type::Miss;
    }
}

int Direct_Cache_set::evict(){
    if(isFull()){
        line->fill(0, 0, 0);
        decrementCapacity();
        return 1;
    } 
    else{
        return -1;
    }
}

int Direct_Cache_set::insert(int tag){
    if(line->getTag()!=tag){
        line->fill(tag,1,0);
        incrementCapacity();
        return 1;
    }
    else{
        return -1;
    }
}