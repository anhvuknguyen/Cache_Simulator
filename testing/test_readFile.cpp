#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int main() {
    ifstream file("traces/trace1.txt");

    if (!file.is_open()) {
        cerr << "Failed to open file" << std::endl;
        return 1;
    }

    string operation;
    int address;
    while (file.good()) {
        file >> operation >> std::hex >> address;
        cout << operation + " " + to_string(address) << endl;
    }

    file.close();
    return 0;
}

// ***Reading Different Data Types***
// cppstd::ifstream file("data.txt");
// int age;
// double salary;
// std::string name;

// // Just like scanner.nextInt(), scanner.nextDouble(), scanner.next()
// file >> age >> salary >> name;

// ***Conversion Table***
// new Scanner(new File("f.txt"))      std::ifstream file("f.txt")
// scanner.nextLine()                  std::getline(file, str)
// scanner.nextInt()                   file >> myInt
// scanner.nextDouble()                file >> myDouble
// scanner.hasNextLine()               while (std::getline(...)) or file.good()
// scanner.close()                     file.close()

// ***Reading Hex***
// std::ifstream file("data.txt"); // file contains: 0x1A 0xFF 0x10

// int a, b, c;
// file >> std::hex >> a >> b >> c;

// std::cout << a << std::endl; // 26
// std::cout << b << std::endl; // 255
// std::cout << c << std::endl; // 16

// Important: std::hex is Sticky
// Unlike Java where you pass the base per-call, 
// std::hex stays active for the rest of the stream until you change it back:
// cppfile >> std::hex >> a;  // reads as hex
// file >> b;              // STILL reads as hex!
// file >> std::dec >> c;  // back to decimal