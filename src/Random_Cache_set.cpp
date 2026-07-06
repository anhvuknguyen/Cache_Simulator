#include "Random_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;
using namespace Cache_types;

Random_Cache_set::Random_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy) : Cache_set(setSize, repPolicy){
    lineArr = new vector<Cache_line>;
}

Random_Cache_set::~Random_Cache_set(){
    delete lineArr;
}

std::string Random_Cache_set::toString(){
    string str;
    for(int i=0;i<((int)lineArr->size());i++){
        str+=lineArr->at(i).toString()+="\n";
    }
    return str;
}

Cache_types::Miss_Type Random_Cache_set::lookup(int tag){
    if(lineArr->empty())return Miss_Type::Miss;
    for(int i=0;i<((int)lineArr->size());i++){
        if(lineArr->at(i).getTag() == tag){
            return Miss_Type::Hit;
        }
    }
    return Miss_Type::Miss;
}

int Random_Cache_set::evict(){
    if(isFull()){
        decrementCapacity();
        incrementEvictions();
        int index = rand() % get_set_Size();
        lineArr->erase(lineArr->begin() + index);
        return 1;
    }
    else{
        return -1;
    }
}

int Random_Cache_set::insert(int tag){
    if(lookup(tag)==Miss_Type::Hit){ ////ISSUE
        return -1;
    }
    incrementCapacity();
    lineArr->emplace_back(tag,true,false);
    return 1;
}

void Random_Cache_set::reset(){
    Cache_set::reset();
    lineArr->clear();
}