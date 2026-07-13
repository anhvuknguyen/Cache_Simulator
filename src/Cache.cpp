#include "Cache.h"
#include "Cache_utils.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <vector>
#include <iostream>

#include "Direct_Cache_set.h"
#include "LRU_Cache_set.h"
#include "MRU_Cache_set.h"
#include "FIFO_Cache_set.h"
#include "LIFO_Cache_set.h"
#include "Random_Cache_set.h"
#include "LFU_Cache_set.h"

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
    else if(repPolicy==Replacement_Policy::LRU){
        return make_unique<LRU_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::MRU){
        return make_unique<MRU_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::FIFO){
        return make_unique<FIFO_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::LIFO){
        return make_unique<LIFO_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::Random){
        return make_unique<Random_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::LFU){
        return make_unique<LFU_Cache_set>(setSize,repPolicy);
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
    eviction_Count = 0;

    num_OffsetBits = log2(numBlocks);
    num_IndexBits = log2(numSets);
    num_TagBits = 32 - num_IndexBits - num_OffsetBits;
    if(num_TagBits < 1){
        throw invalid_argument("Not enough space for Tag Bits");
    }

    for(int i=0;i<num_Sets;i++){
        cacheArr.push_back(cacheFactory(setSize, repPolicy));
    }

    shadowCache = new LRU_Cache_set(num_Lines,Replacement_Policy::LRU);
}

//Debugging Strings
string Cache::viewCache(){
    string str;
    for(int i=0;i<num_Sets;i++){
        str+= "---Set " + to_string(i) + "---  Evictions: "+to_string(cacheArr[i]->get_Eviction_Count())+"\n";
        str+= cacheArr[i]->toString();
    }
    return str;
}

string Cache::viewShadowCache(){
    return shadowCache->toString();
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
        "\n\t        Evictions: " + to_string(eviction_Count) +
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
        //Handle shadowCache
        int blockNumber = address >> num_OffsetBits;
        Miss_Type shadowMiss_T = shadowCache->lookup(blockNumber);
        if(shadowMiss_T==Miss_Type::Miss){
            if(shadowCache->isFull()){
                shadowCache->evict();
            }
            shadowCache->insert(blockNumber);
        }
        //Handle cacheArr
        Miss_Type miss_T = cacheArr[index]->lookup(tag);
        if(miss_T==Miss_Type::Hit){
            cout << "Hit" << endl;
            hit_Count++;
            return 1;
        }
        else if(miss_T==Miss_Type::Miss){
            miss_Count++;
            classifyMiss(address, shadowMiss_T);
            if(cacheArr[index]->isFull()){
                cacheArr[index]->evict();
                eviction_Count++;
            }
            cacheArr[index]->insert(tag);
        }
    }
    return -1;
}

//Classify Miss
void Cache::classifyMiss(unsigned int address, Miss_Type shadowMiss_T){
    int blockAddress = address >> num_OffsetBits;
    if(!blockSet.count(blockAddress)){
        blockSet.insert(blockAddress);
        cout << "Compulsory Miss" << endl;
        compulsory_Miss_Count++;
        return;
    }
    else if(shadowMiss_T==Miss_Type::Miss){
        cout << "Capacity Miss" << endl;
        capacity_Miss_Count++;
        return;
    }
    else{
        if(num_Sets==1){
            cout << "Capacity Miss" << endl;
            capacity_Miss_Count++;
            return;
        }
        cout << "Conflict Miss" << endl;
        conflict_Miss_Count++;
        return;
    }
}

void Cache::reset(){
    for(int i=0;i<num_Sets;i++){
        cacheArr[i]->reset();
    }
    hit_Count=0;
    miss_Count=0;
    eviction_Count=0;
    compulsory_Miss_Count=0;
    capacity_Miss_Count=0;
    conflict_Miss_Count=0;

    blockSet.clear();
    shadowCache->reset();
}