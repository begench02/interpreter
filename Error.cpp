#include "Error.h"

namespace Error {
    void printError(string line, string message) {
        printError(line, 0, message);
    }
    void printError(string line, int offset, string message) {
        std::cout << "[line] " << line << ". "
            << "[error]: " << offset << message << std::endl;
        exit(EXIT_FAILURE);

    }

    void printWarning(string line, string message) {
        printWarning(line, 0, message);
    }
    void printWarning(string line, int offset, string message) {
        std::cout << "[line] " << line << ". "
            << "[warning]: " << offset << message << std::endl;
    }
};
