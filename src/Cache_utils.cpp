#include "Cache_utils.h"

//Checks if number is a power of 2
//(10000 & 01111 = 0); Property of all powers of 2
bool Cache_utils::isPowerOfTwo(int x){
    return (x & (x-1)) == 0;
}

//Takes the log2 of a number
//Input expects a power of 2
int Cache_utils::log2(int x){
    if(!isPowerOfTwo(x)){
        return -1;
    }
    int exp = 0;
    while(x!=0){
        x>>=1;
        exp++;
    }
    return exp-1;
}