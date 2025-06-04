#include "TokenCursor.h"
#include "CharacterType.h"

bool TokenCursor::isIdentifier(string word) {
    return this->keywords.count(word) > 0;
}

Token* TokenCursor::scanCharacter(string line, int start, int *current, State state, Token *token) {
  if (state == O) {
    Error::printWarning(line, *current, "Unexpected character: ");
  } else if (state == F) {
    if (token->getType() == INTEGER) {
      string number = line.substr(start, (*current) - start);
      return new Token(token->getType(), number);
    } else if (token->getType() == IDENTIFIER) {
      string identifier = line.substr(start, (*current) - start - 1);
      if (keywords.count(identifier)) {
        return new Token(keywords[identifier]);
      } else {
        return token;
      }
    } else {
      return token;
    }
  }

  if (line.length() == *current && token->getType() == IDENTIFIER) {
    string word = line.substr(start, (*current) - start);
    if (this->keywords.count(word)) {
      TokenType token_type = this->keywords.at(word);
      return new Token(token_type);
    }
    return new Token(token->getType(), word);
  }

  char character = line[*current];

  if (character == ' ') {
    if (token->getType() == START) {
      start++;
      (*current)++;
      return this->scanCharacter(line, start, current, S, token);
    } else {
      (*current)++;
      return this->scanCharacter(line, start, current, F, token);
    }
  }

  if (character == ' ' && state == S) {
    (*current) = (*current) + 1;
    start = start + 1;
    return this->scanCharacter(line, start, current, state, token);
  }

  if (this->terminals.count(character) > 0) {
    if (state == S) {
      TokenType terminal = this->terminals.at(character);
      (*current)++;
      if (terminal == PLUS || terminal == MINUS) {
        token = new Token(terminal, to_string(1));
      } else if (terminal == MULTIPLY || terminal == DIVIDE) {
        token = new Token(terminal, to_string(2));
      } else {
        token = new Token(terminal);
      }
      
      return this->scanCharacter(line, start, current, F, token);
    } else {
      return this->scanCharacter(line, start, current, F, token);
    }
  }

  CharacterType *characterType = new CharacterType(character);
  if (characterType->type() == NUMBER) {
    state = this->transitionTable[state][INTEGER];
    (*current)++;
    return this->scanCharacter(line, start, current, state, new Token(INTEGER));
  } else if (characterType->type() == STRING) {
    state = this->transitionTable[state][IDENTIFIER];
    (*current)++;
    return this->scanCharacter(line, start, current, state, new Token(IDENTIFIER));
  }

  return token;
}

Token* TokenCursor::scanToken(string line, int* offset) {
  State state = S;
  int start = *offset;

  Token *start_token = new Token(START);
  return this->scanCharacter(line, start, offset, state, start_token);
}
