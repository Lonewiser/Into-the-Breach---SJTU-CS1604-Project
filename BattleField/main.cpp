#include <iostream>
#include <fstream>
#include "field.h"
#include "engine.h"
using namespace std;

int main() {
    Field f(8, 8);

    string filename = "../demo/in.txt";
    // string filename = "../maps/map1.txt";
    ifstream ifs;
    ifs.open(filename);
    if (!ifs) {
        cout << "Cannot open the file: " << filename << endl;
        assert(false);
    }

    filename = "../demo/out1.txt";
    ofstream ofs;
    ofs.open(filename);

    loadMap(ifs, f);

    play(f, ifs, ofs);
    // play(f, cin, cout);

    ifs.close();
    ofs.close();
    return 0;
}
