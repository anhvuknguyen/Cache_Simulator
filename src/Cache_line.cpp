#include "Cache_line.h"
#include <cstdlib>
#include <string> 

using namespace std;

//Constructor
Cache_line::Cache_line() : tag(0), validBit(false), dirtyBit(false){}

//Fill - use when editing cache line
void Cache_line::fill(int t, bool valid, bool dirty){
    tag = t;
    validBit = valid;
    dirtyBit = dirty;
}

//Getters & Setters
void Cache_line::setTag(int t){
    tag = t;
}
void Cache_line::setValidBit(bool valid){
    validBit = valid;
}
void Cache_line::setDirtyBit(bool dirty){
    dirtyBit = dirty;
}
int Cache_line::getTag(){
    return tag;
}
bool Cache_line::getValidBit(){
    return validBit;
}
bool Cache_line::getDirtyBit(){
    return dirtyBit;
}
string Cache_line::toString(){
    string vStr = getValidBit() ? "1":"0";
    string dStr = getDirtyBit() ? "1":"0";
    string tagStr = to_string(getTag());
    return "|Valid: " + vStr + " |Dirty: " + dStr + " |Tag: " + tagStr;
}