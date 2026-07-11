#include "LFU_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <utility>
#include <limits>
#include <list>
#include <unordered_map>

using namespace std;
using namespace Cache_types;

LFU_Cache_set::LFU_Cache_set(int setSize, Replacement_Policy repPolicy) : Cache_set(setSize, repPolicy){
    lineList = new list<Cache_line>();
    lineMap = new unordered_map<int,pair<int,list<Cache_line>::iterator>>();
}

LFU_Cache_set::~LFU_Cache_set(){
    delete lineList;
    delete lineMap;
}

std::string LFU_Cache_set::toString(){
    string str;
    list<Cache_line>::iterator it;
    for(it = lineList->begin(); it!=lineList->end();it++){
        str+=it->toString()+"\n";
    }
    return str;
}

Cache_types::Miss_Type LFU_Cache_set::lookup(int tag){
    auto targetIt = lineMap->find(tag);
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        lineList->splice(lineList->begin(),*lineList,targetIt->second.second);
        (*lineMap)[tag].first += 1;     //Increment Freq
        (*lineMap)[tag].second = lineList->begin();     //Update LRU for tiebreakers
        return Miss_Type::Hit;
    }
}

int LFU_Cache_set::evict(){
    if(isFull()){
        auto it = lineList->rbegin();
        int lowestFreq = numeric_limits<int>::max();
        int tag=-1;
        while(it!=lineList->rend()){
            int tempTag = it->getTag();
            pair<int,list<Cache_line>::iterator> p = (*lineMap).at(tempTag);
            if(p.first<lowestFreq){
                tag = tempTag;
                lowestFreq = p.first;
            }
            it++;
        }
        if(tag==-1){
            return -1;
        }
        lineList->erase((*lineMap).at(tag).second);
        lineMap->erase(tag);
        decrementCapacity();
        incrementEvictions();
        return 1;
    }
    else{
        return -1;
    }
}

int LFU_Cache_set::insert(int tag){
    if(lineMap->find(tag)!=lineMap->end()){
        return -1;
    }
    incrementCapacity();
    lineList->emplace_front(tag,true,false);
    lineMap->insert({tag,{1,lineList->begin()}});
    return 1;
}

void LFU_Cache_set::reset(){
    Cache_set::reset();
    lineList->clear();
    lineMap->clear();
}