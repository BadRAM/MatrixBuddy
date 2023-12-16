#include <LedControl.h>
#include <EEPROM.h>
#include "SpriteData.h"

#define LIGHT A0

#define BUTT1 A1
#define BUTT2 A2
#define BUTT3 A3

#define EVENTQUEUELEN 64

int DIN = 11;
int CS = 7;
int CLK  = 13;
LedControl lc=LedControl(DIN, CLK, CS,0);

unsigned int Butt1Hist = 0;
unsigned int Butt2Hist = 0;
unsigned int Butt3Hist = 0;

unsigned long LastButtonPress = 0;

// animEvents contain a function pointer, an argument pointer, and 
// a delay. arugmentless functions may be called by casting them
// to the expected type and setting arg to null.
struct animEvent
{
  void (*func)(int);
  int (*arg)[4];
  unsigned long delay;
};

int nextEvent = 0; // the event that's going to happen next
int lastEvent = 0; // the last event in the queue
struct animEvent events[EVENTQUEUELEN];

// the millis() time of the last event's completion
unsigned long prevEvent = 0;

unsigned long LastAction = 0;
int ActionInterval = 2000;

int LightLevel = 512;
unsigned long LightWatchdog = 0;

byte Brightness = 0;
byte BrightnessAddr = 1;
const int SleepThreshValues[16] = 
{
  128, 256, 384, 512, 
  640, 768, 832, 896, 
  928, 960, 976, 992, 
  1008, 1016, 1020, 1024
};
byte SleepThresh = 8;
byte SleepThreshAddr = 2;
byte WakeThresh = 6;
byte WakeThreshAddr = 3;
bool StrongMode = false;
byte StrongModeAddr = 4;

bool Happy = true;

bool BridgeActive = false;
unsigned long LastBridgeMessage = 0;
unsigned long LastBridgeEyeClose = 0;
bool SingMode = false;

int (*CurrentEyes)[4];
int (*CurrentMouth)[4];

void drawEyes(int eyes[4])
{
  for(int i=0;i<4;i++) lc.setColumn(0,7-i,eyes[i]);
}

void drawMouth(int mouth[4])
{
  for(int i=4;i<8;i++) lc.setColumn(0,7-i,mouth[i-4]);
}

void addEvent(void (*func)(int), int (*arg)[4], unsigned long delay)
{
  if (events[nextEvent].func == NULL)
  {
    lastEvent = nextEvent;
    prevEvent = millis();
  }
  else
  {
    // increment lastEvent, with guard against overrun.
    int e = (lastEvent + 1) % EVENTQUEUELEN;
    if (e == nextEvent)
    {
      // Serial.println("event queue overrun!");
      return;
    }
    lastEvent = e;
  }

  events[lastEvent].func = func;
  events[lastEvent].arg = arg;
  events[lastEvent].delay = delay;
}

void closeEyes()
{
  addEvent(&drawEyes, &Blink1, 0);
  addEvent(&drawEyes, &Blink2, 20);
  addEvent(&drawEyes, &Blink2, 20);
}

void openEyes()
{
  addEvent(&drawEyes, &Blink2, 0);
  addEvent(&drawEyes, &Blink1, 20);
  addEvent(&drawEyes, CurrentEyes, 20);
}

void blink()
{
  // Serial.println("blink");
  addEvent(&drawEyes, &Blink1, 0);
  addEvent(&drawEyes, &Blink2, 10);
  addEvent(&drawEyes, &Blink1, 50);
  addEvent(&drawEyes, CurrentEyes, 50);
}

void wink()
{
  // Serial.println("wink");
  addEvent(&drawEyes, &Wink1, 0);
  addEvent(&drawEyes, &Wink2, 10);
  addEvent(&drawEyes, &Wink1, 300);
  addEvent(&drawEyes, CurrentEyes, 50);
}

void yawn()
{
  // Serial.println("yawn");
  addEvent(&drawMouth, &Yawn1,  0);
  addEvent(&drawMouth, &Yawn2,  100);
  addEvent(&drawEyes,  &Squint, 100);
  addEvent(&drawMouth, &Yawn3,  0);
  addEvent(&drawMouth, &Yawn4,  100);
  addEvent(&drawMouth, &Yawn3,  2000);
  addEvent(&drawMouth, &Yawn4,  200);
  addEvent(&drawMouth, &Yawn3,  200);
  addEvent(&drawMouth, &Yawn2,  100);
  addEvent(&drawMouth, &Yawn1,  100);
  addEvent(&drawEyes, CurrentEyes,  0);
  addEvent(&drawMouth, CurrentMouth,  100);
  addEvent(&drawEyes, CurrentEyes, 50);
  blink();
  blink();
}

void updateLightLevel()
{
  // LightLevel = analogRead(LIGHT);
  LightLevel = (LightLevel + analogRead(LIGHT)) / 2;
  // Serial.println(LightLevel);
}

void sleep()
{
  CurrentMouth = &Smile;
  yawn();
  finishEvents();
  updateLightLevel();
  delay(100);
  updateLightLevel();
  delay(100);
  updateLightLevel();
  delay(100);
  updateLightLevel();
  if (LightLevel < SleepThreshValues[SleepThresh])
  {
    return;
  }
  delay(200);
  drawEyes(Blink1);
  delay(250);
  drawMouth(SleepMouth);
  delay(250);
  drawEyes(Blink2);
  delay(2000);
  lc.clearDisplay(0);
  unsigned long litSince = 0;
  while (true)
  {
    updateLightLevel();
    if (LightLevel < SleepThreshValues[WakeThresh])
    {
      if (litSince == 0)
      {
        litSince = millis();
      }
      else if (millis() - litSince >= 4000)
      {
        break;
      }
    }
    else
    {
      litSince = 0;
    }

    // Serial.print("litSince: ");
    // Serial.print(litSince);
    // Serial.print(", Lightlevel: ");
    // Serial.println(LightLevel);
    delay(500);
  }
  delay(1000);
  drawEyes(Blink2);
  drawMouth(SleepMouth);
  delay(500);
  drawEyes(Blink1);
  delay(500);
  CurrentMouth = &Smile;
  Happy = true;
  drawMouth(Smile);
  yawn();

  // prevent instant random action
  LastAction = millis();
}

void drawBarDisplay(int row, byte val)
{
  byte e = (8 - (val + 1 +(millis() % 200 > 100)) / 2);
  lc.setRow(0, row, 0xFF << e);
}

void screenBrightnessConfig()
{
  clearEvents();
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
  closeEyes();
  finishEvents();
  updateButtons();
  lc.clearDisplay(0);
  drawEyes(BrightnessIcon);

  while (!buttonPressed(&Butt2Hist))
  {
    drawBarDisplay(6, Brightness);

    delay(5);
    updateButtons();

    if (buttonPressed(&Butt1Hist) && Brightness != 0) Brightness--;
    if (buttonPressed(&Butt3Hist) && Brightness != 15) Brightness++;
    Brightness = constrain(Brightness, 0, 15);

    lc.setIntensity(0, Brightness);
  }

  EEPROM.write(BrightnessAddr, Brightness);
  // Serial.println(EEPROM.read(BrightnessAddr));

  drawMouth(*CurrentMouth);
  openEyes();
  finishEvents();

  // prevent instant random action
  LastAction = millis();
}

void sleepThreshConfig()
{
  clearEvents();
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
  closeEyes();
  finishEvents();
  updateButtons();
  lc.clearDisplay(0);
  drawEyes(SleepThreshIcon);

  while (!buttonPressed(&Butt2Hist))
  {
    // byte e = (7 - (SleepThresh + 1) / 2);
    // e = (((SleepThresh % 2) * 2) + 1) << e;
    // lc.setRow(0, 6, e);

    updateLightLevel();
    for (int i = 15; i >= 0; i--)
    {
      if (LightLevel > SleepThreshValues[i])
      {
        drawBarDisplay(5, i);
        break;
      }
    }

    drawBarDisplay(6, SleepThresh);
    drawBarDisplay(7, WakeThresh);

    delay(5);
    updateButtons();

    if (buttonPressed(&Butt1Hist) && SleepThresh != 0) SleepThresh--;
    if (buttonPressed(&Butt3Hist) && SleepThresh != 15) SleepThresh++;
    SleepThresh = constrain(SleepThresh, 0, 15);
    WakeThresh = SleepThresh - min(2, SleepThresh);

    // Serial.print("SleepThresh: ");
    // Serial.println(SleepThresh);
  }

  EEPROM.write(SleepThreshAddr, SleepThresh);
  EEPROM.write(WakeThreshAddr, WakeThresh);
  // Serial.println(EEPROM.read(SleepThreshAddr));

  drawMouth(*CurrentMouth);
  openEyes();
  finishEvents();

  // prevent instant random action
  LastAction = millis();
}

void toggleStrongMode()
{
  finishEvents();

  if (!StrongMode)
  {
    Happy = true;
    CurrentMouth = &Smile;
    drawMouth(*CurrentMouth);
    StrongMode = true;
    wink();
  }
  else
  {
    Happy = true;
    CurrentMouth = &Mischief;
    drawMouth(*CurrentMouth);
    StrongMode = false;
    blink();
  }

  EEPROM.write(StrongModeAddr, StrongMode);

  // prevent instant random action
  LastAction = millis();
}

void randomAction()
{
  int roll = random(1, 100);

  // if(digitalRead(BUTT1) == LOW) roll = 1;
  // if(digitalRead(BUTT2) == LOW) roll = 2;
  // if(digitalRead(BUTT3) == LOW) roll = 3;

  if (roll == 1)
  {
    CurrentMouth = &Cat;
    Happy = true;
    drawMouth(*CurrentMouth);
  }
  else if (roll == 2)
  {
    yawn();
  }
  else if (roll == 3)
  {
    CurrentMouth = &Smile;
    Happy = true;
    drawMouth(*CurrentMouth);
  }
  else if (roll < 15) // change mood
  {
    int mood = 2;

    if (CurrentMouth == &Pout)
    {
      mood = random(4, 6);
    }
    else if (CurrentMouth == &Neutral || CurrentMouth == &Uncertain)
    {
      mood = random(2, 7);
    }
    else if (CurrentMouth == &Smile)
    {
      mood = random(1, 6);
    }

    if (StrongMode && mood >= 4)
    {
      mood = 3;
    }

    switch (mood)
    {
      case 1:
        CurrentMouth = &Mischief;
        Happy = true;
        break;
      case 2:
        CurrentMouth = &Smile;
        Happy = true;
        break;
      case 3:
        CurrentMouth = &Smile;
        Happy = true;
        break;
      case 4:
        CurrentMouth = &Neutral;
        Happy = false;
        break;
      case 5:
        CurrentMouth = &Uncertain;
        Happy = false;
        break;
      case 6:
        CurrentMouth = &Pout;
        Happy = false;
        break;
    }
  }
  else if (roll < 35) // Look in random direction
  {
    switch (random(1,4))
    {
      case 1:
        drawEyes(LookLeft);
        break;
      case 2:
        drawEyes(LookUp);
        break;
      case 3:
        drawEyes(LookRight);
        break;
      default:
        drawEyes(LookUp);
        break;
    }
    //delay(random(500, 2000));
    addEvent(&drawEyes, CurrentEyes, random(500, 2000));
  }
  else if (roll < 45) // Emotion event. Wink if happy, squint if not.
  {
    if (Happy)
    {
      wink();
    }
    else
    {
      drawEyes(Squint);
      addEvent(&drawEyes, CurrentEyes, random(800, 4000));
      // delay(random(800, 4000));
    }
  }
  else // blink most of the time
  {
    blink();
    while(random(1, 6) == 1)
    {
      addEvent(&drawEyes, CurrentEyes, 50);
      blink();
    }
  }
}

void updateEvents()
{
  // abort if no events.
  if (events[nextEvent].func == NULL) return;

  if (millis() - prevEvent >= events[nextEvent].delay)
  {
    // Serial.print("event time, nextEvent: ");
    // Serial.print(nextEvent);
    // Serial.print(" delay: ");
    // Serial.print(events[nextEvent].delay);
    // Serial.print(" mil since lE: ");
    // Serial.print(millis() - prevEvent);
    // Serial.print(" lastEvent: ");
    // Serial.print(lastEvent);
    // Serial.print(" - ");

    // do event
    if (events[nextEvent].arg == NULL)
    {
      // Serial.println("Called Event()");
      ((void*)*events[nextEvent].func);
    }
    else
    {
      // Serial.println("Called Event(arg)");
      (*events[nextEvent].func)(*events[nextEvent].arg);
    }

    // clear event
    events[nextEvent].func = 0;
    events[nextEvent].arg = 0;
    events[nextEvent].delay = 0;

    // increment nextEvent
    nextEvent++;
    nextEvent = nextEvent % EVENTQUEUELEN;

    // update prevEvent
    prevEvent = millis();
  }
  else
  {
    //Serial.print(".");
  }
}

void finishEvents()
{
  while (events[nextEvent].func != NULL)
  {
    updateEvents();
    delay(1);
  }
}

void clearEvents()
{
  for (int i = 0; i < EVENTQUEUELEN; i++)
  {
    events[i].func = 0;
    events[i].arg = 0;
    events[i].delay = 0;
  }

  nextEvent = 0;
  lastEvent = 0;
}

void updateButton(int button, int* history)
{
  *history = *history << 1;
  *history |= digitalRead(button);
}

void updateButtons()
{
  updateButton(BUTT1, &Butt1Hist);
  updateButton(BUTT2, &Butt2Hist);
  updateButton(BUTT3, &Butt3Hist);
  if (buttonPressed(&Butt1Hist) || 
      buttonPressed(&Butt1Hist) || 
      buttonPressed(&Butt1Hist))
  {
    LastButtonPress = millis();
  }
}

bool buttonPressed(int* history)
{
  return *history == 0xFFFE;
}

void setup()
{
  Serial.begin(57600);

  Brightness = EEPROM.read(BrightnessAddr);
  if (Brightness == 255) Brightness = 0;
  SleepThresh = EEPROM.read(SleepThreshAddr);
  if (SleepThresh == 255) SleepThresh = 8;
  WakeThresh = EEPROM.read(WakeThreshAddr);
  if (WakeThresh == 255) WakeThresh = 6;
  StrongMode = EEPROM.read(StrongModeAddr);
  if (StrongMode == 255) StrongMode = 0;

  lc.shutdown(0,false);
  lc.setIntensity(0, Brightness);
  lc.clearDisplay(0);

  pinMode(BUTT1, INPUT_PULLUP);
  pinMode(BUTT2, INPUT_PULLUP);
  pinMode(BUTT3, INPUT_PULLUP);
  pinMode(LIGHT, INPUT_PULLUP);

  randomSeed(analogRead(A7));

  LightLevel = analogRead(LIGHT);

  CurrentEyes = &Eyes;
  CurrentMouth = &Smile;

  drawEyes(Blink2);
  delay(200);
  drawMouth(SleepMouth);
  drawEyes(Blink1);
  delay(200);
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
  delay(500);
}

void loop()
{
  updateButtons();

  // update light level and check if sleepy time
  updateLightLevel();
  if (LightLevel >= SleepThreshValues[SleepThresh])
  {
    sleep();
    return;
  }

  //Serial.println(Butt1Hist, BIN);
  // Check for button presses
  if(buttonPressed(&Butt1Hist))
  {
    // Serial.println("Butt 1 pressed");
    sleepThreshConfig();
    return;
  }

  // Check for button presses
  if(buttonPressed(&Butt2Hist))
  {
    // Serial.println("Butt 2 pressed");
    toggleStrongMode();
    return;
  }

  // Check for button presses
  if(buttonPressed(&Butt3Hist))
  {
    // Serial.println("Butt 3 pressed");
    screenBrightnessConfig();
    return;
  }

  if (BridgeActive)
  {
    if (millis() - LastBridgeMessage >= 1000)
    {
      BridgeActive = false;
      drawMouth(*CurrentMouth);
      drawEyes(*CurrentEyes);
      SingMode = false;
    }

    if (millis() - LastBridgeEyeClose >= 600)
    {
      drawEyes(*CurrentEyes);
    }
    delay(5);
    return; // early return to skip random actions
  }

  // Do random event if time is right
  if (millis() - LastAction >= ActionInterval)
  {
    randomAction();
    LastAction = millis();
    ActionInterval = random(3000, 8000);
  }

  updateEvents();

  delay(5);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    switch (inChar)
    {
      case 's':
        // (s)can, do nothing and (a)cknowledge
        Serial.print("a"); // (a)cknowledge execution of command
        break;
      case '0':
        if (BridgeActive)
        {
          drawMouth(*CurrentMouth);
          drawEyes(*CurrentEyes);
          SingMode = false;
          BridgeActive = false;
        }
        break;
      case '1':
        SingMode = false;
        drawMouth(Talk);
        break;
      case '2':
      case '3':
        if (SingMode)
        {
          drawMouth(Sing);
          LastBridgeEyeClose = millis();
        }
        else
        {
          drawMouth(Yawn3);
        }
        break;
      case '4':
        SingMode = true;
        drawMouth(Yawn4);
        drawEyes(Squint);
        LastBridgeEyeClose = millis();
        break;
    }

    if ((inChar != 's') && (inChar != '0'))
    {
      BridgeActive = true;
      LastBridgeMessage = millis();
    }
  }
}
