#include "Items.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Bounce2.h>
#include <SPI.h>
#include <TimeLib.h>
#include "global_defines.h"

EsmBaseItem* currentItem = nullptr; // the base item pointer to later hold our item

Adafruit_ST7735* tft = nullptr;

byte pressedMask = 0x0000;
byte justPressedMask = 0x0000;

Button leftButton;
Button rightButton;
Button upButton;
Button downButton;
Button acceptButton;

void updateButtonMasks()
{
  leftButton.update();
  rightButton.update();
  upButton.update();
  downButton.update();
  acceptButton.update();

  byte canPressMask = ~ pressedMask;
  pressedMask = 0x0000;

  pressedMask |= BTN_LEFT * leftButton.pressed();
  pressedMask |= BTN_RIGHT * rightButton.pressed();
  pressedMask |= BTN_UP * upButton.pressed();
  pressedMask |= BTN_DOWN * downButton.pressed();
  pressedMask |= BTN_ACCEPT * acceptButton.pressed();

  justPressedMask = pressedMask & canPressMask;
}

void setup() {
  Serial.begin(9600);
  Serial.println(EsmBaseItem::header); // print the header on the serial connection
  
  setTime(0,0,0,1,6,2020); // set the time to some arbitrary value, just so the timestamp will work
  
  tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // create the pointer to the display object
  tft->initR(INITR_BLACKTAB); // initialize the display
  tft->fillScreen(BLACK);
  tft->setRotation(3); // set the rotation of the display (this is dependent on how the display will be oriented

  Button* buttons[] = {&leftButton, &rightButton, &upButton, &downButton, &acceptButton};
  byte pins[] = {PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_DOWN, PIN_ACCEPT};
  for(int i = 0; i < 5; i++) // initialize all pins as inputs, using the internal pullup resistors
  {
    buttons[i]->attach(pins[i], INPUT_PULLUP);
    buttons[i]->interval(DEBOUNCE_INTERVAL);
    buttons[i]->setPressedState(LOW);
  }

  currentItem = new EsmItemLikert("Foo", "Bar", "A sample likert type item.", 7, "1", "7"); // create a sample item
  currentItem->draw(); // draw the item
}

void loop() {
  updateButtonMasks();
  if(pressedMask & BTN_ACCEPT)
  {
    // if the accept button is pressed, print the current result of the item on the serial connection
    char logstring[LOGSTRINGLENGTH];
    currentItem->result(logstring);
    Serial.println(logstring);
  }
  else
  {
    // if the accept button is not pressed, forward the updating to the item
    currentItem->update(pressedMask, justPressedMask);
  }
  delay(FRAMETIME);
}
