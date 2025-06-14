#include "RPN.h"
#include <functional>
#include "cassert"

RPN::RPN(ifstream *file) {
  this->program = file;
}

// +========================================================+
vector<Token*> RPN::generateRPNGeneration(int state, TokenType tokenType, Token* token) {
  vector<Token*> result;
  vector<TokenType> rpn_generation = rpnGenerationTable[state][tokenType];

  for (TokenType token_type : rpn_generation) {
    result.push_back(new Token(token_type));
  }

  return result;
}

vector<Token*> RPN::generateRPNGeneration(int state, TokenType tokenType) {
  vector<Token*> result;
  vector<TokenType> rpn_generation = rpnGenerationTable[state][tokenType];
  
  for (TokenType token_type : rpn_generation) {
    if (token_type == INTEGER || token_type == IDENTIFIER) {
      cout << "Should be error!" << endl;
    } else {
      result.push_back(new Token(token_type));
    }
  }

  return result;
}
// +========================================================+

void RPN::generate() { 
  TokenCursor tokenCursor;
  int line_number = 0; 

  string line;
  ll.push(P);
  rpn_generation.push(new Token(EMPTY));

  function<void(Token*)> handleProgram = [&](Token* token) {
    switch (token->getType()) {
      case PROGRAM1: {
        tags.push(rpn.size());
        rpn.push_back(new Token(TAG_PLACE));
        rpn.push_back(new Token(JUMP_FALSE));

        break;
      } case PROGRAM2: {
        int tag_position = tags.top();
        tags.pop();

        rpn[tag_position] = new Token(TAG_PLACE, to_string(rpn.size() + 2));
        tags.push(rpn.size());
        rpn.push_back(new Token(TAG_PLACE));
        rpn.push_back(new Token(JUMP));

        break;
      } case PROGRAM3: {
        int tag_position = tags.top();
        tags.pop();

        rpn[tag_position] = new Token(TAG_PLACE, to_string(rpn.size()));

        break;
      } case PROGRAM4: {
        tags.push(rpn.size());

        break;
      } case PROGRAM5: {
        int tag_position = tags.top();
        tags.pop();

        rpn[tag_position] = new Token(TAG_PLACE, to_string(rpn.size() + 2));
        rpn.push_back(new Token(TAG_PLACE, to_string(tags.top())));
        rpn.push_back(new Token(JUMP));

        break;
      } case PROGRAM11: {
        variables[token->getLiteral()] = 0;

        break;
      } case PROGRAM12: {
        arrays[token->getLiteral()] = 0;

        break;
      } case PROGRAM14: {
        rpn.push_back(new Token(FILL_ARRAY));

        break;
      } case PROGRAM15: {
        rpn.push_back(new Token(FREE));
        
        break;
      } default: { 
        if (token->getType() == IDENTIFIER || token->getType() == INTEGER) {
          rpn.push_back(data.top());
          data.pop();
        } else {
          rpn.push_back(token);
        }
      }
    }
  };

  auto isTokenTerminal = [](int token_number){
    return token_number < P;
  };
  
  auto doesGrammarMatchToken = [](int current_grammar, Token* token) {
    return current_grammar == token->getType();
  };

  auto isProgram = [](Token* token) {
    vector<TokenType> programs = { PROGRAM1, PROGRAM2, PROGRAM3, PROGRAM11 };

    for (TokenType program : programs) {
      if (program == token->getType()) return true;
    }

    return false;
  };

  while (getline(*program, line)) {
    int offset = 0;

    while (offset < line.length()) {
      Token *token = tokenCursor.scanToken(line, &offset);

      if (token->getType() == IDENTIFIER || token->getType() == INTEGER) {
        data.push(token);
      }

      while (true) {
        int current_grammar = ll.top();

        if (isTokenTerminal(current_grammar) && doesGrammarMatchToken(current_grammar, token)) {
          ll.pop();

          if (rpn_generation.top()->getType() != EMPTY) {
            handleProgram(rpn_generation.top());
          }

          rpn_generation.pop();
          break;
        }

        // -100 because all non terminals starts from 100 to avoid collitions with terminals
        int row = ll.top() - 100;
        TokenType column = token->getType();
        vector<int> rules_temp = rpnGrammarTable[row][column];
        vector<Token*> rpn_generation_temp = generateRPNGeneration(row, column, token);

        if (rules_temp.empty()) {
          cout << "Token type: " << token->getType() << endl;
          cout << "Token literal: " << token->getLiteral() << endl;
          // Empty mean error, while 1 mean just pop
          return Error::printError(line, offset, "Syntactic Analysis: grammar error");
        }
        
        if (rules_temp.size() == 1 && rules_temp[0] == 1) {
          rules_temp.pop_back();
        }

        if (ll.size()) ll.pop();

        if (rpn_generation.top()->getType() != EMPTY) {
          handleProgram(rpn_generation.top());
        }

        if (rpn_generation.size()) {
          rpn_generation.pop();
        }

        for (int i = rules_temp.size() - 1; i >= 0; --i) {
          ll.push(rules_temp[i]);
          rpn_generation.push(rpn_generation_temp[i]);
        }
      } 
    }
  }
}


bool RPN::isNumber(Token* token) {
  return token->getType() == INTEGER;
}

bool RPN::isOperation(Token* token) {
  TokenType token_type = token->getType();
  vector<TokenType> operations = { PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN, TAG_PLACE, JUMP_FALSE };

  for (TokenType operation : operations) {
    if (token_type == operation) return true;
  }
  return false;
}


// HELPER FUNCTION [Move somewhere else??] =========
int RPN::tokenToValue(Token *token) {
  int result = 0;
  if (token->getType() == IDENTIFIER) {
    string variable_name = token->getLiteral();

    if (variables.count(variable_name) == 0) {
      Error::printWarning("No line", "Variable %s not initialized");
    } else {
      result = variables[variable_name];
    }

    return result;
  }

  return stoi(token->getLiteral());
}
// ==================================================

int RPN::execute() {
  // HELPER FUNCTION
  auto pop_back_and_get = [](stack<Token*> &stack) -> Token* {
    if (stack.empty()) {
      Error::printError("Execution time", "Execution stack empty");
      return new Token(INTEGER, "-1");
    }

    Token* token = stack.top();
    stack.pop();
    return token;
  };

  stack<Token*> result;
  stack<Token*> tags;
  stack<int*> indexed;
  int i = 0;
  while (i < rpn.size()) {
    Token* token = rpn[i];

    switch (token->getType()) {
      case PLUS: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand + right_operand)));
        break;
      }

      case MINUS: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand - right_operand)));
        break;
      }

      case ASSIGN: {
        int variable_value = tokenToValue(pop_back_and_get(result));

        if (!indexed.empty()) {
          (*indexed.top()) = variable_value;
          indexed.pop();
          break;
        }

        string variable_name= pop_back_and_get(result)->getLiteral();
        variables[variable_name] = variable_value;

        break;
      }

      case MULTIPLY: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand * right_operand)));

        break;
      }

      case DIVIDE: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand / right_operand)));

        break;
      }

      case GREATER: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand > right_operand ? 1 : 0)));

        break;
      }

      case LESS: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand < right_operand ? 1 : 0)));

        break;
      }

      case EQUALITY: {
        int right_operand = tokenToValue(pop_back_and_get(result));
        int left_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand == right_operand ? 1 : 0)));

        break;
      }

      case INEQUALITY: {
        int left_operand = tokenToValue(pop_back_and_get(result));
        int right_operand = tokenToValue(pop_back_and_get(result));

        result.push(new Token(INTEGER, to_string(left_operand != right_operand ? 1 : 0)));

        break;
      }

      case JUMP_FALSE: {
        int condition = stoi(pop_back_and_get(result)->getLiteral());
        if (condition == 0) {
          i = stoi(pop_back_and_get(tags)->getLiteral()) - 1;
        }

        break;
      }

      case JUMP: {
        i = stoi(pop_back_and_get(tags)->getLiteral()) - 1;

        break;
      }

      case COUT: {
        int value;
        if (!indexed.empty()) {
          value = *indexed.top();
        } else {
          value = tokenToValue(pop_back_and_get(result));
        }
        
        cout << value << endl;

        break;
      }

      case FILL_ARRAY: {
        int array_size = tokenToValue(pop_back_and_get(result));
        string array_name = pop_back_and_get(result)->getLiteral();
        
        arrays[array_name] = (int*)malloc(array_size * sizeof(int));
        break;
      }

      case FREE: {
        for (auto& [array_name, array_passport] : arrays) {
          free(array_passport);
        }

        break;
      }

      case INDEXING: {
        int index = tokenToValue(pop_back_and_get(result));
        string array_name = pop_back_and_get(result)->getLiteral();

        indexed.push(&arrays[array_name][index]);

        break;
      } default: {
        if (rpn[i]->getType() == TAG_PLACE) {
          tags.push(rpn[i]);
        } else {
          result.push(rpn[i]);
        }

        break;
      }

    }

    i++;
  }

  if (result.size()) return stoi(result.top()->getLiteral());
  return 0;
}

