#include "SRRIP_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;
using namespace Cache_types;

SRRIP_Cache_set::SRRIP_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy): Cache_set(setSize,repPolicy){
    lineList = new vector<SRRIP_Pair>();
    insert_at = 0;
}

SRRIP_Cache_set::~SRRIP_Cache_set() {
    delete lineList;
}

string SRRIP_Cache_set::toString() {
    string str;
    vector<SRRIP_Pair>::iterator it;
    for(it = lineList->begin(); it!=lineList->end();it++){
        str+=it->line.toString()+"\n";
    }
    return str;
}

int SRRIP_Cache_set::findTag(int tag){
    for(int i=0;i<(int)lineList->size();i++){
        if(lineList->at(i).line.getTag()==tag){
            return i;
        }
    }
    return -1;
}

int SRRIP_Cache_set::findOldest(){
    for(int i=0;i<(int)lineList->size();i++){
        if(lineList->at(i).rrpv==3){
            return i;
        }
    }
    return -1;
}

void SRRIP_Cache_set::incrementAges(){
    for(int i=0;i<(int)lineList->size();i++){
        lineList->at(i).rrpv+=1;
    }
}

void SRRIP_Cache_set::set_DirtyBit(int tag){
    int i = findTag(tag);
    lineList->at(i).line.setDirtyBit(1);
}

void SRRIP_Cache_set::clear_DirtyBit(int tag){
    int i = findTag(tag);
    lineList->at(i).line.setDirtyBit(0);
}

Miss_Type SRRIP_Cache_set::lookup(int tag){
    int i = findTag(tag);
    if (i == -1){
        return Miss_Type::Miss;
    }
    else{
        lineList->at(i).rrpv = 0;
        return Miss_Type::Hit;
    }
}

Evict_Return_T SRRIP_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int index_oldest = findOldest();
        while(index_oldest == -1){
            incrementAges();
            index_oldest = findOldest();
        }
        int tag = lineList->at(index_oldest).line.getTag();
        int dirtyBit = lineList->at(index_oldest).line.getDirtyBit();
        insert_at = index_oldest;
        lineList->erase(lineList->begin()+index_oldest);
        return {tag,dirtyBit};
    }
    else{
        return {-1,-1};
    }
}

int SRRIP_Cache_set::insert(unsigned int address, int tag) {
    int i = findTag(tag);
    if(i != -1){
        return -1;
    }
    if(isFull()==false){
        lineList->push_back({Cache_line(tag,true,false,address), 2});
        incrementCapacity();
    }
    else{
        lineList->insert(lineList->begin() + insert_at,{Cache_line(tag,true,false,address), 2});
        incrementCapacity();
    }
    return 1;
}

void SRRIP_Cache_set::reset(){
    Cache_set::reset();
    lineList->clear();
}