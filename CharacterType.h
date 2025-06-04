enum CharacterTypeEnum { NUMBER, STRING, UNDETERMINED };

class CharacterType {
  char character;

public:
  CharacterType(char character);
  CharacterTypeEnum type();
};
