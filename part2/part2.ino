#include "QuestionnaireState.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>
#include "global_defines.h"

QuestionnaireState* currentState = nullptr; // the base item pointer to later hold our item

Adafruit_ST7735* tft = nullptr;

byte pressedMask = 0x0000;
byte justPressedMask = 0x0000;

void updateButtonMasks()
{
  byte canPressMask = ~ pressedMask;
  pressedMask = 0x0000;

  if(digitalRead(PIN_LEFT) == LOW)
  {
    pressedMask |= BTN_LEFT;
  }
  if(digitalRead(PIN_RIGHT) == LOW)
  {
    pressedMask |= BTN_RIGHT;
  }
  if(digitalRead(PIN_UP) == LOW)
  {
    pressedMask |= BTN_UP;
  }
  if(digitalRead(PIN_DOWN) == LOW)
  {
    pressedMask |= BTN_DOWN;
  }
  if(digitalRead(PIN_ACCEPT) == LOW)
  {
    pressedMask |= BTN_ACCEPT;
  }

  justPressedMask = pressedMask & canPressMask;
}

void setup() { 
  setTime(0,0,0,1,6,2020); // set the time to some arbitrary value, just so the timestamp will work
  
  tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // create the pointer to the display object
  tft->initR(INITR_BLACKTAB); // initialize the display
  tft->fillScreen(BLACK);
  tft->setRotation(3); // set the rotation of the display (this is dependent on how the display will be oriented

  byte pins[] = {PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_DOWN, PIN_ACCEPT};
  for(int i = 0; i < 5; i++) // initialize all pins as inputs, using the internal pullup resistors
  {
    pinMode(pins[i], INPUT_PULLUP);
  }

  pinMode(SD_CS, OUTPUT);

  if(!SD.begin(SD_CS))
  {
    return;
  }

  

  currentState = new QuestionnaireState();
  currentState->setup((char*)"signal 1");
}

void loop() {
  updateButtonMasks();

  currentState->update(pressedMask, justPressedMask);
  
  delay(FRAMETIME);
}
