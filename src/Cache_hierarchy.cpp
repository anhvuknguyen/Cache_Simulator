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
#include <cassert>

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
        cout<< "Main Memory Read" << endl;
        memory_reads++;
        return;
    }

    int offset, index, tag;
    hierarchy.at(level)->decompose(address,offset,index,tag);
    Miss_Type shadowMiss = hierarchy.at(level)->insertToShadowCache(address);
    Miss_Type miss_T = hierarchy.at(level)->levelLookup(index, tag);
    hierarchy.at(level)->incrementReads();
    if(miss_T==Miss_Type::Miss){
        cout << "Read Miss in L["+to_string(level+1)+"]" << endl;
        hierarchy.at(level)->incrementReadMiss();
        hierarchy.at(level)->classifyMiss(address,shadowMiss);
        read(level+1,address);
    }
    else if(miss_T==Miss_Type::Hit){
        cout << "Read Hit in L["+to_string(level+1)+"]" << endl;
        hierarchy.at(level)->incrementReadHit();
        return;
    }

    //After we miss, insert (evict if needed)
    installAt(level,index,tag);
}

void Cache_hierarchy::write(int level, unsigned int address){
    //Base Case
    if(level==hierarchy_size){
        cout<< "Main Memory Write" << endl;
        memory_writes++;
        return;
    }

    int offset, index, tag;
    hierarchy.at(level)->decompose(address,offset,index,tag);
    Miss_Type shadowMiss = hierarchy.at(level)->insertToShadowCache(address);
    Miss_Type miss_T = hierarchy.at(level)->levelLookup(index, tag);
    hierarchy.at(level)->incrementWrites();
    if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Back_Write_Allocate){
        if(miss_T==Miss_Type::Miss){
            cout << "Write Miss in L["+to_string(level+1)+"]" << endl;
            hierarchy.at(level)->incrementWriteMiss();
            hierarchy.at(level)->classifyMiss(address,shadowMiss);
            read(level+1,address);
            //Evict & Insert
            installAt(level,index,tag);
        }
        else if(miss_T==Miss_Type::Hit){
            hierarchy.at(level)->incrementWriteHit();
            cout << "Write Hit in L["+to_string(level+1)+"]" << endl;
        }
        hierarchy.at(level)->setDirtyBit(index,tag);
        cout << "Wrote in L["+to_string(level+1)+"]; Block Address: "+to_string(address) << endl;
    }
    else if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Through_No_Write_Allocate){
        if(miss_T==Miss_Type::Hit){
            hierarchy.at(level)->incrementWriteHit();
            cout << "Wrote in L["+to_string(level+1)+"]; Block Address: "+to_string(address) << endl;
        }
        else if(miss_T==Miss_Type::Miss){
            hierarchy.at(level)->incrementWriteMiss();
            hierarchy.at(level)->classifyMiss(address,shadowMiss);
            cout << "Write Miss in L["+to_string(level+1)+"]" << endl;
        }
        write(level+1,address);
    }
}

void Cache_hierarchy::writeback(int level, unsigned int address){
    //Base Case
    if(level==hierarchy_size){
        cout<< "Main Memory Write" << endl;
        memory_writes++;
        return;
    }
    assert(hierarchy.at(level)->getReplacementPolicy() != Replacement_Policy::Belady);

    int offset, index, tag;
    hierarchy.at(level)->decompose(address,offset,index,tag);
    hierarchy.at(level)->incrementWritebacksReceived();
    Miss_Type miss_T = hierarchy.at(level)->levelLookup(index, tag);

    if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Back_Write_Allocate){
        if(miss_T==Miss_Type::Miss){
            cout << "Writeback Miss in L["+to_string(level+1)+"]" << endl;
            //Evict & Insert
            installAt(level,index,tag);
        }
        else if(miss_T==Miss_Type::Hit){
            cout << "Writeback Hit in L["+to_string(level+1)+"]" << endl;
        }
        hierarchy.at(level)->setDirtyBit(index,tag);
        cout << "Wrote in L["+to_string(level+1)+"]; Block Address: "+to_string(address) << endl;
    }
    else if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Through_No_Write_Allocate){
        if(miss_T==Miss_Type::Hit){
            cout << "Wrote in L["+to_string(level+1)+"]; Block Address: "+to_string(address) << endl;
        }
        writeback(level+1,address);
    }
}

void Cache_hierarchy::installAt(int level, int index, int tag){
    if(hierarchy.at(level)->indexIsFull(index)){
        Evict_Return_T eviction_return = hierarchy.at(level)->levelEvict(index);
        unsigned int old_address = 0;
        hierarchy.at(level)->recompose(old_address,index,eviction_return.tag);
        cout<<"Eviction in L["+to_string(level+1)+"]; Block Address: "+to_string(old_address);
        if(hierarchy.at(level)->getWriteStrat()==Write_Strategy::Write_Back_Write_Allocate){
            cout<<"; Dirty Bit: "+to_string(eviction_return.dirtyBit);
        }
        cout<< endl;
        if(eviction_return.dirtyBit==1){
            unsigned int w_address = 0;
            hierarchy.at(level)->recompose(w_address,index,eviction_return.tag);
            assert(!(hierarchy.at(level)->getWriteStrat() == Write_Strategy::Write_Through_No_Write_Allocate && eviction_return.dirtyBit == 1));
            writeback(level+1,w_address);
        }
    }
    unsigned int address = 0;
    hierarchy.at(level)->recompose(address,index,tag);
    cout<<"Installment in L["+to_string(level+1)+"]; Block Address: "+to_string(address) << endl;
    hierarchy.at(level)->levelInsert(address, index,tag);
}

int Cache_hierarchy::access(Operation op, unsigned int address){
    int offset,index,tag;
    hierarchy.at(0)->decompose(address,offset,index,tag);
    cout<< "Tag: " + to_string(tag) + " Index: " + to_string(index) << endl;
    if(hierarchy_size==1 && hierarchy.at(0)->getReplacementPolicy()==Replacement_Policy::Belady){
        hierarchy.at(0)->belady_advanceFile(index);
    }
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
        str+=hierarchy.at(i)->viewCache() + "\n";
    }
    return str;
}

std::string Cache_hierarchy::getStats(){
    string str;
    vector<Cache_level_stats> stats_v;
    for(int i=0;i<hierarchy_size;i++){
        stats_v.push_back(hierarchy.at(i)->getStats());
    }
    str+="Cache Stats:\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="L["+to_string(i+1)+"]\t\t\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="        Total Reads: "+to_string(stats_v.at(i).reads)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="          Read Hits: "+to_string(stats_v.at(i).read_hit_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="        Read Misses: "+to_string(stats_v.at(i).read_miss_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="       Total Writes: "+to_string(stats_v.at(i).writes)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="         Write Hits: "+to_string(stats_v.at(i).write_hit_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="       Write Misses: "+to_string(stats_v.at(i).write_miss_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="Writebacks Received: "+to_string(stats_v.at(i).writebacks_received)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="          Evictions: "+to_string(stats_v.at(i).eviction_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="  Compulsory Misses: "+to_string(stats_v.at(i).compulsory_Miss_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="    Conflict Misses: "+to_string(stats_v.at(i).conflict_Miss_Count)+"\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="    Capacity Misses: "+to_string(stats_v.at(i).capacity_Miss_Count)+"\t\t";
    }
    str+="\n";

    str+="\nMemory  Reads: "+to_string(memory_reads);
    str+="\nMemory Writes: "+to_string(memory_writes);
    str+="\n";
    return str;
}

string Cache_hierarchy::getDetails(){
    string str;
    vector<Cache_level_details> details_v;
    for(int i=0;i<hierarchy_size;i++){
        details_v.push_back(hierarchy.at(i)->getDetails());
    }
    str+="Cache Details:\n";
    for(int i=0;i<hierarchy_size;i++){
        str+="L["+to_string(i+1)+"]\t\t\t\t\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Mapping_Technique m = details_v.at(i).mapping_tech;
        if(m==Mapping_Technique::Direct){
            str+="Mapping Technique: Direct         \t\t";
        }
        else if(m==Mapping_Technique::Fully_Associative){
            str+="Mapping Technique: Fully_Associative \t\t";
        }
        else{
            str+="Mapping Technique: Set_Associative \t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Replacement_Policy r = details_v.at(i).replacement_pol;
        if(r==Replacement_Policy::Belady){
            str+="Replacement Policy: Belady's Algorithm\t\t";
        }
        else if(r==Replacement_Policy::Direct){
            str+="                                    \t\t";
        }
        else if(r==Replacement_Policy::FIFO){
            str+="Replacement Policy: First-In First-Out\t\t";
        }
        else if(r==Replacement_Policy::LFU){
            str+="Replacement Policy: Least Frequently Used\t\t";
        }
        else if(r==Replacement_Policy::LIFO){
            str+="Replacement Policy: Last-In First-Out\t\t";
        }
        else if(r==Replacement_Policy::LRU){
            str+="Replacement Policy: Least Recently Used\t\t";
        }
        else if(r==Replacement_Policy::MRU){
            str+="Replacement Policy: Most Recently Used\t\t";
        }
        else if(r==Replacement_Policy::Random){
            str+="Replacement Policy: Random Replacement\t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Write_Strategy w = details_v.at(i).write_strat;
        if(w==Write_Strategy::Write_Back_Write_Allocate){
            str+="Write Strategy: Write-Back\t\t\t";
        }
        if(w==Write_Strategy::Write_Through_No_Write_Allocate){
            str+="Write Strategy: Write-Through\t\t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Write_Strategy w = details_v.at(i).write_strat;
        if(w==Write_Strategy::Write_Back_Write_Allocate){
            str+="                Write-Allocate\t\t\t";
        }
        if(w==Write_Strategy::Write_Through_No_Write_Allocate){
            str+="                No-Write-Allocate\t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
            str+="Cache Size: "+to_string(details_v.at(i).cache_Size)+" Bytes\t\t\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Mapping_Technique m = details_v.at(i).mapping_tech;
        if(m==Mapping_Technique::Direct){
            str+="Num. Lines: "+to_string(details_v.at(i).total_lines)+"\t\t\t\t\t";
        }

        else{
            str+="Num. Lines: "+to_string(details_v.at(i).total_lines)+"\t\t\t\t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
        Mapping_Technique m = details_v.at(i).mapping_tech;
        if(m==Mapping_Technique::Direct){
            str+="                      \t\t\t\t";
        }

        else{
            str+="Lines Per Set: "+to_string(details_v.at(i).lines_Per_Set)+"\t\t\t\t";
        }
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
            str+="Num. Sets: "+to_string(details_v.at(i).num_Sets)+" \t\t\t\t\t";
    }
    str+="\nAddress Split:\n";
    for(int i=0;i<hierarchy_size;i++){
            str+="Num. Tag Bits: "+to_string(details_v.at(i).num_TagBits)+"\t\t\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
            str+="Num. Index Bits: "+to_string(details_v.at(i).num_IndexBits)+"\t\t\t\t";
    }
    str+="\n";
    for(int i=0;i<hierarchy_size;i++){
            str+="Num. Offset Bits: "+to_string(details_v.at(i).num_OffsetBits)+"\t\t\t\t";
    }
    str+="\n";
    return str;
}

void Cache_hierarchy::reset(){
    for(int i=0;i<hierarchy_size;i++){
        hierarchy.at(i)->reset();
    }
    memory_reads=0;
    memory_writes=0;
}