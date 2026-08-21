#include "LIFO_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <stack>
#include <unordered_map>

using namespace std;
using namespace Cache_types;

LIFO_Cache_set::LIFO_Cache_set(int setSize, Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    lineStack = new stack<int>();
    lineMap = new unordered_map<int,Cache_line>();
}

LIFO_Cache_set::~LIFO_Cache_set(){
    delete lineStack;
    delete lineMap;
}

string LIFO_Cache_set::toString(){
    return toString_Tool(*lineStack);
}

string LIFO_Cache_set::toString_Tool(std::stack<int> copy){
    string str;
    while(!copy.empty()){
        str+=lineMap->at(copy.top()).toString() + "\n";
        copy.pop();
    }
    return str;
}

void LIFO_Cache_set::set_DirtyBit(int tag){
    lineMap->at(tag).setDirtyBit(1);
}

void LIFO_Cache_set::clear_DirtyBit(int tag){
    lineMap->at(tag).setDirtyBit(0);
}

Miss_Type LIFO_Cache_set::contains(int tag){
    return lookup(tag);
}

Miss_Type LIFO_Cache_set::lookup(int tag){
    auto targetIt = lineMap->find(tag);
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        return Miss_Type::Hit;
    }
}

Cache_types::Evict_Return_T LIFO_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineStack->top();
        int dirtyBit = lineMap->at(tag).getDirtyBit();
        lineStack->pop();
        lineMap->erase(tag);
        return {tag,dirtyBit};
    }
    else{
        return {-1,-1};
    }
}

int LIFO_Cache_set::insert(int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineStack->emplace(tag);
    lineMap->insert({tag,Cache_line(tag,true,false)});
    return 1;
}

void LIFO_Cache_set::reset(){
    Cache_set::reset();
    while(!lineStack->empty()){
        lineStack->pop();
    }
    lineMap->clear();
}