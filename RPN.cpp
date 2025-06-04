#include "RPN.h"

RPN::RPN(ifstream *file) {
  this->program = file;
}

void RPN::generate() {
  TokenCursor tokenCursor;
  int line_number = 0; 

  string line;
  ll.push(P);
  rpn_generation.push(EMPTY);

  while (getline(*program, line)) {
    int offset = 0;

    while (offset < line.length()) {
      Token *token = tokenCursor.scanToken(line, &offset);

      while (true) {
        vector<int> ll_temp = rpnGrammarTable[ll.top()][token->getType()];
        vector<int> rpn_generation_temp = rpnGenerationTable[ll.top()][token->getType()];
        if (ll.size()) ll.pop();
        if (rpn_generation.top() != EMPTY) {
          rpn.push(rpn_generation.top());
        }
        if (rpn_generation.size()) {
          rpn_generation.pop();
        }

        for (int i = ll_temp.size() - 1; i > 0; --i) {
          ll.push(ll_temp[i]);
          rpn_generation.push(rpn_generation_temp[i]);
        }

        if (ll.top() < 7) break;
      } 
    }
  }

  for (int i = 0; i < rpn.size(); ++i) {
    cout << rpn.top() << endl;
    rpn.pop();
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

//int RPN::execute() {
//  stack<int> result;
//
//  for (auto token : this->result) {
//    if (this->isNumber(token)) {
//      result.push(stoi(token->getLiteral()));
//    } else if (token->getType() == IDENTIFIER) {
//      if (this->variables.count(token->getLiteral()) > 0) {
//        result.push(stoi(this->variables.at(token->getLiteral())));
//      }
//    } else if (this->isOperation(token)){
//      int second_operand = result.top();
//      result.pop();
//
//      int first_operand = result.top();
//      result.pop();
//
//      switch (token->getType()) {
//        case PLUS:
//          result.push(first_operand + second_operand);
//          break;
//        case MINUS:
//          result.push(first_operand - second_operand);
//          break;
//        case MULTIPLY:
//          result.push(first_operand * second_operand);
//          break;
//        case DIVIDE:
//          result.push(first_operand / second_operand);
//          break;
//        default:
//          cout << "Default Operation" << endl;
//          break;
//      }
//    }
//
//  }
//
//  return result.top();
//}
  
