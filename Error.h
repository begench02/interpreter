#include <iostream>

using namespace std;

namespace Error {
    void printError(string line, string message);
    void printError(string line, int offset, string message);
    void printWarning(string line, string message);
    void printWarning(string line, int offset, string message);
};
