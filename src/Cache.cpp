#include "Cache.h"
#include "Cache_utils.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <vector>

#include "Direct_Cache_set.h"
#include "LRU_Cache_set.h"

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
    if(repPolicy==Replacement_Policy::LRU){
        return make_unique<LRU_Cache_set>(setSize,repPolicy);
    }
    else{
        throw invalid_argument("Provided replacement policy does not exist");
    }
}

//Constructor
Cache::Cache(int setSize, int numSets, int numBlocks, Mapping_Technique mapTech, Replacement_Policy repPolicy) : hit_Count(0), miss_Count(0), compulsory_Miss_Count(0), capacity_Miss_Count(0), conflict_Miss_Count(0) {

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

//Debugging Strings
string Cache::viewCache(){
    string str;
    for(int i=0;i<num_Sets;i++){
        str+= "---Set " + to_string(i) + "---\n";
        str+= cacheArr[i]->toString();
    }
    return str;
}

string Cache::getStats(){
    string str;
    str += "Cache: \n\t             Size: " + to_string(cache_Size)  +
        "\n\t    Num. of Lines: " + to_string(num_Lines) +
        "\n\t         Tag Bits: " + to_string(num_TagBits) +
        "\n\t       Index Bits: " + to_string(num_IndexBits) +
        "\n\t      Offset Bits: " + to_string(num_OffsetBits) +
        "\n";
    str += "Cache Stats: \n\t             Hits: " + to_string(hit_Count) +
        "\n\t           Misses: " + to_string(miss_Count) +
        "\n\tCompulsory Misses: " + to_string(compulsory_Miss_Count) +
        "\n\t  Conflict Misses: " + to_string(conflict_Miss_Count) +
        "\n\t  Capacity Misses: " + to_string(capacity_Miss_Count) +
        "\n";
    return str;
}

int Cache::access(Cache_types::Operation op, unsigned int address){
    int offset = address & ((1<<num_OffsetBits)-1);
    int index = (address >> num_OffsetBits) & ((1<<num_IndexBits)-1);
    int tag = (address >> (num_OffsetBits+num_IndexBits)) & ((1<<num_TagBits)-1);
    
    if(op==Operation::Read){
        Miss_Type miss_T = cacheArr[index]->lookup(tag);
        if(miss_T==Miss_Type::Hit){
            hit_Count++;
            return 1;
        }
        else if(miss_T==Miss_Type::Miss){
            miss_Count++;
            classifyMiss(address);
            if(cacheArr[index]->isFull()){
                cacheArr[index]->evict();
                cacheArr[index]->insert(tag);
            }
            else{
                cacheArr[index]->insert(tag);
            }
        }
    }
    return -1;
}

//Classify Miss
void Cache::classifyMiss(unsigned int address){
    int blockAddress = address >> num_OffsetBits;
    if(!blockSet.count(blockAddress)){
        blockSet.insert(blockAddress);
        compulsory_Miss_Count++;
        return;
    }
    // ***MUST IMPLEMENT CAPACITY MISS***
    else{
        conflict_Miss_Count++;
        return;
    }
}