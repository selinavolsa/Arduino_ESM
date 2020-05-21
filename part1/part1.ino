#include "Items.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <TimeLib.h>
#include "global_defines.h"

EsmBaseItem* currentItem = nullptr; // the base item pointer to later hold our item

Adafruit_ST7735* tft = nullptr;
  
byte getButtonMask() // function to get the currently pressed button, and compress that into a single byte
{
  byte buttons = 0x0000;

  byte pins[] = {PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_DOWN, PIN_ACCEPT};
  byte button_codes[] = {BTN_LEFT, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_ACCEPT};
  int numCodes = 5;

  for(int i = 0; i < numCodes; i++) // loop through all pins and set the pins appropriately
  {
    if(digitalRead(pins[i]) == LOW)
    {
      buttons |= button_codes[i];
    }
  }

  return buttons;
}

void setup() {
  Serial.begin(9600);
  Serial.println(EsmBaseItem::header); // print the header on the serial connection
  
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

  currentItem = new EsmItemLikert("Foo", "Bar", "A sample likert type item.", 7, "1", "7"); // create a sample item
  currentItem->draw(); // draw the item
}

void loop() {
  byte buttonMask = getButtonMask(); // get the currently pressed buttons
  if(buttonMask & BTN_ACCEPT)
  {
    // if the accept button is pressed, print the current result of the item on the serial connection
    char logstring[LOGSTRINGLENGTH];
    currentItem->result(logstring);
    Serial.println(logstring);
  }
  else
  {
    // if the accept button is not pressed, forward the updating to the item
    currentItem->update(buttonMask);
  }
  delay(FRAMETIME);
}
