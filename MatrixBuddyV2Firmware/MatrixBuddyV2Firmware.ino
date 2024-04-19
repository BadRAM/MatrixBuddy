#include <FlashStorage_SAMD.h>
#include "LedControl.h"
#include "SpriteData.h"

#define LIGHT 0

#define BUTT1 1
#define BUTT2 2
#define BUTT3 3

#define EVENTQUEUELEN 64

int DIN = 9;
int CS = 10;
int CLK  = 8;
LedControl lc=LedControl(DIN, CLK, CS, 1);

unsigned int Butt1Hist = 0;
unsigned int Butt2Hist = 0;
unsigned int Butt3Hist = 0;

unsigned long LastButtonPress = 0;

// animEvents contain a function pointer, an argument pointer, and 
// a delay. arugmentless functions may be called by casting them
// to the expected type and setting arg to null.
struct animEvent
{
  void (*func)(int*);
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
unsigned long LastFrame = 0;
int FrameInterval = 10;

int LightLevel = 512;
unsigned long LightWatchdog = 0;

byte Brightness = 0;
byte BrightnessAddr = 1;
const int SleepThreshValues[16] = 
{
  512, 1024, 1536, 2048, 
  2560, 3072, 3328, 3584, 
  3712, 3840, 3904, 3968, 
  4032, 4064, 4080, 4096
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

byte FrameBuffer[8] = {0,0,0,0,0,0,0,0};
// {
//   0b11110011,
//   0b10000001,
//   0b00011001,
//   0b00111101,
//   0b10011000,
//   0b10011000,
//   0b10000001,
//   0b11001111,
// };

void drawEyes(int eyes[4])
{
  for(int i=0;i<4;i++) FrameBuffer[i] = eyes[i];
  updateDisplay();
}

void drawMouth(int mouth[4])
{
  for(int i=4;i<8;i++) FrameBuffer[i] = mouth[i-4];
  updateDisplay();
}

// rotate and flip the display buffer while outputting it as setrow instructions.
void updateDisplay()
{
  byte OutputBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (int x=0; x<8; x++)
  {
    for (int y=0; y<8; y++)
    {
      OutputBuffer[x] |= ((FrameBuffer[(14-y)%8] >> x) & 1) << y;
    }
  }
  lc.setRow(0, 0, OutputBuffer[0]);
  lc.setRow(0, 1, OutputBuffer[1]);
  lc.setRow(0, 2, OutputBuffer[2]);
  lc.setRow(0, 3, OutputBuffer[3]);
  lc.setRow(0, 4, OutputBuffer[4]);
  lc.setRow(0, 5, OutputBuffer[5]);
  lc.setRow(0, 6, OutputBuffer[6]);
  lc.setRow(0, 7, OutputBuffer[7]);
}

void clearDisplay()
{
  lc.clearDisplay(0);
  for (int i=0; i<8; i++)
  {
    FrameBuffer[i] = 0;
  }
}

void addEvent(void (*func)(int*), int (*arg)[4], unsigned long delay)
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
  addEvent(&drawEyes, &Blink2, 50);
  addEvent(&drawEyes, &Blink2, 50);
}

void openEyes()
{
  addEvent(&drawEyes, &Blink2, 0);
  addEvent(&drawEyes, &Blink1, 50);
  addEvent(&drawEyes, CurrentEyes, 50);
}

void blink()
{
  // Serial.println("blink");
  addEvent(&drawEyes, &Blink1, 0);
  addEvent(&drawEyes, &Blink2, 50);
  addEvent(&drawEyes, &Blink1, 100);
  addEvent(&drawEyes, CurrentEyes, 100);
}

void wink()
{
  // Serial.println("wink");
  addEvent(&drawEyes, &Wink1, 0);
  addEvent(&drawEyes, &Wink2, 50);
  addEvent(&drawEyes, &Wink1, 300);
  addEvent(&drawEyes, CurrentEyes, 50);
}

void yawn()
{
  //Serial.println("yawn");
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
  // Manually read ADC
  ADC->SWTRIG.bit.START = 1;                       // Initiate a software trigger to start an ADC conversion
  while(ADC->STATUS.bit.SYNCBUSY);                 // Wait for write synchronization
  while(!ADC->INTFLAG.bit.RESRDY);                 // Wait for the conversion to complete
  ADC->INTFLAG.bit.RESRDY = 1;                     // Clear the result ready (RESRDY) interrupt flag
  while(ADC->STATUS.bit.SYNCBUSY);                 // Wait for read synchronization
  int result = ADC->RESULT.reg;                    // Read the ADC result
  
  LightLevel = (LightLevel + result) / 2;
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
  clearDisplay();
  unsigned long litSince = 0;
  while (true)
  {
    updateLightLevel();
    updateButtons();

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

    if (buttonPressed(&Butt1Hist))
    {
      sleepThreshConfig(false);
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
  lc.setRow(0, 7 - row, 0xFF >> e + 1 | ((e > 7 ? 0 : 1) << 7));
}

void screenBrightnessConfig()
{
  clearEvents();
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
  closeEyes();
  finishEvents();
  updateButtons();
  clearDisplay();
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
  EEPROM.commit();
  // Serial.println(EEPROM.read(BrightnessAddr));

  drawMouth(*CurrentMouth);
  openEyes();
  finishEvents();

  // prevent instant random action
  LastAction = millis();
}

void sleepThreshConfig(bool anim)
{
  if (anim)
  {
    clearEvents();
    drawEyes(*CurrentEyes);
    drawMouth(*CurrentMouth);
    closeEyes();
    finishEvents();
  }

  updateButtons();
  clearDisplay();
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
  EEPROM.commit();
  // Serial.println(EEPROM.read(SleepThreshAddr));

  if (anim)
  {
    drawMouth(*CurrentMouth);
    openEyes();
    finishEvents();
  }
  else
  {
    clearDisplay();
  }

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
  EEPROM.commit();

  // prevent instant random action
  LastAction = millis();
}

void randomAction()
{
  int roll = random(1, 100);

  // if(digitalRead(BUTT1) == LOW) roll = 1;
  // if(digitalRead(BUTT2) == LOW) roll = 2;
  // if(digitalRead(BUTT3) == LOW) roll = 3;

  if (roll == 1 && Happy == true)
  {
    addEvent(&drawMouth, &Cat, 0);
    addEvent(&drawMouth, CurrentMouth, 2000);
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

void updateButton(int button, unsigned int* history)
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
      buttonPressed(&Butt2Hist) || 
      buttonPressed(&Butt3Hist))
  {
    LastButtonPress = millis();
  }
}

bool buttonPressed(unsigned int* history)
{
  return (*history & 0xFF) == 0xFE;
}

void setup()
{
  Serial.begin(57600);

  //while(!Serial);                               // Wait for the console to open

  //Serial.println("SERIAL ONLINE");

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

  //Serial.println("Initializing ADC");

  // Manually configure ADC for light sensor
  pinMode(LIGHT, INPUT_PULLUP);
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |    // Divide Clock ADC GCLK by 512 (48MHz/512 = 93.7kHz)
                   ADC_CTRLB_RESSEL_12BIT;         // Set ADC resolution to 12 bits
  while(ADC->STATUS.bit.SYNCBUSY);                 // Wait for synchronization
  ADC->SAMPCTRL.reg = 0x08;                        // Set max Sampling Time Length to half divided ADC clock pulse times SAMPCTRL (341.33us)
  ADC->INPUTCTRL.bit.MUXPOS = 0x0;                 // Set the analog input to A0
  while(ADC->STATUS.bit.SYNCBUSY);                 // Wait for synchronization
  ADC->CTRLA.bit.ENABLE = 1;                       // Enable the ADC
  while(ADC->STATUS.bit.SYNCBUSY);                 // Wait for synchronization

  //Serial.println("ADC ONLINE");

  updateLightLevel();
  randomSeed(LightLevel);

  //Serial.println("SEED SET");

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
  // Serial.print("Butt1: ");
  // Serial.print(Butt1Hist, BIN);
  // Serial.print(" Butt2: ");
  // Serial.print(Butt2Hist, BIN);
  // Serial.print(" Butt3: ");
  // Serial.print(Butt3Hist, BIN);
  // Serial.print(" LightSensor: ");
  // Serial.println(digitalRead(LIGHT));

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
    //Serial.println("Butt 1 pressed");
    sleepThreshConfig(true);
    return;
  }

  // Check for button presses
  if(buttonPressed(&Butt2Hist))
  {
    //Serial.println("Butt 2 pressed");
    toggleStrongMode();
    return;
  }

  // Check for button presses
  if(buttonPressed(&Butt3Hist))
  {
    //Serial.println("Butt 3 pressed");
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

  // delay until it's time to start the next frame.
  while(millis() - LastFrame < FrameInterval)
  LastFrame = millis();
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
