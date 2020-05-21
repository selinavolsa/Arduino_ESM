#include "Items.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <TimeLib.h>

// definition of the .csv header
const char* EsmBaseItem::header =  "quest_name;item_name;type;prompt;left_label;right_label;resolution;response;timestamp";

void EsmBaseItem::printPrompt(const char* prompt, unsigned int x, unsigned int y, unsigned int maxX)
{
  char workPrompt[PROMPTLENGTH];
  strcpy(workPrompt, prompt);

  unsigned int maxChars = (maxX - x) / (FONT_WIDTH + 1); // the maximum number of characters that will fit in a line, respecting the left and right margins
  unsigned int lineStart = 0; // index of the character with which the current line starts
  unsigned int lastSpace = 0; // position of the last space encountered
  unsigned int numLines = 0; // number of previous lines

  tft->setCursor(x, y);
  tft->setTextSize(1);
  tft->setTextColor(WHITE);

  bool foundLast = false;
  for(int i = 0; !foundLast; i++) // loop through prompt until the terminating 0 is encountered
  {
    if(prompt[i] == ' ' || prompt[i] == 0)
    {
      if(i - lineStart > maxChars)
      // if the current line would already be longer than the maximum length, then print up until the last encountered space, and start a new line after that
      {
        workPrompt[lastSpace] = 0;
        tft->print(&workPrompt[lineStart]);
        lineStart = lastSpace + 1;
        tft->setCursor(x, y + LINE_HEIGHT * ++numLines);
      }
      else
      {
      // if the current line still fits, update the index of the last encountered space
        lastSpace = i;
      }
    }
    if(prompt[i] == 0)
    {
    // if the terminating zero is encountered, terminate the loop
      foundLast = true;
    }
  }
  // print the remaining part of the prompt
  tft->print(&workPrompt[lineStart]);
}

// initializing all non constant static rendering paramteters to 0
unsigned int EsmItemLikert::yPrompt = 0;
unsigned int EsmItemLikert::yScale = 0;
unsigned int EsmItemLikert::yLabels = 0;
unsigned int EsmItemLikert::scaleOffset = 0;
unsigned int EsmItemLikert::maxXPrompt = 0;

EsmItemLikert::EsmItemLikert(const char* parentQuestionnaire, const char* itemName, const char* prompt, byte range, const char* leftLabel, const char* rightLabel)
{
  this->range = range;
  strcpy(this->parentQuestionnaire, parentQuestionnaire);
  strcpy(this->leftLabel, leftLabel);
  strcpy(this->rightLabel, rightLabel);
  strcpy(this->prompt, prompt);
  strcpy(this->itemName, itemName);

  value = range / 2 + 1; // the current value is initialized to be in the middle of the scale

  calculateParameters();
}

void EsmItemLikert::calculateParameters()
{
  // if the static parameters are not yet calculated (i.e. equal 0), calculate them, otherwise skip them
  if(yPrompt == 0 || yScale == 0)
  {
    int tftHeight = tft->height();
    yPrompt = tftHeight / 4; // prompt text will start at one quarter from the screen top
    yScale = tftHeight - tftHeight / 4; // the scale will be one quarter from the screen bottom
  }
  if(yLabels == 0)
  {
    yLabels = yScale + 15 + radiusRing; // labels will be 15 pixels below the circles
  }
  if(scaleOffset == 0)
  {
    scaleOffset = 2 * radiusRing + ringSeparation; // offset from one point center to the next
  }
  if(maxXPrompt == 0)
  {
    maxXPrompt = tft->width() - xPrompt; // right margin is the same as right margin
  }

  int tftWidth = tft->width();
  // getTextBounds() requires four values to fill, even if only one is needed later
  int16_t x1, y1;
  uint16_t w, h;
  tft->getTextBounds(rightLabel, 0, yLabels, &x1, &y1, &w, &h); // only the width of the label, w, is required
  xRightLabel = tftWidth - w - marginLabels; // this will ensure that the right end of the label will be exactly one margin from the right screen edge

  int scaleWidth = (range - 1) * scaleOffset; // the total width of the scale
  xScale = (tftWidth - scaleWidth) / 2; // xScale is calculated to center the scale
}

void EsmItemLikert::draw()
{
  // fill the screen black, then print the prompt and draw the scale
  tft->fillScreen(BLACK);

  tft->setCursor(xPrompt, yPrompt);
  tft->setTextColor(WHITE);
  tft->setTextSize(1);

  printPrompt(prompt, xPrompt, yPrompt, maxXPrompt);

  drawLikert();
}

void EsmItemLikert::drawLikert()
{
  tft->setTextSize(1);
  tft->setTextColor(WHITE, BLACK);

  // print both labels

  tft->setCursor(marginLabels, yLabels);
  tft->print(leftLabel);

  tft->setCursor(xRightLabel, yLabels);
  tft->print(rightLabel);

  // print all the circles, then overdraw the center with black
  for(int i = 0; i < range; i++)
  {
    tft->drawCircle(xScale + i * scaleOffset, yScale, radiusRing, WHITE);
    tft->fillCircle(xScale + i * scaleOffset, yScale, radiusSelector, BLACK);
  }
  tft->fillCircle(xScale + (value - 1) * scaleOffset, yScale, radiusSelector, RED); // fill only the currently selecte circle with red
}

void EsmItemLikert::update(byte buttonMask)
{
  if(buttonMask & BTN_LEFT)
  {
    // if the left button is pressed, decrease the value (if possible)
    value = value > 1 ? value -1 : 1;
    drawLikert();
  }
  if(buttonMask & BTN_RIGHT)
  {
    // if the right button is pressed, increase the value (if possible
    value = value < range ? value + 1 : range;
    drawLikert();
  }
}

void EsmItemLikert::result(char* logstring)
{
  // create a line of a csv file for this variable, filling it with all available data
  sprintf(logstring, "%s;%s;likert;%s;%s;%s;%d;%d;%d", parentQuestionnaire, itemName, prompt, leftLabel, rightLabel, range, value, now());
}
