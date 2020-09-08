#ifndef ES_ITEMS_h
#define ES_ITEMS_h

#include "Arduino.h"
#include "global_defines.h"

// EsmBaseItem is the base class of all items, and defines a few methods and members all items use
class EsmBaseItem
{
public:
  virtual void update(byte pressedMask, byte justPressedMask){};
  virtual void draw(){};
  virtual void result(char* logstring){};
  
  static const char* header; // this will be the header of the .csv file

protected:
   char parentQuestionnaire[NAMELENGTH]; // name of the parent questionnaire
   char prompt[PROMPTLENGTH]; // question or instruction to display with the item
   char itemName[NAMELENGTH];
   byte value;

   virtual void calculateParameters(){}; // to make the constructor a bit more readable, the calculating of various parameters for the rendering gets its own function
   void printPrompt(const char* prompt, unsigned int x, unsigned int y, unsigned int maxX); // function to print the prompt nicely
};

// EsmItemLikert is an example of a likert scale type item, inheriting from the base item
class EsmItemLikert : public EsmBaseItem
{
public:
  EsmItemLikert(const char* parentQuestionnaire, const char* itemName, const char* prompt, byte range, const char* leftLabel, const char* rightLabel);

  void update(byte pressedMask, byte justPressedMask);
  void draw();
  void result(char* logstring);

private:
  char leftLabel[LABELLENGTH]; // label on the left end of the scale
  char rightLabel[LABELLENGTH]; // label on the right side of the scale
  byte range; // number of steps on the scale

  // all the following values represent pixel values used for setting up the rendering
  static const unsigned int xPrompt = 10; // left margin of the prompt text
  static const unsigned int radiusRing = 7; // radius of the (empty) circle representing a scale step
  static const unsigned int radiusSelector = 5; // radius of the circle in the selected ring
  static const unsigned int marginLabels = 10; // margins of the labels from the left and right screen edges
  static const unsigned int ringSeparation = 2; // separation between the circles
  // the following values will need to be calculated dynamically, depending on other values, but will  not change between items
  static unsigned int yPrompt; // y (start) position of the prompt text
  static unsigned int maxXPrompt; // maximum x value of the prompt (implicitly defining a right margin)
  static unsigned int yScale; // y value of the 
  static unsigned int yLabels; // y value of the labels
  static unsigned int scaleOffset; // x offset between scale points
  // the following values are calculated on a per item basis
  unsigned int xScale; // starting x value of the scale points
  unsigned int xRightLabel; // starting x value of the right label
  
  void calculateParameters();
  void drawLikert(); // function to only redraw the scale, as the prompt doesn't need to be redrawn every time
};

#endif
