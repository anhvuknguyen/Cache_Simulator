#include "Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;
using namespace Cache_types;

//Factory Function for choosing a Cache_line subclass
Cache_line* Cache_set::cacheLineFactory(Cache_types::Replacement_Policy repPolicy){
    if(repPolicy==Replacement_Policy::Direct){
        return new Cache_line();
    }
    else{
        throw invalid_argument("Replacement Policy input does not exist");
    }
}

//Constructor
Cache_set::Cache_set(int size,Replacement_Policy repPolicy){
    set_Size = size;
    replacement_Policy=repPolicy;
    for(int i=0;i<set_Size;i++){
        Cache_line* ptr = cacheLineFactory(repPolicy);
        set.push_back(ptr);
    }
}

//Copy Constructor
// Cache_set::Cache_set(const Cache_set &other){
//     this->set_size = other.set_size;
//     this->replacement_Policy = other.replacement_Policy;
//     this->set = new Cache_line[set_size];
//     for(int i=0;i<set_size;i++){
//         this->set[i] = other.set[i];
//     }
// }

//Destructor
// Cache_set::~Cache_set(){
//     delete[] set;
// }

//= Operator Overload
// Cache_set& Cache_set::operator=(const Cache_set &other){
//     if(this == &other){
//         return *this;
//     }
//     delete[] this->set;
//     this->set_size = other.set_size;
//     this->set = new Cache_line[this->set_size];
//     for(int i=0;i<set_size;i++){
//         this->set[i] = other.set[i];
//     }
//     return *this;   
// }

Cache_line& Cache_set::getLine(int index){
    return *(set[index]);
}

// bool Cache_set::contains(int tag){
//     for(int i=0;i<set_size;i++){
//         if(set[i].getTag()==tag){
//             return true;
//         }
//     }
//     return false;
// }

string Cache_set::toString(){
    string str;
    for(int i=0;i<set_Size;i++){
        str += getLine(i).toString() + "\n";
    }
    return str;
}