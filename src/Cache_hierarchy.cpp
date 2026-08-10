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

Cache_hierarchy::Cache_hierarchy(int num_caches, std::vector<int> num_lines_v_in, std::vector<int> num_sets_v_in, std::vector<int> num_blocks_v_in, std::vector<Mapping_Technique> mapping_Techniques_in, std::vector<Replacement_Policy> replacement_Policies_in){
    size = num_caches;

    for(int i=0;i<size;i++){
        int nLines_perSet = num_lines_v_in.at(i);
        int nSets = num_sets_v_in.at(i);
        int nBlocks = num_blocks_v_in.at(i);
        Mapping_Technique mTech = mapping_Techniques_in.at(i);
        Replacement_Policy rPol = replacement_Policies_in.at(i);

        //Populate respective vectors
        num_lines_per_set_v.emplace_back(nLines_perSet);
        num_sets_v.emplace_back(nSets);
        num_blocks_v.emplace_back(nBlocks);
        mapping_Techniques.emplace_back(mTech);
        replacement_Policies.emplace_back(rPol);

        //Construct caches
        hierarchy.push_back(make_unique<Cache>(nLines_perSet,nSets,nBlocks,mTech,rPol));
    }
}

void Cache_hierarchy::reset(){
    for(int i=0;i<size;i++){
        hierarchy.at(i).reset();
    }
}