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
    lineStack = new stack<Cache_line>();
    lineMap = new unordered_map<int,Cache_line>();
}

LIFO_Cache_set::~LIFO_Cache_set(){
    delete lineStack;
    delete lineMap;
}

string LIFO_Cache_set::toString(){
    return toString_Tool(*lineStack);
}

string LIFO_Cache_set::toString_Tool(std::stack<Cache_line> copy){
    string str;
    while(!copy.empty()){
        str+=copy.top().toString() + "\n";
        copy.pop();
    }
    return str;
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

int LIFO_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineStack->top().getTag();
        lineStack->pop();
        lineMap->erase(tag);
        return 1;
    }
    else{
        return -1;
    }
}

int LIFO_Cache_set::insert(int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineStack->emplace(tag,true,false);
    lineMap->insert({tag,lineStack->top()});
    return 1;
}

void LIFO_Cache_set::reset(){
    Cache_set::reset();
    while(!lineStack->empty()){
        lineStack->pop();
    }
    lineMap->clear();
}