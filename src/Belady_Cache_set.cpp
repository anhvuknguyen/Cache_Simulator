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
    traceList->erase(traceList->begin());
    if(targetIt==lineMap->end()){
        return Miss_Type::Miss;
    }
    else{
        lineList->splice(lineList->begin(),*lineList,targetIt->second);
        (*lineMap)[tag] = lineList->begin();
        return Miss_Type::Hit;
    }
}

void Belady_Cache_set::evictionHelper(list<Cache_line> lineListCopy, vector<int> &inCache){
    list<Cache_line>::iterator it = lineListCopy.begin();
    while(it!=lineListCopy.end()){
        inCache.push_back(it->getTag());
    }
}

int Belady_Cache_set::evict(){
    if(isFull()){
        vector<int> inCache;
        evictionHelper(*lineList,inCache);
        //until inCache has only 1 element or the end of the traceList is reached
        return 1;
    }
    else{
        return -1;
    }
}