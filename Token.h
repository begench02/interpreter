#include <iostream>
using namespace std;


enum TokenType {
  // TODO: Token should not return IDENTIFIER
  // Will it be better to rename INTEGER to NUMBER?
  INT, INTARR, IF, ELSE, WHILE, CIN, COUT, IDENTIFIER, INTEGER, PLUS, MINUS, ASSIGN,
  MULTIPLY, DIVIDE, OPEN_ROUND_BRACKET, CLOSE_ROUND_BRACKET, 
  OPEN_SQUARE_BRACKET, CLOSE_SQUARE_BRACKET, EQUALITY, GREATER, LESS, INEQUALITY,
  OPEN_CURLY_BRACKET, CLOSE_CURLY_BRACKET, SEMICOLON, TERMINAL,

  // HELPER TOKENS
  START, EMPTY, JUMP_FALSE, TAG_PLACE, JUMP,

  // PROGRAMS
  PROGRAM1, PROGRAM2, PROGRAM3, PROGRAM4, PROGRAM5, PROGRAM11,

  TOKEN_TYPE_SIZE
};

class Token {
  public:
    Token(TokenType token);
    Token(TokenType token, string literal);

    TokenType getType();
    string getLiteral();
    string toString();


  private:
    TokenType type;
    string literal;
};
