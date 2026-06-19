#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <fstream>

#include "Cache_line.h"
#include "Cache_set.h"
#include "Cache.h"
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
Replacement_Policy REPPOLICY[] = {Replacement_Policy::Direct,Replacement_Policy::LRU};
string REPPOLICY_strings[] = {"Direct", "LRU"};
int REPPOLICY_size = 2;

//Cache Stats
int setSize = 0;
int numSets = 0;
int numBlocks = 0;
Mapping_Technique mapTech;
Replacement_Policy repPolicy;
Cache *cache = NULL;

//Trace Directory
string trace_directory = "./traces";

Cache* buildCache(){
    int mapTech_index = -1;
    int repPolicy_index = -1;

    if(cache!=NULL){
        cout << "\nMust clear existing cachse first!\n" << endl;
        return cache;
    }

    do{
        cout << "Choose a mapping technique: " << endl;
        for(int i=0;i<MAPTECH_size;i++){
            cout << "[" + to_string(i) + "] " + MAPTECH_strings[i] + "\n";
        }
        cout << ">> ";
        cin >> mapTech_index;
    }while(!(mapTech_index>-1 && mapTech_index<MAPTECH_size));
    mapTech = MAPTECH[mapTech_index];

    do{
        cout << "Choose the number of blocks per line: ";
        cin >> numBlocks;
    }while(numBlocks==0 || !isPowerOfTwo(numBlocks));

    if(mapTech==Mapping_Technique::Direct){
        setSize=1;
        do{
            cout << "Choose the number of lines: ";
            cin >> numSets;
        }while(numSets==0 || !isPowerOfTwo(numSets));
        repPolicy = REPPOLICY[0];
        return new Cache(setSize, numSets, numBlocks, mapTech, repPolicy);
    }
    else if(mapTech==Mapping_Technique::Fully_Associative){
        numSets=1;
        do{
            cout << "Choose the number of lines: ";
            cin >> setSize;
        }while(setSize==0 || isPowerOfTwo(setSize));
    }
    else if(mapTech==Mapping_Technique::Set_Associative){
        do{
            cout << "Choose the size of your set: ";
            cin >> setSize;
        }while(setSize==0 || !isPowerOfTwo(setSize));
        do{
            cout << "Choose the number of sets: ";
            cin >> numSets;
        }while(numSets==0 || !isPowerOfTwo(numSets));
    }

    do{
        cout << "Choose a replacement policy: " << endl;
        for(int i=0;i<REPPOLICY_size;i++){
            cout << "[" + to_string(i) + "] " + REPPOLICY_strings[i] + "\n";
        }
        cout << ">> ";
        cin >> repPolicy_index;
    }while(!(repPolicy_index>-1 && repPolicy_index<REPPOLICY_size));
    repPolicy = REPPOLICY[repPolicy_index];

    return new Cache(setSize, numSets, numBlocks, mapTech, repPolicy);
}

void runTrace(){
    string traceFile;
    vector<string> *traceList = new vector<string>();
    int traceSize = 0;
    int traceIndex=-1;
    cout << "Choose file from trace directory: " << endl;
    try{
        if(fs::exists(trace_directory) && fs::is_directory(trace_directory)){
            do{
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
            }while(!(traceIndex>-1 && traceIndex<traceSize));
        }
    }
    catch(const fs::filesystem_error& e){
        cerr << "Error: " << e.what() << endl;
    }
    traceFile = traceList->at(traceIndex);
    
    ifstream file(traceFile);
    if (!file.is_open()) {
        cerr << "Failed to open file" << std::endl;
        return;
    }
    string operation;
    unsigned int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        cout << operation + " " + to_string(address) << endl;
        Operation op = (operation=="R")? Operation::Read : Operation::Write;
        cache->access(op,address);
    }
    cout << cache->getStats() << endl;
    cout << cache->viewCache() << endl;
}

void clearCache(){
    delete cache;
    cache=NULL;
}

int main() {
    bool active = true;
    
    while(active){
        cout << "What would you like to do?" << endl;
        cout << "[b] Build Cache" << endl;
        cout << "[t] Trace File" << endl;
        cout << "[c] Clear Cache" << endl;
        cout << "[q] Quit" << endl;

        string choice;
        cout << ">> ";
        cin >> choice;

        if(choice=="b"){
            cache = buildCache();
        }
        else if(choice=="t"){
            if(cache==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                runTrace();
            }
        }
        else if(choice=="c"){
            if(cache==NULL){
                cout << "\nCache has not been built!\n" << endl;
                continue;
            }
            else{
                clearCache();
            }
        }
        else if(choice=="q"){
            active=false;
            cout << "Quitting" << endl;
        }
        else{
            cout << "\nInvalid Option\n" << endl;
        }
        
    }
}

