#include <LedControl.h>

int BUTT1 = 2;
int BUTT2 = 3;
int BUTT3 = 4;

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

int Yawn1[4] ={
  B00000000,
  B00111100,
  B00000000,
  B00000000
};

int Yawn2[4] ={
  B00000000,
  B00111100,
  B00111100,
  B00000000
};

int Yawn3[4] ={
  B00000000,
  B00111100,
  B00100100,
  B00011000
};

int Yawn4[4] ={
  B00111100,
  B00100100,
  B00100100,
  B00011000
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

void yawn()
{
  drawMouth(Yawn1);
  delay(100);
  drawMouth(Yawn2);
  delay(100);
  drawEyes(Squint);
  drawMouth(Yawn3);
  delay(100);
  drawMouth(Yawn4);
  delay(2000);
  drawMouth(Yawn3);
  delay(200);
  drawMouth(Yawn4);
  delay(200);
  drawMouth(Yawn3);
  delay(100);
  drawMouth(Yawn2);
  delay(100);
  drawMouth(Yawn1);
  drawEyes(*CurrentEyes);
  delay(100);
  drawMouth(*CurrentMouth);
  blink();
  delay(50);
  blink();
  drawEyes(*CurrentEyes);
}

void randomAction()
{
  int roll = random(1, 100);

  if(digitalRead(BUTT1) == LOW) roll = 1;
  if(digitalRead(BUTT2) == LOW) roll = 2;
  if(digitalRead(BUTT3) == LOW) roll = 3;

  if (roll == 1)
  {
    CurrentMouth = &Cat;
    Happy = true;
  }
  else if (roll == 2)
  {
    yawn();
  }
  else if (roll == 3)
  {
    CurrentMouth = &Smile;
    Happy = true;
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
    delay(random(500, 2000));
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
      delay(random(800, 4000));
    }
  }
  else // blink most of the time
  {
    blink();
    while(random(1, 6) == 1)
    {
      delay(50);
      blink();
    }
  }
  drawEyes(*CurrentEyes);
  drawMouth(*CurrentMouth);
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

  pinMode(BUTT1, INPUT_PULLUP);
  pinMode(BUTT2, INPUT_PULLUP);
  pinMode(BUTT3, INPUT_PULLUP);

  delay(1000);
  if(random(1,3) == 1)
  {
    wink();
  }
  else
  {
    yawn();
  }

  randomSeed(analogRead(A7));
}
  
void loop()
{
  delay(random(2000, 8000));
  randomAction();
}


