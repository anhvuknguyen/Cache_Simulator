#include "Cache.h"
#include "Cache_utils.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <vector>

using namespace std;
using namespace Cache_utils;

//Constructor
Cache::Cache(int setSize, int numSets, int numBlocks){
    if(!isPowerOfTwo(setSize)){ 
        throw invalid_argument("Size of set must be a power of 2");
    }
    if(!isPowerOfTwo(numSets)){
        throw invalid_argument("Number of sets must be a power of 2");
    }
    if(!isPowerOfTwo(numBlocks)){
        throw invalid_argument("Number of blocks per line must be a power of 2");
    }

    cache_size = setSize * numSets;
    num_sets = numSets;
    num_Blocks = numBlocks;

    num_offsetBits = log2(numBlocks);
    num_indexBits = log2(numSets);
    num_tagBits = 32 - num_indexBits - num_offsetBits;
    if(num_tagBits < 1){
        throw invalid_argument("Not enough space for Tag Bits");
    }

    for(int i=0;i<num_sets;i++){
        cacheArr.emplace_back(setSize);
    }
}

//ToString
string Cache::toString(){
    string str;
    str += "Cache Stats: \n\t       Size: " + to_string(cache_size)  +
        "\n\t   Tag Bits: " + to_string(num_tagBits) +
        "\n\t Index Bits: " + to_string(num_indexBits) +
        "\n\tOffset Bits: " + to_string(num_offsetBits) +
        "\n";
    for(int i=0;i<num_sets;i++){
        str+= "---Set " + to_string(i) + "---\n";
        str+= cacheArr[i].toString();
    }
    return str;
}