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

Cache_hierarchy::Cache_hierarchy(int num_caches, std::vector<int> set_size_v_in, std::vector<int> num_sets_v_in, std::vector<int> num_blocks_v_in, std::vector<Mapping_Technique> mapping_Techniques_in, std::vector<Replacement_Policy> replacement_Policies_in){
    hierarchy_size = num_caches;

    for(int i=0;i<hierarchy_size;i++){
        int nLines_perSet = set_size_v_in.at(i);
        int nSets = num_sets_v_in.at(i);
        int nBlocks = num_blocks_v_in.at(i);
        Mapping_Technique mTech = mapping_Techniques_in.at(i);
        Replacement_Policy rPol = replacement_Policies_in.at(i);

        //Populate respective vectors
        set_size_v.emplace_back(nLines_perSet);
        num_sets_v.emplace_back(nSets);
        num_blocks_v.emplace_back(nBlocks);
        mapping_Techniques.emplace_back(mTech);
        replacement_Policies.emplace_back(rPol);

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
    int offset, index, tag;
    int nMisses;    //Count how many misses we have so we know how many inserts we need to do
    for(int i=0;i<hierarchy_size;i++){
        hierarchy.at(i)->decompose(address,offset,index,tag); //fills the respective bits
        Miss_Type miss_T = hierarchy.at(i)->levelLookup(index,tag);
        if(miss_T==Miss_Type::Hit){
            cout << "Hit in L["+to_string(i+1)+"]" << endl;
            hierarchy.at(i)->incrementHit();
        }
    }
}

int Cache_hierarchy::access(Operation op, unsigned int address){
    vector<Miss_Type> shadow_miss_v;
    if(op==Operation::Read){
        //Insert to all 3 shadow Caches
        for(int i=0;i<hierarchy_size;i++){
            Miss_Type shadowMiss = hierarchy.at(i)->insertToShadowCache(address);
            shadow_miss_v.push_back(shadowMiss);
        }
        //Handle All 3 lookups
        hierarchicalLookup(address);
    }
    
    return 0;
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
        hierarchy.at(i).reset();
    }
}