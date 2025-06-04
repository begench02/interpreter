#include <iostream>
using namespace std;


enum TokenType {
  // TODO: Token should not return IDENTIFIER
  // Will it be better to rename INTEGER to NUMBER?
  INT, INTARR, IF, ELSE, WHILE, CIN, COUT, IDENTIFIER, INTEGER, PLUS, MINUS, ASSIGN,
  MULTIPLY, DIVIDE, OPEN_ROUND_BRACKET, CLOSE_ROUND_BRACKET, 
  OPEN_SQUARE_BRACKET, CLOSE_SQUARE_BRACKET, EQUALITY, GREATER, LESS, INEQUALITY,
  OPEN_CURLY_BRACKET, CLOSE_CURLY_BRACKET, SEMICOLON, TERMINAL, TOKEN_TYPE_SIZE,

  // HELPER TOKENS
  START
};

class Token {
  public:
    Token(TokenType token);
    Token(TokenType token, string literal);

    TokenType getType();
    string getLiteral();
    bool isOperation();
    string toString();
    string isToken();


  private:
    TokenType type;
    string literal;
};
