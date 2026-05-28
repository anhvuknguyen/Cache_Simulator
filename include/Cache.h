#ifndef CACHE
#define CACHE

#include <string> 

using namespace std;

class Cache{
private:
    int cache_size;
    int num_sets;
public:
    Cache(int setSize, int numSets, int blockSize);
};

#endif