#ifndef TOKENCURSOR
#define TOKENCURSOR
#include "TokenCursor.h"
#endif

#ifndef HEADERS
#define HEADERS
#include "Token.h"
#endif

#include <vector>
#include "Error.h"
#include <stack>
#include <fstream>
#include <unordered_map>

using namespace std;

class RPN {
  stack<int> ll;
  stack<Token*> rpn_generation;
  vector<Token*> rpn;
  stack<int> tags;
  ifstream *program;
  stack<Token*> data;

  enum RPN_STATE { P = 100, A, C, D, E, S, U, T, V, F, H, Z, RPN_STATE_SIZE = 12};

  vector<TokenType> rpnGenerationTable[RPN_STATE_SIZE][26] = {
    {
      { PROGRAM11, IDENTIFIER, EMPTY, EMPTY, ASSIGN, EMPTY }, 
      { PROGRAM12, IDENTIFIER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { PROGRAM14, EMPTY, PROGRAM15 },
      {}, {}, {}
    },
    {
      {}, {}, 
      { EMPTY, EMPTY, EMPTY, PROGRAM1, EMPTY, EMPTY, EMPTY, EMPTY, PROGRAM3 }, 
      {}, 
      { PROGRAM4, EMPTY, EMPTY, PROGRAM1, EMPTY, EMPTY, PROGRAM5, EMPTY },
      { EMPTY, EMPTY, IDENTIFIER, EMPTY, CIN, EMPTY, EMPTY }, 
      { EMPTY, EMPTY, EMPTY, COUT, EMPTY, EMPTY }, 
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
      {}, {}, {}, 
      { PROGRAM2, EMPTY, EMPTY, EMPTY }, 
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, 
      { IDENTIFIER, EMPTY, EMPTY, EMPTY },
      { INTEGER, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, PLUS },
      { EMPTY, EMPTY, MINUS },
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
      { IDENTIFIER, EMPTY },
      { INTEGER },
      {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, EMPTY },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { EMPTY, EMPTY, INDEXING },
      {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    },
    {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
    }
  };

  vector<int> rpnGrammarTable[RPN_STATE_SIZE][26] = {
    { 
      { INT, IDENTIFIER, ASSIGN, S, SEMICOLON, P },
      { INTARR, IDENTIFIER, OPEN_SQUARE_BRACKET, S, CLOSE_SQUARE_BRACKET, SEMICOLON, P }, 
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
      { OPEN_CURLY_BRACKET, A, CLOSE_CURLY_BRACKET }, {}, {}, {}
    },
    { 
      { 1 }, { 1 }, 
      { IF, OPEN_ROUND_BRACKET, C, CLOSE_ROUND_BRACKET, OPEN_CURLY_BRACKET, A, CLOSE_CURLY_BRACKET, E, A }, 
      { 1 }, 
      { WHILE, OPEN_ROUND_BRACKET, C, CLOSE_ROUND_BRACKET, OPEN_CURLY_BRACKET, A, CLOSE_CURLY_BRACKET, A },
      { CIN, OPEN_ROUND_BRACKET, IDENTIFIER, H, CLOSE_ROUND_BRACKET, SEMICOLON, A },
      { COUT, OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET, SEMICOLON, A },
      { IDENTIFIER, H, ASSIGN, S, SEMICOLON, A },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, 
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 } 
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
      { OPEN_ROUND_BRACKET, S, CLOSE_ROUND_BRACKET, V, U },
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
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
      { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }
    }
  };

public:
  vector<Token*> generateRPNGeneration(int state, TokenType tokenType, Token* value);
  vector<Token*> generateRPNGeneration(int state, TokenType tokenType);
  unordered_map<string, int> variables;
  unordered_map<string, int*> arrays;
  RPN(ifstream *file);
  void generate();
  int execute();
  bool isNumber(Token* token);
  bool isOperation(Token* token);


  // HELPER METHODS
  int tokenToValue(Token* token);
  void replaceIdentifier(Token* token);
};

