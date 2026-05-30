#include "Cache.h"
#include "Cache_utils.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <vector>

using namespace std;
using namespace Cache_utils;
using namespace Cache_types;

//Validate Constructor Inputs
void Cache::validateInput(int setSize, int numSets, int numBlocks, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy){
    //Direct mapping does not have a replacement policy
    if(mapTech==Mapping_Technique::Direct && repPolicy!=Replacement_Policy::Direct){
        throw invalid_argument("Direct Mapping Technique must use Direct Replacement Policy");
    }
    //Direct mapping has no sets, so setSize = 1
    if(mapTech==Mapping_Technique::Direct && setSize!=1){
        throw invalid_argument("Direct Mapping Technique must have setSize of 1");
    }

    //Fully Associative mapping has one set, so setSize = N
    if(mapTech==Mapping_Technique::Fully_Associative && numSets!=1){
        throw invalid_argument("Fully_Associative Mapping Technique must have 1 set");
    }

    //Assert that all numbers are powers of 2
    if(!isPowerOfTwo(setSize)){ 
        throw invalid_argument("Size of set must be a power of 2");
    }
    if(!isPowerOfTwo(numSets)){
        throw invalid_argument("Number of sets must be a power of 2");
    }
    if(!isPowerOfTwo(numBlocks)){
        throw invalid_argument("Number of blocks per line must be a power of 2");
    }
}

//Constructor Factory
unique_ptr<Cache_set> Cache::cacheFactory(int setSize, Replacement_Policy repPolicy){
    if(repPolicy==Replacement_Policy::Direct){
        return make_unique<Direct_Cache_set>(setSize,repPolicy);
    }
    else{
        throw invalid_argument("Provided replacement policy does not exist");
    }
}

//Constructor
Cache::Cache(int setSize, int numSets, int numBlocks, Mapping_Technique mapTech, Replacement_Policy repPolicy){

    //Validate Inputs:
    validateInput(setSize,numSets,numBlocks,mapTech,repPolicy);
    
    mapping_Technique = mapTech;
    replacement_Policy = repPolicy;

    cache_Size = setSize * numSets * numBlocks;
    num_Lines = setSize * numSets;
    num_Sets = numSets;
    num_Blocks = numBlocks;

    num_OffsetBits = log2(numBlocks);
    num_IndexBits = log2(numSets);
    num_TagBits = 32 - num_IndexBits - num_OffsetBits;
    if(num_TagBits < 1){
        throw invalid_argument("Not enough space for Tag Bits");
    }

    for(int i=0;i<num_Sets;i++){
        cacheArr.push_back(cacheFactory(setSize, repPolicy));
    }
}

//ToString
string Cache::toString(){
    string str;
    str += "Cache Stats: \n\t         Size: " + to_string(cache_Size)  +
        "\n\tNum. of Lines: " + to_string(num_Lines) +
        "\n\t     Tag Bits: " + to_string(num_TagBits) +
        "\n\t   Index Bits: " + to_string(num_IndexBits) +
        "\n\t  Offset Bits: " + to_string(num_OffsetBits) +
        "\n";
    for(int i=0;i<num_Sets;i++){
        str+= "---Set " + to_string(i) + "---\n";
        str+= cacheArr[i]->toString();
    }
    return str;
}