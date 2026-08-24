#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <fstream>

#include "Cache_line.h"
#include "Cache_set.h"
#include "Cache.h"
#include "Cache_hierarchy.h"
#include "Cache_utils.h"

using namespace std;
using namespace Cache_types;
using namespace Cache_utils;

namespace fs = std::filesystem;

//Mapping Technique List
Mapping_Technique MAPTECH[] = {Mapping_Technique::Direct,Mapping_Technique::Fully_Associative,Mapping_Technique::Set_Associative};
string MAPTECH_strings[] = {"Direct","Fully Associative","Set Associative"};
int MAPTECH_size = 3;

//Replacement Policy List
Replacement_Policy REPPOLICY[] = {Replacement_Policy::Direct,Replacement_Policy::Random,Replacement_Policy::FIFO,Replacement_Policy::LIFO,Replacement_Policy::LRU,Replacement_Policy::MRU,Replacement_Policy::LFU, Replacement_Policy::Belady};
string REPPOLICY_strings[] = {"Direct", "Random", "FIFO", "LIFO", "LRU", "MRU", "LFU", "Belady"};
int REPPOLICY_size = 8;

Write_Strategy WRITESTRAT[] = {Write_Strategy::Write_Back_Write_Allocate,Write_Strategy::Write_Through_No_Write_Allocate};
string WRITESTRAT_strings[] = {"Write-Back, Write-Allocate", "Write-Through, No-Write-Allocate"};
int WRITESTRAT_size = 2;

//Cache Stats
int hierarchy_size = -1;
vector<Level_config> level_config_v;
Cache_hierarchy *cache_h = NULL;

//Trace Directory
string trace_directory = "./traces";

Cache_hierarchy* buildCacheHierarchy(){
    if(cache_h!=NULL){
        cout << "\nMust clear existing cache hierarchy first!\n" << endl;
        return cache_h;
    }

    do{
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Choose the number of Cache levels: ";
        cin >> hierarchy_size;
    }while(hierarchy_size<1 || hierarchy_size>4 || cin.fail()); 

    for(int j=0;j<hierarchy_size;j++){
        int mapTech_index = -1;
        int repPolicy_index = -1;
        int writeStrat_index = -1;

        Level_config cfg;

        int blockSize = 0;
        int setSize = 0;
        int numSets = 0;
        Mapping_Technique mapTech;
        Replacement_Policy repPolicy;
        Write_Strategy writeStrat;

        do{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Choose a mapping technique for L["+to_string(j+1)+"]: " << endl;
            for(int i=0;i<MAPTECH_size;i++){
                cout << "[" + to_string(i) + "] " + MAPTECH_strings[i] + "\n";
            }
            cout << ">> ";
            cin >> mapTech_index;
        }while(!(mapTech_index>-1 && mapTech_index<MAPTECH_size && !cin.fail()));
        mapTech = MAPTECH[mapTech_index];
        cfg.mapping_Tech=mapTech;

        do{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Choose block size for L["+to_string(j+1)+"] (bytes): ";
            cin >> blockSize;
        }while(blockSize==0 || !isPowerOfTwo(blockSize) || cin.fail());
        cfg.num_blocks=blockSize;

        if(mapTech==Mapping_Technique::Direct){
            do{
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Choose the number of lines for L["+to_string(j+1)+"]: ";
                cin >> numSets;
            }while(numSets==0 || !isPowerOfTwo(numSets) || cin.fail());
            setSize=1;
            repPolicy = REPPOLICY[0];

            cfg.num_sets=numSets;
            cfg.set_size=setSize;
            cfg.replacement_Pol=repPolicy;
            do{
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Choose a write strategy for L["+to_string(j+1)+"]: " << endl;
                for(int i=0;i<WRITESTRAT_size;i++){
                    cout << "[" + to_string(i) + "] " + WRITESTRAT_strings[i] + "\n";
                }
                cout << ">> ";
                cin >> writeStrat_index;

            }while(!(writeStrat_index>-1 && writeStrat_index<WRITESTRAT_size && !cin.fail()));
            writeStrat = WRITESTRAT[writeStrat_index];
            cfg.write_Strat = writeStrat;
            level_config_v.push_back(cfg);
            continue;
        }
        else if(mapTech==Mapping_Technique::Fully_Associative){
            do{
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Choose the number of lines for L["+to_string(j+1)+"]: ";
                cin >> setSize;
            }while(setSize==0 || !isPowerOfTwo(setSize) || cin.fail());
            numSets=1;

            cfg.set_size=setSize;
            cfg.num_sets=numSets;
        }
        else if(mapTech==Mapping_Technique::Set_Associative){
            do{
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Choose associativity for L["+to_string(j+1)+"] (lines per set): ";
                cin >> setSize;
            }while(setSize==0 || !isPowerOfTwo(setSize) || cin.fail());
            do{
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Choose the number of sets for L["+to_string(j+1)+"]: ";
                cin >> numSets;
            }while(numSets==0 || !isPowerOfTwo(numSets) || cin.fail());

            cfg.set_size=setSize;
            cfg.num_sets=numSets;
        }

        do{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Choose a replacement policy for L["+to_string(j+1)+"]: " << endl;
            for(int i=0;i<REPPOLICY_size;i++){
                cout << "[" + to_string(i) + "] " + REPPOLICY_strings[i] + "\n";
            }
            cout << ">> ";
            cin >> repPolicy_index;

            if(repPolicy_index == 7 && hierarchy_size!=1){
                repPolicy_index=-1;
                cout << "Belady's Replacement Algorithm only available for cache hierarchies of size 1." << endl;
            }

        }while(!(repPolicy_index>-1 && repPolicy_index<REPPOLICY_size && !cin.fail()));
        repPolicy = REPPOLICY[repPolicy_index];
        cfg.replacement_Pol=repPolicy;

        do{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Choose a write strategy for L["+to_string(j+1)+"]: " << endl;
            for(int i=0;i<WRITESTRAT_size;i++){
                cout << "[" + to_string(i) + "] " + WRITESTRAT_strings[i] + "\n";
            }
            cout << ">> ";
            cin >> writeStrat_index;

        }while(!(writeStrat_index>-1 && writeStrat_index<WRITESTRAT_size && !cin.fail()));
        writeStrat = WRITESTRAT[writeStrat_index];
        cfg.write_Strat = writeStrat;

        level_config_v.push_back(cfg);
    }

    return new Cache_hierarchy(hierarchy_size,level_config_v);
}

void chooseTraceFiles(string &traceType_dir, string &traceFile){
    vector<string> *traceList = new vector<string>();
    int traceSize;
    int traceIndex=-1;

    //Choose Trace Type
    cout << "Choose type of trace: " << endl;
    try{
        if(fs::exists(trace_directory) && fs::is_directory(trace_directory)){
            do{
                traceSize = 0;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                for (const auto& entry : fs::directory_iterator(trace_directory)) {
                    traceList->push_back(entry.path());
                    string str = entry.path();
                    size_t substring_index = str.rfind("/");
                    str = str.substr(substring_index+1);
                    cout << "[" << to_string(traceSize) << "] " << str << endl;
                    traceSize++;
                }
                cout << ">> ";
                cin >> traceIndex;
            }while(!(traceIndex>-1 && traceIndex<traceSize && !cin.fail()));
        }
    }
    catch(const fs::filesystem_error& e){
        cerr << "Error: " << e.what() << endl;
    }
    traceType_dir = traceList->at(traceIndex);

    //Choose Trace Directory
    delete traceList;
    traceIndex=-1;
    traceList = new vector<string>();
    cout << "Choose file from trace directory: " << endl;
    try{
        if(fs::exists(traceType_dir) && fs::is_directory(traceType_dir)){
            do{
                traceSize = 0;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                for (const auto& entry : fs::directory_iterator(traceType_dir)) {
                    traceList->push_back(entry.path());
                    string str = entry.path();
                    size_t substring_index = str.rfind("/");
                    str = str.substr(substring_index+1);
                    cout << "[" << to_string(traceSize) << "] " << str << endl;
                    traceSize++;
                }
                cout << ">> ";
                cin >> traceIndex;
            }while(!(traceIndex>-1 && traceIndex<traceSize && !cin.fail()));
        }
    }
    catch(const fs::filesystem_error& e){
        cerr << "Error: " << e.what() << endl;
    }
    traceFile = traceList->at(traceIndex);
}

void runTrace(){
    string traceType_dir;
    string traceFile;
    
    chooseTraceFiles(traceType_dir, traceFile);
    
    ifstream file(traceFile);
    if (!file.is_open()) {
        cerr << "Failed to open file" << std::endl;
        return;
    }

    if(hierarchy_size==1 && level_config_v.at(0).replacement_Pol==Replacement_Policy::Belady){
        cache_h->belady_loadFile(traceFile);
    }

    string operation;
    unsigned int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        cout << operation + " " + to_string(address) << endl;
        Operation op = (operation=="R")? Operation::Read : Operation::Write;
        cache_h->access(op,address);
    }
    cout << cache_h->getStats() << endl;
}

void deleteCacheHierarchy(){
    delete cache_h;
    hierarchy_size=-1;
    level_config_v.clear();
    cache_h=NULL;
}

void viewCacheHierarchy(){
    cout << cache_h->viewCache() << endl;
}

void viewCacheHierarchyStats(){
    cout << cache_h->getStats() << endl;
}

void viewCacheHierarchyDetails(){
    cout << cache_h->getDetails() << endl;
}

void resetCacheHierarchy(){
    cache_h->reset();
}

int main() {
    bool active = true;
    
    while(active){
        cout << "What would you like to do?" << endl;
        cout << "[b] Build Cache Hierarchy" << endl;
        cout << "[v] View Cache Hierarchy" << endl;
        cout << "[s] View Cache Hierarchy Stats" << endl;
        cout << "[p] View Cache Hierarchy Properties" << endl;
        cout << "[t] Trace File" << endl;
        cout << "[r] Reset Cache Hierarchy" << endl;
        cout << "[d] Delete Cache Hierarchy" << endl;
        cout << "[q] Quit" << endl;

        string choice;
        cout << ">> ";
        cin >> choice;

        if(choice=="b"){
            cache_h = buildCacheHierarchy();
        }
        else if(choice=="t"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                runTrace();
            }
        }
        else if(choice=="d"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                deleteCacheHierarchy();
            }
        }
        else if(choice=="q"){
            active=false;
            cout << "Quitting" << endl;
        }
        else if(choice=="v"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                viewCacheHierarchy();
            }
        }
        else if(choice=="s"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                viewCacheHierarchyStats();
            }
        }
        else if(choice=="p"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                viewCacheHierarchyDetails();
            }
        }
        else if(choice=="r"){
            if(cache_h==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                resetCacheHierarchy();
            }
        }
        else{
            cout << "\nInvalid Option\n" << endl;
        }
        
    }
}

