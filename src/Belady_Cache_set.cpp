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

int Belady_Cache_set::loadTrace(string traceFile){
    ifstream file(traceFile);
    if (!file.is_open()) {
        return -1;
    }
    string operation;
    unsigned int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        Operation op = (operation=="R")? Operation::Read : Operation::Write;
    }
}

Miss_Type Belady_Cache_set::lookup(int tag){
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