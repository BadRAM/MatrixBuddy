#include <LedControl.h>

int DIN = 11;
int CS = 7;
int CLK  = 13;
LedControl lc=LedControl(DIN, CLK, CS,0);
int Happy = true;

int Smile[4] ={
  B00000000,
  B00100100,
  B00011000,
  B00000000
};

int Mischief[4] ={
  B00000000,
  B00101010,
  B00010100,
  B00000000
};

int Cat[4] ={
  B00000000,
  B01000010,
  B10011001,
  B01100110
};

int Fangs[4] ={
  B00000000,
  B01111110,
  B00100100,
  B00000000
};

int Neutral[4] ={
  B00000000,
  B00000000,
  B00111100,
  B00000000
};

int Pout[4] ={
  B00000000,
  B00011000,
  B00100100,
  B00000000
};

int Uncertain[4] ={
  B00000000,
  B00010100,
  B00101000,
  B00000000
};

int Eyes[4] ={
  B00000000,
  B00100100,
  B01100110,
  B00100100
};

int Wink1[4] ={
  B00000000,
  B00100000,
  B01100110,
  B00100100
};

int Wink2[8] ={
  B00000000,
  B00100000,
  B01100000,
  B00100110
};

int Blink1[8] ={
  B00000000,
  B00000000,
  B01100110,
  B00100100
};

int Blink2[8] ={
  B00000000,
  B00000000,
  B00000000,
  B01100110
};

int Squint[4] ={
  B00000000,
  B00000000,
  B01100110,
  B00000000
};

int LookUp[4] ={
  B00100100,
  B00100100,
  B01100110,
  B00000000
};

int LookLeft[4] ={
  B00000000,
  B01001000,
  B01101100,
  B01001000
};

int LookRight[4] ={
  B00000000,
  B00010010,
  B00110110,
  B00010010
};

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

void blink()
{
  drawEyes(Blink1);
  delay(10);
  drawEyes(Blink2);
  delay(50);
  drawEyes(Blink1);
  delay(50);
}

void wink()
{
  drawEyes(Wink1);
  delay(10);
  drawEyes(Wink2);
  delay(300);
  drawEyes(Wink1);
  delay(50);
}

void setup()
{
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);

  CurrentEyes = &Eyes;
  CurrentMouth = &Smile;

  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);

  pinMode(2, INPUT_PULLUP);

  randomSeed(analogRead(A7));
}
  
void loop(){
  delay(random(2000, 8000));
  int roll = random(1, 100);
  if (digitalRead(2) == LOW)
  {
    // digitalWrite(LED_BUILTIN, HIGH);
    roll = 1;
  }
  else
  {
    // digitalWrite(LED_BUILTIN, LOW);
  }
  if (roll < 10) // change mood
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
  else if (roll < 30) // Look in random direction
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
    delay(random(100, 1000));
  }
  else if (roll < 40) // Emotion event. Wink if happy, squint if not.
  {
    if (Happy)
    {
      wink();
    }
    else
    {
      drawEyes(Squint);
      delay(random(800, 4000));
    }
  }
  else if (roll < 50) // double blink
  {
    blink();
    delay(50);
    blink();
  }
  else if (roll == 51)
  {
    CurrentMouth = &Cat;
    Happy = true;
  }
  else // blink most of the time
  {
    blink();
  }
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
}