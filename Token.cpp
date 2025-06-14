#include "Token.h"

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

string Token::toString() {
  if (this->literal.size()) {
    return this->literal;
  }
  return to_string(this->type);
}

