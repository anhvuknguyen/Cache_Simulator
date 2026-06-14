//Test #1
#include <iostream>
#include <cstdlib>

#include "Cache_line.h"
#include "Cache_set.h"

using namespace std;

int main() {
    Cache_set* a = new Cache_set(3);
    cout << "---Set a---" << endl;
    a->getLine(0).fill(372,1,0);
    cout << a->getLine(0).getTag() << endl;
    cout << a->getLine(0).getValidBit() << endl;
    cout << a->getLine(0).getDirtyBit() << endl;

    a->getLine(1).fill(372572762,1,0);
    cout << a->getLine(1).getTag() << endl;
    cout << a->getLine(1).getValidBit() << endl;
    cout << a->getLine(1).getDirtyBit() << endl;

    a->getLine(2).fill(0,0,0);
    cout << a->getLine(2).getTag() << endl;
    cout << a->getLine(2).getValidBit() << endl;
    cout << a->getLine(2).getDirtyBit() << endl;

    Cache_set* b = new Cache_set(*a);
    cout << "\n---Set b---" << endl;
    cout << b->getLine(0).getTag() << endl;
    cout << b->getLine(0).getValidBit() << endl;
    cout << b->getLine(0).getDirtyBit() << endl;

    cout << b->getLine(1).getTag() << endl;
    cout << b->getLine(1).getValidBit() << endl;
    cout << b->getLine(1).getDirtyBit() << endl;

    cout << b->getLine(2).getTag() << endl;
    cout << b->getLine(2).getValidBit() << endl;
    cout << b->getLine(2).getDirtyBit() << endl;

    Cache_set* c = new Cache_set(1);
    cout << "\n---Set c---" << endl;
    c->getLine(0).fill(4253,1,0);
    cout << c->getLine(0).getTag() << endl;
    cout << c->getLine(0).getValidBit() << endl;
    cout << c->getLine(0).getDirtyBit() << endl;

    *c = *a;
    cout << "\n---Set c---" << endl;
    cout << c->getLine(0).getTag() << endl;
    cout << c->getLine(0).getValidBit() << endl;
    cout << c->getLine(0).getDirtyBit() << endl;

    cout << c->getLine(1).getTag() << endl;
    cout << c->getLine(1).getValidBit() << endl;
    cout << c->getLine(1).getDirtyBit() << endl;

    cout << c->getLine(2).getTag() << endl;
    cout << c->getLine(2).getValidBit() << endl;
    cout << c->getLine(2).getDirtyBit() << endl;
    delete a;
    delete b;
    delete c;
}
    