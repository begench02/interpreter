#ifndef HEADERS
#define HEADERS
#include "Token.h"
#endif

#include "Error.h"
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

enum State { S = 0, A = 1, B = 2, F = 3, O = 4, StateSize = 5 };

class TokenCursor {
    private:
        unordered_map<char, TokenType> terminals = {
          { '+', PLUS },
          { '-', MINUS },
          { '=', ASSIGN },
          { '*', MULTIPLY},
          { '/', DIVIDE },
          { '(', OPEN_ROUND_BRACKET },
          { ')', CLOSE_ROUND_BRACKET },
          { '[', OPEN_SQUARE_BRACKET },
          { ']', CLOSE_SQUARE_BRACKET },
          { '{', OPEN_CURLY_BRACKET },
          { '}', CLOSE_CURLY_BRACKET },
          { '~', EQUALITY },
          { '>', GREATER },
          { '<', LESS },
          { '!', INEQUALITY },
          { ';', SEMICOLON },
        };
        
        State transitionTable[StateSize][20] = {
          { A, B, F, F, F, F, F, S, F, F, F, F, F, F, F, F, S, S, O, F },
          { A, A, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, O, F },
          { O, B, F, F, F, F, F, F, O, O, O, O, F, F, F, F, F, F, O, F }
        };
        
        unordered_map<string, TokenType> keywords = {
          { "int", INT },
          { "intarr", INTARR },
          { "if", IF },
          { "else", ELSE },
          { "cin", CIN },
          { "cout", COUT }
        };

        bool isAlpha(char c);
        bool isDigit(char c);
        Token* scanCharacter(string line, int start, int* current, State state, Token *token);
        bool isIdentifier(string word);
    public:
        Token* scanToken(string line, int* offset);
};

