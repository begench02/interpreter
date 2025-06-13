#include <stdbool.h>
#include <iostream>
#include <fstream>

#ifndef TOKENCURSOR
#define TOKENCURSOR
#include "RPN.h"
#include "TokenCursor.h"
#endif

using namespace std;

int main(int argc, char *argv[]) {
  bool has_error = false;
  if (argc < 2) {
    Error::printError(0, "Provide file as an argument");
  }

  ifstream file;
  file.open(argv[1]);
  if (file.fail()) {
    cout << "Error opening file" << endl;
    return 1;
  }

  RPN *rpn = new RPN(&file);
  rpn->generate();
  int result = rpn->execute();
 // cout << "Result: " << result << endl;
  
  file.close();
  if (has_error == true) {
    return -1;
  }
  return 0;
}
