#include "Direct_Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;
using namespace Cache_types;

Direct_Cache_set::Direct_Cache_set(int setSize, Cache_types::Replacement_Policy repPolicy) : Cache_set(setSize,repPolicy){
    line = new Cache_line();
}

Direct_Cache_set::~Direct_Cache_set(){
    delete line;
}

string Direct_Cache_set::toString(){
    return line->toString()+"\n";
}