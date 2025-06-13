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
      } case PROGRAM11: {
        variables[token->getLiteral()] = "0";

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

        if (isTokenTerminal(current_grammar) && doesGrammarMatchToken(current_grammar, token) && !isProgram(token)) {
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

  if (token_type == PLUS || token_type == MINUS || token_type == MULTIPLY || token_type == DIVIDE)
    return true;

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
      result = stoi(variables[variable_name]);
    }

    return result;
  }

  return stoi(token->getLiteral());
}
// ==================================================

int RPN::execute() {
  stack<int> execution_stack;

  // HELPER FUNCTION
  auto pop_back_and_get = [](stack<int>& stack) -> int {
    if (stack.empty()) {
      Error::printError("Execution time", "Execution stack empty");
      return -1;
    }

    int value = stack.top();
    stack.pop();
    return value;
  };

  int i = 0;
  while (i < rpn.size()) {
    Token* token = rpn[i];

    switch (token->getType()) {
      case ASSIGN: {
        assert(i >= 2);
        string variable_name= rpn[i - 2]->getLiteral();
        string variable_value = to_string(tokenToValue(rpn[i - 1]));

        variables[variable_name] = variable_value;

        break;
      }

      case GREATER: {
        assert(i >= 2);
        int left_operand = tokenToValue(rpn[i - 2]);
        int right_operand = tokenToValue(rpn[i - 1]);

        execution_stack.push(left_operand > right_operand ? 1 : 0);

        break;
      }

      case JUMP_FALSE: {
        int condition = pop_back_and_get(execution_stack);
        if (condition == 0) {
          assert(i >= 1);
          i = stoi(rpn[i - 1]->getLiteral()) - 1;
        }

        break;
      }

      case COUT: {
        assert(i >= 1);
        int value = tokenToValue(rpn[i - 1]);
        cout << value << endl;

        break;
      }

    }

    i++;
  }

  return 0;
}

