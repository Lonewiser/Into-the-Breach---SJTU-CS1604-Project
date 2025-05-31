#include <iostream>
#include <fstream>
#include "field.h"
#include "engine.h"
using namespace std;

int main() {
    Field f(8, 8);

    string filename = "../maps/map1.txt";
    ifstream ifs;
    ifs.open(filename);
    if (!ifs) {
        cout << "Cannot open the file: " << filename << endl;
        assert(false);
    }

    loadMap(ifs, f);

    play(f, cin, cout);

    ifs.close();
    return 0;
}
