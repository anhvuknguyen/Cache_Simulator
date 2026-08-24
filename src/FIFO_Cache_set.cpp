#include "FIFO_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <queue>
#include <unordered_map>

using namespace std;
using namespace Cache_types;

FIFO_Cache_set::FIFO_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    lineQueue = new queue<int>();
    lineMap = new unordered_map<int,Cache_line>();
}

FIFO_Cache_set::~FIFO_Cache_set(){
    delete lineQueue;
    delete lineMap;
}

std::string FIFO_Cache_set::toString(){
    return toString_Tool(*lineQueue);
}

std::string FIFO_Cache_set::toString_Tool(std::queue<int> copy){
    string str;
    while(!copy.empty()){
        str+=lineMap->at(copy.front()).toString() + "\n";
        copy.pop();
    }
    return str;
}

void FIFO_Cache_set::set_DirtyBit(int tag){
    lineMap->at(tag).setDirtyBit(1);
}

void FIFO_Cache_set::clear_DirtyBit(int tag){
    lineMap->at(tag).setDirtyBit(0);
}

Miss_Type FIFO_Cache_set::lookup(int tag){
    auto targetIt = lineMap->find(tag);
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        return Miss_Type::Hit;
    }
}

Cache_types::Evict_Return_T FIFO_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineQueue->front();
        int dirtyBit = lineMap->at(tag).getDirtyBit();

        lineQueue->pop();
        lineMap->erase(tag);
        return {tag,dirtyBit};
    }
    else{
        return {-1,-1};
    }
}

int FIFO_Cache_set::insert(unsigned int address, int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineQueue->emplace(tag);
    lineMap->insert({tag,Cache_line(tag,true,false,address)});
    return 1;
}

void FIFO_Cache_set::reset(){
    Cache_set::reset();
    while(!lineQueue->empty()){
        lineQueue->pop();
    }
    lineMap->clear();
}