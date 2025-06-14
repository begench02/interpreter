#include "TokenCursor.h"
#include "CharacterType.h"

bool TokenCursor::isKeyword(string &word) const {
    return this->keywords.count(word) > 0;
}

Token* TokenCursor::handleWhitespace(const string &line, int &start, int *current, State &state, Token *token) {
  if (token->getType() == START) {
    ++start;
    *current = start;
  } else {
    state = F;
  }

  return scanCharacter(line, start, current, state, token);
}

Token *TokenCursor::handleTerminal(const string &line, int &start, int *current, State &state, Token *token) {
  if (token->getType() != START) {
    return scanCharacter(line, start, current, F, token);
  }

  char ch = line[*current];
  auto it = terminals.find(ch);

  if (it == terminals.end()) {
    return nullptr;
  }

  TokenType terminal = it->second;
  ++(*current);

  switch (terminal) {
    case PLUS:
    case MINUS:
      token = new Token(terminal, "1");
      
      break;
    case MULTIPLY:
    case DIVIDE: 
      token = new Token(terminal, "2");

      break;
    default:
      token = new Token(terminal);
  }

  state = F;
  return scanCharacter(line, start, current, state, token);
}

Token *TokenCursor::handleNumberOrIdentifier(const string &line, int &start, int *current, State &state, Token *token) {
  char ch = line[*current];
  CharacterType characterType(ch);

  if (characterType.type() == NUMBER) {
    state = transitionTable[state][INTEGER - 7];
    ++(*current);

    token = new Token(INTEGER, line.substr(start, *current - start));
    return scanCharacter(line, start, current, state, token);
  }

  if (characterType.type() == STRING) {
    state = transitionTable[state][IDENTIFIER - 7];
    ++(*current);

    token = new Token(IDENTIFIER, line.substr(start, *current - start));
    return scanCharacter(line, start, current, state, token);
  }

  return scanCharacter(line, start, current, F, token);
}

Token *TokenCursor::finalizeToken(const string &line, int start, int *current, State state, Token *token) {
  if (state != F) return token;

  if (token->getType() == INTEGER) {
    string number = line.substr(start, *current - start);
    return new Token(INTEGER, number);
  }

  if (token->getType() == IDENTIFIER) {
    string identifier = line.substr(start, *current - start);
    if (isKeyword(identifier)) {
      return new Token(keywords.at(identifier), identifier);
    }
    return new Token(IDENTIFIER, identifier);
  }

  return token;
}

Token *TokenCursor::scanCharacter(const string &line, int start, int *current, State state, Token *token) {
  if (state == F || *current >= static_cast<int>(line.length())) {
    return finalizeToken(line, start, current, state, token);
  }

  if (line[*current] == ' ') {
    return handleWhitespace(line, start, current, state, token);
  }

  if (terminals.count(line[*current]) > 0) {
      return handleTerminal(line, start, current, state, token);
  }

  if (auto res = handleNumberOrIdentifier(line, start, current, state, token)) {
    return res;
  }

  Error::printWarning(line, *current, "Unexpected character: ");
  ++(*current);
  return scanCharacter(line, state, current, state, token);
}

Token* TokenCursor::scanToken(const string &line, int *offset) {
  State state = S;
  int start = *offset;

  Token *start_token = new Token(START);
  return this->scanCharacter(line, start, offset, state, start_token);
}
