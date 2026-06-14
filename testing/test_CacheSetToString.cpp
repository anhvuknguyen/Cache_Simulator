//Test #2
#include <iostream>
#include <cstdlib>

#include "Cache_line.h"
#include "Cache_set.h"

using namespace std;

int main() {
    Cache_set* a = new Cache_set(3);
    cout << "---Set a---" << endl;
    a->getLine(0).fill(372,1,0);
    a->getLine(1).fill(372572762,1,0);
    a->getLine(2).fill(0,0,0);
    cout << a->toString() << endl;
    delete a;
}