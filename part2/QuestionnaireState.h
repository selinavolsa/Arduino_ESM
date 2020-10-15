#ifndef ES_Questionnaire_State_h
#define ES_Questionnaire_State_h

#include "Items.h"
#include "global_defines.h"

class ItemQueue
{
public:
  ItemQueue();

  void addItem(const char* type, const char* questionnaireName, const char* itemName, const char* prompt, int resolution, const char* leftLabel, const char* rightLabel);
  void makeEmpty();
  bool isEmpty();
  EsmBaseItem* currentItem();
  void first();
  void next();
  bool isLast();

private:
  EsmBaseItem* queue[MAXITEMNUM];
  byte length;
  byte index;
};

class QuestionnaireState
{
public:
  void setup(char* stateInfo = (char*) "");
  void update(byte pressedMask, byte justPressedMask);
  void destroy();

private:
  ItemQueue itemQueue;

  char questionnaireName[NAMELENGTH];

  void makeLog();
};

#endif // ES_Questionnaire_State_h
