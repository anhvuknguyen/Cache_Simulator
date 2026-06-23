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
    lineQueue = new queue<Cache_line>();
    lineMap = new unordered_map<int,Cache_line>();
}

FIFO_Cache_set::~FIFO_Cache_set(){
    delete lineQueue;
    delete lineMap;
}

std::string FIFO_Cache_set::toString(){
    return toString_Tool(*lineQueue);
}

std::string FIFO_Cache_set::toString_Tool(std::queue<Cache_line> copy){
    string str;
    while(!copy.empty()){
        str+=copy.front().toString() + "\n";
        copy.pop();
    }
    return str;
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

int FIFO_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineQueue->front().getTag();
        lineQueue->pop();
        lineMap->erase(tag);
        return 1;
    }
    else{
        return -1;
    }
}

int FIFO_Cache_set::insert(int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineQueue->emplace();
    lineQueue->back().setTag(tag);
    lineQueue->back().setValidBit(true);
    lineMap->insert({tag,lineQueue->back()});
    return 1;
}

void FIFO_Cache_set::reset(){
    Cache_set::reset();
    while(!lineQueue->empty()){
        lineQueue->pop();
    }
    lineMap->clear();
}