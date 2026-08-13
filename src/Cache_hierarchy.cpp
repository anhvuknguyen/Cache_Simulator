#include "Cache_hierarchy.h"
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

using namespace std;
using namespace Cache_utils;
using namespace Cache_types;

Cache_hierarchy::Cache_hierarchy(int num_caches, std::vector<Level_config> level_config_v_in){
    hierarchy_size = num_caches;
    level_config_v = level_config_v_in;
    for(int i=0;i<hierarchy_size;i++){
        int nLines_perSet = level_config_v_in.at(i).set_size;
        int nSets = level_config_v_in.at(i).num_sets;
        int nBlocks = level_config_v_in.at(i).num_blocks;
        Mapping_Technique mTech = level_config_v_in.at(i).mapping_Tech;
        Replacement_Policy rPol = level_config_v_in.at(i).replacement_Pol;

        //Construct caches
        hierarchy.push_back(make_unique<Cache>(nLines_perSet,nSets,nBlocks,mTech,rPol));
    }
}

int Cache_hierarchy::belady_loadFile(string traceFile){
    if(hierarchy_size!=1){
        throw invalid_argument("Belady requires a single level cache!");
    }
    return hierarchy.at(0)->belady_loadFile(traceFile);
}

int Cache_hierarchy::hierarchicalLookup(unsigned int address){
    vector<Miss_Type> shadow_miss_v;
    //Insert to all 3 shadow Caches
    for(int i=0;i<hierarchy_size;i++){
        Miss_Type shadowMiss = hierarchy.at(i)->insertToShadowCache(address);
        shadow_miss_v.push_back(shadowMiss);
    }
    
    int offset, index, tag;
    int nMisses=0;    //Count how many misses we have so we know how many inserts we need to do
    for(int i=0;i<hierarchy_size;i++){
        hierarchy.at(i)->decompose(address,offset,index,tag); //fills the respective bits
        Miss_Type miss_T = hierarchy.at(i)->levelLookup(index,tag);
        if(miss_T==Miss_Type::Hit){
            cout << "Hit in L["+to_string(i+1)+"]" << endl;
            hierarchy.at(i)->incrementHit();
            return nMisses;
        }
        else if(miss_T==Miss_Type::Miss){
            cout << "Miss in L["+to_string(i+1)+"]" << endl;
            hierarchy.at(i)->incrementMiss();
            hierarchy.at(i)->classifyMiss(address,shadow_miss_v.at(i));
            nMisses++;
        }
    }
    return nMisses;
}

void Cache_hierarchy::hierarchicalInsert(int nMisses, unsigned int address){
    int offset, index, tag;
    for(int i=nMisses-1;i>=0;i--){
        hierarchy.at(i)->decompose(address,offset,index,tag);
        if(hierarchy.at(i)->indexIsFull(index)){
            hierarchy.at(i)->levelEvict(index);
        }
        hierarchy.at(i)->levelInsert(index,tag);
    }
}

int Cache_hierarchy::access(Operation op, unsigned int address){
    if(op==Operation::Read){
        //Handle All 3 lookups
        int nMisses = hierarchicalLookup(address); //Counts how many misses (and therefore, how many hits)
        hierarchicalInsert(nMisses,address);
        return 1;
    }
    
    return -1;
}

std::string Cache_hierarchy::viewCache(){
    string str;
    for(int i=0;i<hierarchy_size;i++){
        str+="L["+to_string(i+1)+"]:\n";
        str+=hierarchy.at(i)->viewCache();
    }
    return str;
}

std::string Cache_hierarchy::getStats(){
    string str;
    for(int i=0;i<hierarchy_size;i++){
        str+="L["+to_string(i+1)+"]:\n";
        str+=hierarchy.at(i)->getStats();
    }
    return str;
}

void Cache_hierarchy::reset(){
    for(int i=0;i<hierarchy_size;i++){
        hierarchy.at(i)->reset();
    }
}