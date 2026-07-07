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

int MRU_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int tag = lineList->front().getTag();
        lineList->pop_front();
        lineMap->erase(tag);
        return 1;
    }
    else{
        return -1;
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