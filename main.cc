#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cubitos.h"

using namespace std;

vector<vector<float>> readFile(const string& filename) {
    vector<vector<float>> points;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<float> point;
        float value;

        while (ss >> value) {
            point.push_back(value);
        }

        points.push_back(point);
    }

    return points;
}

int main(int argc, char* argv[]) {
    enum FLAG { UNSET = 0, SET };
    FLAG tikz = UNSET;
    string param;
    int param_i;

    // Dirty arg handling
    for (param_i = 1; param_i < argc;) {
        param = argv[param_i];
        if (param == "-t") {
            tikz = SET;
            param_i++;
            break;
        } else {
            break;
        }
    }
    if (argc - param_i != 2) {
        cerr << "Use:" << endl
             << argv[0] << " [flags] <filename> <max_depth>" << endl
             << "\t-t tikz output" << endl;
        return 1;
    }

    vector<vector<float>> v = readFile(argv[param_i++]);
    int depth = stoi(argv[param_i]);

    auto p = cubitos::Cubitos<11>(v);

    p.addToLevel(depth);
    if (tikz) {
        cout << p.barcode().tikzbarcode() << std::endl;
    } else {
        cout << p.barcode() << std::endl;
    }

    return 0;
}
