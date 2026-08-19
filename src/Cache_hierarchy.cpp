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
    memory_reads=0;
    memory_writes=0;
    hierarchy_size = num_caches;
    level_config_v = level_config_v_in;
    for(int i=0;i<hierarchy_size;i++){
        int nLines_perSet = level_config_v_in.at(i).set_size;
        int nSets = level_config_v_in.at(i).num_sets;
        int nBlocks = level_config_v_in.at(i).num_blocks;
        Mapping_Technique mTech = level_config_v_in.at(i).mapping_Tech;
        Replacement_Policy rPol = level_config_v_in.at(i).replacement_Pol;
        Write_Strategy wStrat = level_config_v_in.at(i).write_Strat;

        //Construct caches
        hierarchy.push_back(make_unique<Cache>(nLines_perSet,nSets,nBlocks,mTech,rPol,wStrat));
    }
}

int Cache_hierarchy::belady_loadFile(string traceFile){
    if(hierarchy_size!=1){
        throw invalid_argument("Belady requires a single level cache!");
    }
    return hierarchy.at(0)->belady_loadFile(traceFile);
}

void Cache_hierarchy::read(int level, unsigned int address){
    //Base Case
    if(level==hierarchy_size){
        memory_reads++;
        return;
    }

    int offset, index, tag;
    hierarchy.at(level)->decompose(address,offset,index,tag);
    Miss_Type shadowMiss = hierarchy.at(level)->insertToShadowCache(address);
    Miss_Type miss_T = hierarchy.at(level)->levelLookup(index, tag);
    hierarchy.at(level)->incrementReads();
    if(miss_T==Miss_Type::Miss){
        cout << "Miss in L["+to_string(level+1)+"]" << endl;
        hierarchy.at(level)->incrementReadMiss();
        hierarchy.at(level)->classifyMiss(address,shadowMiss);
        read(level+1,address);
    }
    else if(miss_T==Miss_Type::Hit){
        cout << "Hit in L["+to_string(level+1)+"]" << endl;
        hierarchy.at(level)->incrementReadHit();
        return;
    }

    //After we miss, insert (evict if needed)
    if(hierarchy.at(level)->indexIsFull(index)){
        int dirty = hierarchy.at(level)->levelEvict(index);
        if(dirty==1){
            write(level+1,address);
        }
    }
    hierarchy.at(level)->levelInsert(index,tag);
}

void Cache_hierarchy::write(int level, unsigned int address){
    //Base Case
    if(level==hierarchy_size){
        memory_writes++;
        return;
    }

    int offset, index, tag;
    hierarchy.at(level)->decompose(address,offset,index,tag);
    Miss_Type miss_T = hierarchy.at(level)->levelContains(index, tag);
    if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Back_Write_Allocate){
        if(miss_T==Miss_Type::Miss){
            read(level,address);
        }
        hierarchy.at(level)->setDirtyBit(index,tag);
        cout << "Wrote in L["+to_string(level+1)+"]" << endl;
        hierarchy.at(level)->incrementWrites();
    }
    else if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Through_No_Write_Allocate){
        if(miss_T==Miss_Type::Hit){
            cout << "Wrote in L["+to_string(level+1)+"]" << endl;
            hierarchy.at(level)->incrementWrites();
        }
        write(level+1,address);
    }
}

int Cache_hierarchy::access(Operation op, unsigned int address){
    if(op==Operation::Read){
        read(0,address);
        cout << "" << endl;
        return 1;
    }
    else if(op==Operation::Write){
        write(0,address);
        cout << "" << endl;
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
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="L["+to_string(i+1)+"] Hits: ";
        str+=to_string(hierarchy.at(i)->getReadHits() + hierarchy.at(i)->getWriteHits())+"\n";
    }
    str+="Total Memory Reads: "+to_string(memory_reads)+"\n";
    str+="Total Memory Writes: "+to_string(memory_writes)+"\n";
    return str;
}

void Cache_hierarchy::reset(){
    for(int i=0;i<hierarchy_size;i++){
        hierarchy.at(i)->reset();
    }
    memory_reads=0;
    memory_writes=0;
}