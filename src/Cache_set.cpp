#include "Cache_set.h"
#include "Types.h"
#include <cstdlib>
#include <string> 
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;
using namespace Cache_types;

Cache_set::Cache_set(int setSize, Replacement_Policy repPolicy){
    set_Size = setSize;
    capacity = 0;
    replacement_Policy = repPolicy;
}

Cache_set::~Cache_set() = default;