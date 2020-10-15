#include "QuestionnaireState.h"

#include "Arduino.h"
#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>
#include "global_defines.h"

ItemQueue::ItemQueue()
{
  for(byte i = 0; i < MAXITEMNUM; i++)
  {
    queue[i] = nullptr;
  }
  length = 0;
  index = 0;
}

void ItemQueue::addItem(const char* type, const char* questionnaireName, const char* itemName, const char* prompt, int resolution, const char* leftLabel, const char* rightLabel)
{
  if(length == MAXITEMNUM)
  {
    return;
  }

  if(strcmp(type, "likert") == 0)
  {
    queue[length++] = new EsmItemLikert(questionnaireName, itemName, prompt, resolution, leftLabel, rightLabel);
  }
}

void ItemQueue::makeEmpty()
{
  for(byte i = 0; i < MAXITEMNUM && queue[i] != nullptr; i++)
  {
    delete queue[i];
    queue[i] = nullptr;
  }
  length = 0;
  index = 0;
}

bool ItemQueue::isEmpty()
{
  return length == 0;
}

EsmBaseItem* ItemQueue::currentItem()
{
  return queue[index];
}

void ItemQueue::next()
{
  if(index < length)
  {
    index++;
  }
}

void ItemQueue::first()
{
  index = 0;
}

bool ItemQueue::isLast()
{
  return index == length - 1;
}

void QuestionnaireState::setup(char* stateInfo)
{
  tft ->fillScreen(BLACK);
  strcpy(questionnaireName, stateInfo);

  File file = SD.open(CONFIGFILE, FILE_READ);

  file.find((char*)"\"data\":");
  bool found = false;
  while(!found && file.find((char*)"\"name\":"))
  {
    if(file.findUntil(stateInfo, (char*)","))
    {
      found = true;
      file.find((char*)"\"items\":");
      file.find((char*)"[");
    }
  }

  if(!found)
  {
    file.close();
    return;
  }

  StaticJsonDocument<200> doc;

  do{
    deserializeJson(doc, file);

    const char* name = doc["item"].as<char*>();
    const char* prompt = doc["prompt"].as<char*>();
    const char* type = doc["type"].as<char*>();
    const char* l_lab = doc["l_lab"].as<char*>();
    const char* r_lab = doc["r_lab"].as<char*>();
    int res = doc["res"].as<int>();

    itemQueue.addItem(type, stateInfo, name, prompt, res, l_lab, r_lab);
  }while(file.findUntil((char*)",",(char*)"]"));

  file.close();

  itemQueue.currentItem()->draw();
}

void QuestionnaireState::update(byte pressedMask, byte justPressedMask)
{
  if(justPressedMask & BTN_ACCEPT)
  {
    if(itemQueue.isLast())
    {
      tft->fillScreen(BLACK);
      makeLog();
      itemQueue.makeEmpty();
    }
    else
    {
      itemQueue.next();
      itemQueue.currentItem()->draw();
    }
  }
  else
  {
    itemQueue.currentItem()->update(pressedMask, justPressedMask);
  }
}

void QuestionnaireState::makeLog()
{
  if(itemQueue.isEmpty())
  {
    return;
  }

  bool addHeader = !SD.exists(LOGFILE);

  File file = SD.open(LOGFILE, FILE_WRITE);

  if(!file)
  {
    return;
  }

  if(addHeader)
  {
    file.println(EsmBaseItem::header);
  }

  bool finished = false;
  for(itemQueue.first(); !finished; itemQueue.next())
  {
    char logstring[LOGSTRINGLENGTH];
    itemQueue.currentItem()->result(logstring);

    file.println(logstring);
    finished = itemQueue.isLast();
  }

  file.close();
}

void QuestionnaireState::destroy()
{
  itemQueue.makeEmpty();
}
