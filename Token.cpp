#include "Token.h"
#include <set>

Token::Token(TokenType type, string literal) {
  this->type = type;
  this->literal = literal;
};

Token::Token(TokenType type) {
  this->type = type;
}

TokenType Token::getType() {
  return this->type;
}

string Token::getLiteral() {
  return this->literal;
}

bool Token::isOperation() {
  set<TokenType> literals = { IDENTIFIER, INTEGER, OPEN_ROUND_BRACKET, CLOSE_ROUND_BRACKET };
  return literals.count(this->type) == 0; 
}

string Token::toString() {
  if (this->literal.size()) {
    return this->literal;
  }
  return to_string(this->type);
}

