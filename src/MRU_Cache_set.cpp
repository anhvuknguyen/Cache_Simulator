#include "MRU_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <list>
#include <unordered_map>

using namespace std;
using namespace Cache_types;

MRU_Cache_set::MRU_Cache_set(int setSize, Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    lineList = new list<Cache_line>();
    lineMap = new unordered_map<int,list<Cache_line>::iterator>();
}

MRU_Cache_set::~MRU_Cache_set() {
    delete lineList;
    delete lineMap;
}

string MRU_Cache_set::toString() {
    string str;
    list<Cache_line>::iterator it;
    for(it = lineList->begin(); it!=lineList->end();it++){
        str+=it->toString()+"\n";
    }
    return str;
}

void MRU_Cache_set::set_DirtyBit(int tag){
    lineMap->at(tag)->setDirtyBit(1);
}

void MRU_Cache_set::clear_DirtyBit(int tag){
    lineMap->at(tag)->setDirtyBit(0);
}

Miss_Type MRU_Cache_set::lookup(int tag){
    auto targetIt = lineMap->find(tag);
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        lineList->splice(lineList->begin(),*lineList,targetIt->second);
        (*lineMap)[tag] = lineList->begin();
        return Miss_Type::Hit;
    }
}

Cache_types::Evict_Return_T MRU_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineList->front().getTag();
        int dirtyBit = lineList->front().getDirtyBit();
        lineList->pop_front();
        lineMap->erase(tag);
        return {tag,dirtyBit};
    }
    else{
        return {-1,-1};
    }
}

int MRU_Cache_set::insert(int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineList->emplace_front(tag,true,false);
    lineMap->insert({tag,lineList->begin()});
    return 1;
}

void MRU_Cache_set::reset(){
    Cache_set::reset();
    lineList->clear();
    lineMap->clear();
}