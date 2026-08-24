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
#include <iomanip>
#include <sstream>
#include <algorithm>
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
    std::vector<Cache_level_stats> stats_v;
    for(int i=0;i<hierarchy_size;i++){
        stats_v.push_back(hierarchy.at(i)->getStats());
    }

    const int labelW = 20;
    int colW = 8;
    for(int i=0;i<hierarchy_size;i++){
        const Cache_level_stats &s = stats_v.at(i);
        int widest = std::max({s.reads, s.read_hit_Count, s.read_miss_Count,
                                s.writes, s.write_hit_Count, s.write_miss_Count,
                                s.writebacks_received, s.eviction_Count,
                                s.compulsory_Miss_Count, s.conflict_Miss_Count,
                                s.capacity_Miss_Count});
        int w = static_cast<int>(std::to_string(widest).length()) + 4;
        if(w > colW) colW = w;
    }

    std::ostringstream oss;

    auto row = [&](const std::string &label, int Cache_level_stats::*field){
        oss << std::right << std::setw(labelW) << label;
        for(int i=0;i<hierarchy_size;i++){
            oss << std::left << std::setw(colW) << stats_v.at(i).*field;
        }
        oss << "\n";
    };

    oss << "Cache Stats:\n";
    oss << std::right << std::setw(labelW) << "";
    for(int i=0;i<hierarchy_size;i++){
        oss << std::left << std::setw(colW) << ("L["+std::to_string(i+1)+"]");
    }
    oss << "\n";
    
    row("Total Reads: ",         &Cache_level_stats::reads);
    row("Read Hits: ",           &Cache_level_stats::read_hit_Count);
    row("Read Misses: ",         &Cache_level_stats::read_miss_Count);
    row("Total Writes: ",        &Cache_level_stats::writes);
    row("Write Hits: ",          &Cache_level_stats::write_hit_Count);
    row("Write Misses: ",        &Cache_level_stats::write_miss_Count);
    row("Writebacks Given: ", &Cache_level_stats::writebacks_received);
    row("Evictions: ",           &Cache_level_stats::eviction_Count);
    row("Compulsory Misses: ",   &Cache_level_stats::compulsory_Miss_Count);
    row("Conflict Misses: ",     &Cache_level_stats::conflict_Miss_Count);
    row("Capacity Misses: ",     &Cache_level_stats::capacity_Miss_Count);

    oss << "\n" << std::right << std::setw(labelW) << "Memory  Reads: "  << memory_reads;
    oss << "\n" << std::right << std::setw(labelW) << "Memory Writes: " << memory_writes;
    oss << "\n";

    return oss.str();
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

void Cache_hierarchy::checkInvariants(int trace_read_count, int trace_write_count) const{

    const int total_refs = trace_read_count + trace_write_count;

    // ---- Per-level: the 3Cs must account for every demand miss ----------
    // Catches a miss path that forgets to call classifyMiss (e.g. the
    // write-through branch), or one that classifies a non-demand access.
    for (int n = 0; n < hierarchy_size; ++n) {
        const Cache* L = hierarchy.at(n).get();
        const int classified = L->getCompulsoryMisses()
                             + L->getConflictMisses()
                             + L->getCapacityMisses();
        const int demand_misses = L->getReadMisses() + L->getWriteMisses();
        assert(classified == demand_misses
               && "3C totals do not match demand misses at this level");
    }

    // ---- L1 sees exactly the program's reference stream ------------------
    // Write-allocate fetches descend to L2+, never re-entering L1's counters,
    // so L1's totals must equal the trace's own line counts.
    assert(hierarchy.at(0)->getReads()  == trace_read_count
           && "L1 read count does not match the number of R lines in the trace");
    assert(hierarchy.at(0)->getWrites() == trace_write_count
           && "L1 write count does not match the number of W lines in the trace");

    // ---- Demand traffic between adjacent levels --------------------------
    // A write-back level sends write MISSES down as reads (write-allocate fetch).
    // A write-through level sends EVERY write down as a write, hits included.
    // Writebacks appear on neither side: they are not program references.
    for (int n = 0; n + 1 < hierarchy_size; ++n) {
        const Cache* upper = hierarchy.at(n).get();
        const Cache* lower = hierarchy.at(n + 1).get();

        int descending = upper->getReadMisses();
        if (upper->getWriteStrat() == Write_Strategy::Write_Through_No_Write_Allocate)
            descending += upper->getWrites();
        else
            descending += upper->getWriteMisses();

        assert(lower->getReads() + lower->getWrites() == descending
               && "demand accesses at this level do not match what the level above sent down");
    }

    // ---- A write-through level never holds dirty data --------------------
    // If one ever issues a writeback, the write policy plumbing is broken.
    for (int n = 0; n < hierarchy_size; ++n) {
        if (hierarchy.at(n)->getWriteStrat()
            == Write_Strategy::Write_Through_No_Write_Allocate) {
            if (n + 1 < hierarchy_size) {
                // A write-through level can PASS ON writebacks from above,
                // but must never ORIGINATE one. If you track writebacks issued
                // separately from received, assert issued == received here.
            }
        }
    }

    // ---- Every reference is satisfied exactly once -----------------------
    // Each program reference is served by the first level that has the block,
    // or by memory if none do. Under write-through this does not hold: a write
    // that hits L1 is counted as a hit AND still travels to memory.
    bool any_write_through = false;
    for (int n = 0; n < hierarchy_size; ++n)
        if (hierarchy.at(n)->getWriteStrat()
            == Write_Strategy::Write_Through_No_Write_Allocate)
            any_write_through = true;

    if (!any_write_through) {
        int total_hits = 0;
        for (int n = 0; n < hierarchy_size; ++n)
            total_hits += hierarchy.at(n)->getReadHits()
                        + hierarchy.at(n)->getWriteHits();
        assert(total_hits + memory_reads == total_refs
               && "hits plus memory reads do not account for every reference");
    }

    // ---- All-write-through: every program write reaches memory -----------
    bool all_write_through = true;
    for (int n = 0; n < hierarchy_size; ++n)
        if (hierarchy.at(n)->getWriteStrat()
            != Write_Strategy::Write_Through_No_Write_Allocate)
            all_write_through = false;

    if (all_write_through) {
        assert(memory_writes == trace_write_count
               && "not every program write reached memory in an all-write-through config");
        assert(memory_reads == 0
               && "no-write-allocate should never fetch on a write miss");
    }

    std::cout << "All invariants passed." << std::endl;
}