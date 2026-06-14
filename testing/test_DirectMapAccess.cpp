#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

#include "Cache_library.h"

using namespace std;
using namespace Cache_types;

int main(){
    ifstream file("traces/trace1.txt");

    if (!file.is_open()) {
        cerr << "Failed to open file" << std::endl;
        return 1;
    }

    Cache* c = new Cache(1,8,8,Mapping_Technique::Direct,Replacement_Policy::Direct);

    string operation;
    unsigned int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        cout << operation + " " + to_string(address) << endl;
        Operation op = (operation=="R")? Operation::Read : Operation::Write;
        c->access(op,address);
    }
    cout << c->getStats() << endl;
    cout << c->viewCache() << endl;
    delete c;

    file.close();
    return 0;
}