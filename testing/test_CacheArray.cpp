//Test #3
#include <iostream>
#include <cstdlib>

#include "Cache_line.h"
#include "Cache_set.h"
#include "Cache.h"
#include "Types.h"

using namespace std;
using namespace Cache_types;

int main() {
    Cache* c = new Cache(1,8,8,Mapping_Technique::Direct,Replacement_Policy::Direct);
    cout << c->toString() << endl;
    delete c;
    c = new Cache(1,16,8,Mapping_Technique::Direct,Replacement_Policy::Direct);
    cout << c->toString() << endl;
    delete c;
}