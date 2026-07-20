#include "Belady_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <list>
#include <unordered_map>
#include <assert.h>
#include <vector>

using namespace std;
using namespace Cache_types;
namespace fs = std::filesystem;

Belady_Cache_set::Belady_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    lineList = new list<Cache_line>();
    lineMap = new unordered_map<int,list<Cache_line>::iterator>();
    traceList = new vector<pair<Operation,int>>();
}

Belady_Cache_set::~Belady_Cache_set() {
    delete lineList;
    delete lineMap;
    delete traceList;
}

string Belady_Cache_set::toString() {
    string str;
    list<Cache_line>::iterator it;
    for(it = lineList->begin(); it!=lineList->end();it++){
        str+=it->toString()+"\n";
    }
    return str;
}

void Belady_Cache_set::addFutureTag(Cache_types::Operation op, int tag){
    traceList->emplace_back(op,tag);
}

Miss_Type Belady_Cache_set::lookup(int tag){
    auto targetIt = lineMap->find(tag);
    //every access pops exactly one entry; lookup always precedes evict
    if(traceList->empty()==false){ 
        assert(!traceList->empty());
        traceList->erase(traceList->begin());
    }
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        lineList->splice(lineList->begin(),*lineList,targetIt->second);
        (*lineMap)[tag] = lineList->begin();
        return Miss_Type::Hit;
    }
}

int Belady_Cache_set::evict(){
    if(isFull()){
        list<int> inCache;
        for(Cache_line& line : *lineList){
            inCache.push_back(line.getTag());
        }
        //until inCache has only 1 element or the end of the traceList is reached
        for(pair<Operation,int>& p : *traceList){
            if(inCache.size() == 1){
                break;
            }
            int tag = p.second;
            list<int>::iterator it = inCache.begin();
            while(it!=inCache.end()){
                if(*it == tag){
                    inCache.erase(it);
                    break;
                }
                ++it;
            }
        }
        lineList->erase(lineMap->at(inCache.back()));
        lineMap->erase(inCache.back());
        decrementCapacity();
        incrementEvictions();
        return 1;
    }
    else{
        return -1;
    }
}