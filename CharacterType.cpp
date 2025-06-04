#include "CharacterType.h"

CharacterType::CharacterType(char character) {
  this->character = character;
}

CharacterTypeEnum CharacterType::type() {
  if (this->character >= '0' && this->character <= '9') {
    return NUMBER;
  } else if (
      (this->character >= 'a' && this->character <= 'z') ||
      (this->character >= 'A' && this->character <= 'Z')
      ) {
    return STRING;
  } else {
    return UNDETERMINED;
  }
}
