#ifndef TOKENCURSOR
#define TOKENCURSOR
#include "TokenCursor.h"
#endif

#ifndef HEADERS
#define HEADERS
#include "Token.h"
#endif


#include <vector>
#include <stack>
#include <fstream>
#include <unordered_map>


using namespace std;

class RPN {
  stack<int> ll;
  stack<int> rpn_generation;
  stack<int> rpn;
  ifstream *program;

  enum RPN_STATE { P, A, C, D, E, S, U, T, V, F, H, Z, RPN_STATE_SIZE};
  enum RPN_PROGRAMS { PROGRAM11 = 200 };

  int EMPTY = -1;
  vector<int> rpnGenerationTable[RPN_STATE_SIZE][TOKEN_TYPE_SIZE] = {
    {
      { PROGRAM11, IDENTIFIER, EMPTY, EMPTY, ASSIGN, EMPTY }, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, EMPTY },
      {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, EMPTY, EMPTY, EMPTY, ASSIGN, EMPTY },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, EMPTY, EMPTY, EMPTY, EMPTY},
      { INTEGER, EMPTY, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, 
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, EQUALITY }, 
      { EMPTY, EMPTY, GREATER },
      { EMPTY, EMPTY, LESS },
      { EMPTY, EMPTY, INEQUALITY }
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, EMPTY, EMPTY, EMPTY },
      { INTEGER, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, 
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      {EMPTY, EMPTY, PLUS},
      {EMPTY, EMPTY, MINUS},
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, EMPTY, EMPTY },
      { INTEGER, EMPTY },
      {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, MULTIPLY },
      { EMPTY, EMPTY, DIVIDE },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      {IDENTIFIER, EMPTY},
      {INTEGER},
      {}, {}, {}, {}, {}, 
      {EMPTY, EMPTY, EMPTY},
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    }
  };

  vector<int> rpnGrammarTable[RPN_STATE_SIZE][TOKEN_TYPE_SIZE] = {
    { 
      { INT, IDENTIFIER, ASSIGN, S, SEMICOLON, P },
      { INTARR, IDENTIFIER, OPEN_SQUARE_BRACKET, S, CLOSE_SQUARE_BRACKET, SEMICOLON, P }, 
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {A}, {}, {}, {}
    },
    { 
      { 1 }, { 1 }, 
      { IF, OPEN_ROUND_BRACKET, C, CLOSE_ROUND_BRACKET, OPEN_SQUARE_BRACKET, A, CLOSE_SQUARE_BRACKET, E, A }, 
      { 1 }, 
      { WHILE, OPEN_ROUND_BRACKET, C, CLOSE_ROUND_BRACKET, OPEN_SQUARE_BRACKET, A, CLOSE_SQUARE_BRACKET, A },
      { CIN, IDENTIFIER, H, SEMICOLON, A },
      { COUT, OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET, SEMICOLON, A },
      { IDENTIFIER, H, ASSIGN, S, SEMICOLON, A },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} 
    },
    {
      {}, {}, {}, {}, {}, {}, {},
      { IDENTIFIER, H, V, U, D },
      { INTEGER, V, U, D },
      {}, {}, {}, {}, {},
      { OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET, V, U, D },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
      { EQUALITY, S, Z },
      { GREATER, S, Z },
      { LESS, S, Z },
      { INEQUALITY, S, Z },
      {}, {}, {}, {}
    },
    {
      { 1 }, { 1 }, { 1 },
      { ELSE, OPEN_CURLY_BRACKET, A, CLOSE_CURLY_BRACKET },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, H, V, U },
      { INTEGER, V, U },
      {}, {}, {}, {}, {}, 
      { OPEN_CURLY_BRACKET, S, CLOSE_CURLY_BRACKET, V, U },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
      { PLUS, T, U }, { MINUS, T, U },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, H, V },
      { INTEGER, V },
      {}, {}, {}, {}, {}, 
      { OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET, V },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
      { MULTIPLY, F, V },
      { DIVIDE, F, V },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, H },
      { INTEGER },
      {}, {}, {}, {}, {}, 
      { OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
      { OPEN_SQUARE_BRACKET, S, CLOSE_SQUARE_BRACKET },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    }
  };

public:
  unordered_map<string, string> variables;
  RPN(ifstream *file);
  void generate();
  int execute();
  bool isNumber(Token* token);
  bool isOperation(Token* token);
};

