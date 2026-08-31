#include "Cache.h"
#include "Cache_utils.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

#include "Direct_Cache_set.h"
#include "LRU_Cache_set.h"
#include "MRU_Cache_set.h"
#include "FIFO_Cache_set.h"
#include "LIFO_Cache_set.h"
#include "Random_Cache_set.h"
#include "LFU_Cache_set.h"
#include "Belady_Cache_set.h"
#include "SRRIP_Cache_set.h"
#include "BRRIP_Cache_set.h"

using namespace std;
using namespace Cache_utils;
using namespace Cache_types;

//Validate Constructor Inputs
void Cache::validateInput(int setSize, int numSets, int blockSize, Cache_types::Mapping_Technique mapTech, Cache_types::Replacement_Policy repPolicy){
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
    if(!isPowerOfTwo(blockSize)){
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
    else if(repPolicy==Replacement_Policy::Belady){
        return make_unique<Belady_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::SRRIP){
        return make_unique<SRRIP_Cache_set>(setSize,repPolicy);
    }
    else if(repPolicy==Replacement_Policy::BRRIP){
        return make_unique<BRRIP_Cache_set>(setSize,repPolicy);
    }
    else{
        throw invalid_argument("Provided replacement policy does not exist");
    }
}

//Constructor
Cache::Cache(int setSize, int numSets, int blockSize, Mapping_Technique mapTech, Replacement_Policy repPolicy, Write_Strategy writeStrat){

    //Validate Inputs:
    validateInput(setSize,numSets,blockSize,mapTech,repPolicy);
    
    mapping_Technique = mapTech;
    replacement_Policy = repPolicy;
    write_Strategy = writeStrat;

    cache_Size = setSize * numSets * blockSize;
    set_Size = setSize;
    num_Lines = setSize * numSets;
    num_Sets = numSets;
    block_Size = blockSize;
    eviction_Count = 0;
    
    reads = 0;
    writes = 0;
    read_hit_Count=0;
    read_miss_Count=0;
    write_hit_Count=0;
    write_miss_Count=0;
    writebacks_received=0;
    compulsory_Miss_Count=0;
    capacity_Miss_Count=0;
    conflict_Miss_Count=0;

    num_OffsetBits = log2(blockSize);
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

//Destructor
Cache::~Cache(){
    delete shadowCache;
}

//To_Strings
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

Cache_level_stats Cache::getStats(){
    Cache_level_stats x;
    x.reads = reads;
    x.read_hit_Count = read_hit_Count;
    x.read_miss_Count = read_miss_Count;
    x.writes = writes;
    x.write_hit_Count = write_hit_Count;
    x.write_miss_Count = write_miss_Count;
    x.writebacks_received = writebacks_received;
    x.eviction_Count = eviction_Count;
    x.compulsory_Miss_Count = compulsory_Miss_Count;
    x.capacity_Miss_Count = capacity_Miss_Count;
    x.conflict_Miss_Count = conflict_Miss_Count;

    return x;
}

Cache_level_details Cache::getDetails(){
    Cache_level_details x;
    x.mapping_tech = mapping_Technique;
    x.replacement_pol = replacement_Policy;
    x.write_strat = write_Strategy;
    x.cache_Size = cache_Size;
    x.num_Sets = num_Sets;
    x.total_lines = num_Lines;
    x.lines_Per_Set = set_Size;
    x.block_Size = block_Size;
    x.num_TagBits = num_TagBits;
    x.num_IndexBits = num_IndexBits;
    x.num_OffsetBits = num_OffsetBits;
    
    return x;
}

//Getters
int Cache::getReadHits(){
    return read_hit_Count;
}
int Cache::getWriteHits(){
    return write_hit_Count;
}
Write_Strategy Cache::getWriteStrat() const{
    return write_Strategy;
}
Replacement_Policy Cache::getReplacementPolicy(){
    return replacement_Policy;
}

//Functions
void Cache::decompose(unsigned int address, int &offset, int &index, int &tag){
    offset = address & ((1<<num_OffsetBits)-1);
    index = (address >> num_OffsetBits) & ((1<<num_IndexBits)-1);
    tag = (address >> (num_OffsetBits+num_IndexBits)) & ((1<<num_TagBits)-1);
}

void Cache::recompose(unsigned int &address, int index, int tag){
    address = tag;
    address <<= num_IndexBits;
    address += index;
    address <<= num_OffsetBits;
}

int Cache::belady_loadFile(string traceFile){
    if(replacement_Policy!=Replacement_Policy::Belady){
        return -1;
    }
    for(int i=0;i<num_Sets;i++){
        cacheArr.at(i)->resetTraceList();
    }
    ifstream file(traceFile);
    if (!file.is_open()) {
        return -1;
    }
    string operation;
    unsigned int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        Operation op = (operation=="R")? Operation::Read : Operation::Write;
        int offset, index, tag;
        decompose(address,offset,index,tag);
        cacheArr[index]->addFutureTag(op,tag);
    }
    return 1;
}
int Cache::belady_advanceFile(int index){
    if(replacement_Policy!=Replacement_Policy::Belady){
        return -1;
    }
    cacheArr.at(index)->advanceTraceList();
    return 1;
}
//Only runs after the line is brought into the cache level
void Cache::setDirtyBit(int index, int tag){
    cacheArr.at(index)->set_DirtyBit(tag);
}

//Access Helper Functions
Miss_Type Cache::levelLookup(int index, int tag){
    return cacheArr[index]->lookup(tag);
}

Cache_types::Evict_Return_T Cache::levelEvict(int index){
    eviction_Count++;
    return cacheArr[index]->evict();
}

void Cache::levelInsert(unsigned int address, int index, int tag){
    cacheArr[index]->insert(address, tag);
}

Miss_Type Cache::insertToShadowCache(unsigned int address){
    int blockNumber = address >> num_OffsetBits;
    Miss_Type shadowMiss_T = shadowCache->lookup(blockNumber);
    if(shadowMiss_T==Miss_Type::Miss){
        if(shadowCache->isFull()){
            shadowCache->evict();
        }
        shadowCache->insert(address, blockNumber);
    }
    return shadowMiss_T;
}

void Cache::incrementReads(){
    reads++;
}
void Cache::incrementWrites(){
    writes++;
}
void Cache::incrementReadHit(){
    read_hit_Count++;
}
void Cache::incrementWriteHit(){
    write_hit_Count++;
}
void Cache::incrementReadMiss(){
    read_miss_Count++;
}
void Cache::incrementWriteMiss(){
    write_miss_Count++;
}
void Cache::incrementWritebacksReceived(){
    writebacks_received++;
}
bool Cache::indexIsFull(int index){
    return cacheArr[index]->isFull();
}

int Cache::getCompulsoryMisses() const{
    return compulsory_Miss_Count;
}
int Cache::getConflictMisses()const{
    return conflict_Miss_Count;
}
int Cache::getCapacityMisses()const{
    return capacity_Miss_Count;
}
int Cache::getReadMisses()const{
    return read_miss_Count;
}
int Cache::getWriteMisses()const{
    return write_miss_Count;
}
int Cache::getReads()const{
    return reads;
}
int Cache::getWrites()const{
    return writes;
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
    read_hit_Count=0;
    read_miss_Count=0;
    write_hit_Count=0;
    write_miss_Count=0;
    writebacks_received=0;
    eviction_Count=0;
    compulsory_Miss_Count=0;
    capacity_Miss_Count=0;
    conflict_Miss_Count=0;
    reads=0;
    writes=0;

    blockSet.clear();
    shadowCache->reset();
}