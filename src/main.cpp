#include "LedControl.h"
#define DIN 11
#define CS 7
#define CLK 13
#define DELAY 100
#define WRAP_BOUNDARY 1
#define RANDOMIZE 0

LedControl lc = LedControl(DIN, CLK, CS, 1);

// 282 iteration long initial pattern
bool matrix[8][8] = {
    {0, 0, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 0, 1, 1, 0},
    {1, 1, 0, 1, 0, 1, 0, 1},
    {0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 0, 1},
};

bool backbuf[8][8] = {{0}};

volatile bool doRandomize = false;

void randomize()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      matrix[i][j] = random(2);
    }
  }
  memcpy(backbuf, matrix, 8 * 8 * sizeof(bool));
}

bool get(int i, int j)
{
#ifdef WRAP_BOUNDARY
  if (i < 0)
    i = 7;
  if (i > 7)
    i = 0;
  if (j < 0)
    j = 7;
  if (j > 7)
    j = 0;
#else
  if (i < 0 || i > 7 || j < 0 || j > 7)
  {
    return false;
  }
#endif
  return matrix[j][i];
}

void GOL()
{
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      int neighbors = get(i - 1, j) + get(i + 1, j) + get(i, j - 1) + get(i, j + 1) + get(i - 1, j - 1) + get(i + 1, j + 1) + get(i - 1, j + 1) + get(i + 1, j - 1);
      if (neighbors < 2)
      {
        backbuf[j][i] = 0;
      }
      else if (neighbors > 3)
      {
        backbuf[j][i] = 0;
      }
      else if (neighbors == 3)
      {
        backbuf[j][i] = 1;
      }
    }
  }
  memcpy(matrix, backbuf, 8 * 8 * sizeof(bool));
}

void button_ISR()
{
  doRandomize = true;
}

void setup()
{
  randomSeed(analogRead(0));
  lc.shutdown(0, false);
  lc.setIntensity(0, 0);
  lc.clearDisplay(0);

#if RANDOMIZE
  randomize();
#endif

  memcpy(backbuf, matrix, 8 * 8 * sizeof(bool));

  attachInterrupt(digitalPinToInterrupt(2), button_ISR, RISING);
}

void loop()
{
  if (doRandomize)
  {
    doRandomize = false;
    randomize();
  }
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      lc.setLed(0, i, j, get(i, j));
    }
  }
  delay(DELAY);
  GOL();
}
