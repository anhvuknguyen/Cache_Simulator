//Test #4
#include <iostream>
#include <cstdlib>

#include "Cache_line.h"
#include "Cache_set.h"
#include "Cache.h"
#include "Cache_utils.h"

using namespace std;
using namespace Cache_utils;

int main() {
    cout << "Testing isPowerOfTwo():" << endl;
    cout << "2 is power of 2: "+to_string(isPowerOfTwo(2)) << endl;
    cout << "4 is power of 2: "+to_string(isPowerOfTwo(4)) << endl;
    cout << "8 is power of 2: "+to_string(isPowerOfTwo(8)) << endl;
    cout << "9 is power of 2: "+to_string(isPowerOfTwo(9)) << endl;
    cout << "10 is power of 2: "+to_string(isPowerOfTwo(10)) << endl;
    cout << "16 is power of 2: "+to_string(isPowerOfTwo(16)) << endl;
    cout << "1024 is power of 2: "+to_string(isPowerOfTwo(1024)) << endl;
    cout << "\nTesting log2():" << endl;
    cout << "log2(2) = " + to_string(log2(2)) << endl;
    cout << "log2(4) = " + to_string(log2(4)) << endl;
    cout << "log2(8) = " + to_string(log2(8)) << endl;
    cout << "log2(9) = " + to_string(log2(9)) << endl;
    cout << "log2(10) = " + to_string(log2(10)) << endl;
    cout << "log2(16) = " + to_string(log2(16)) << endl;
    cout << "log2(1024) = " + to_string(log2(1024)) << endl;
}