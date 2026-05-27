#include "Cache_set.h"
#include <cstdlib>
#include <string> 

using namespace std;

//Constructor
Cache_set::Cache_set(int size){
    set_size = size;
    set = new Cache_line[size];
}

//Copy Constructor
Cache_set::Cache_set(const Cache_set &other){
    this->set_size = other.set_size;
    this->set = new Cache_line[set_size];
    for(int i=0;i<set_size;i++){
        this->set[i] = other.set[i];
    }
}

//Destructor
Cache_set::~Cache_set(){
    delete[] set;
}

//= Operator Overload
Cache_set& Cache_set::operator=(const Cache_set &other){
    if(this == &other){
        return *this;
    }
    delete[] this->set;
    this->set_size = other.set_size;
    this->set = new Cache_line[this->set_size];
    for(int i=0;i<set_size;i++){
        this->set[i] = other.set[i];
    }
    return *this;   
}

Cache_line& Cache_set::getLine(int index){
    return set[index];
}

bool Cache_set::contains(int tag){
    for(int i=0;i<set_size;i++){
        if(set[i].getTag()==tag){
            return true;
        }
    }
    return false;
}

string Cache_set::toString(){
    string str;
    for(int i=0;i<set_size;i++){
        str += getLine(i).toString() + "\n";
    }
    return str;
}