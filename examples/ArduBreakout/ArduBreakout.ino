 /*
  Breakout
 Copyright (C) 2011 Sebastian Goscik
 All rights reserved.

 Modifications by Scott Allen 2016 (after previous changes by ???)

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 */

#include <MicroGamer.h>
#include <MicroGamerMemoryCard.h>
#include <MicroGamerTones.h>

// block in EEPROM to save high scores
#define EE_FILE 2

MicroGamer mg;
MicroGamerMemoryCard mem(64/4);
MicroGamerTones audio(mg.audio.enabled);

const unsigned int COLUMNS = 13; //Columns of bricks
const unsigned int ROWS = 4;     //Rows of bricks
int dx = -1;        //Initial movement of ball
int dy = -1;        //Initial movement of ball
int xb;           //Balls starting possition
int yb;           //Balls starting possition
boolean released;     //If the ball has been released by the player
boolean paused = false;   //If the game has been paused
byte xPaddle;       //X position of paddle
boolean isHit[ROWS][COLUMNS];   //Array of if bricks are hit or not
boolean bounced=false;  //Used to fix double bounce glitch
byte lives = 3;       //Amount of lives
byte level = 1;       //Current level
unsigned int score=0;   //Score for the game
unsigned int brickCount;  //Amount of bricks hit
boolean pad, pad2, pad3;  //Button press buffer used to stop pause repeating
boolean oldpad, oldpad2, oldpad3;
char text_buffer[16];      //General string buffer
boolean start=false;    //If in menu or in game
boolean initialDraw=false;//If the inital draw has happened
char initials[3];     //Initials used in high score

//Ball Bounds used in collision detection
byte leftBall;
byte rightBall;
byte topBall;
byte bottomBall;

//Brick Bounds used in collision detection
byte leftBrick;
byte rightBrick;
byte topBrick;
byte bottomBrick;

byte tick;

void setup()
{
  mg.begin();
  mg.setFrameRate(40);
  mg.initRandomSeed();
}

void loop()
{
  // pause render until it's time for the next frame
  if (!(mg.nextFrame()))
    return;

  //Title screen loop switches from title screen
  //and high scores until FIRE is pressed
  while (!start)
  {
    start = titleScreen();
    if (!start)
    {
      start = displayHighScores(EE_FILE);
    }
  }

  //Initial level draw
  if (!initialDraw)
  {
    //Clears the screen
    mg.clear();
    //Selects Font
    //Draws the new level
    level = 1;
    newLevel();
    score = 0;
    initialDraw=true;
  }

  if (lives>0)
  {
    drawPaddle();

    //Pause game if FIRE pressed
    pad = mg.pressed(Y_BUTTON) || mg.pressed(X_BUTTON);

    if(pad == true && oldpad == false && released)
    {
      oldpad2 = false; //Forces pad loop 2 to run once
      pause();
    }

    oldpad = pad;
    drawBall();

    if(brickCount == ROWS * COLUMNS)
    {
      level++;
      newLevel();
    }
  }
  else
  {
    drawGameOver();
    if (score > 0)
    {
      enterHighScore(EE_FILE);
    }

    mg.clear();
    initialDraw=false;
    start=false;
    lives=3;
    newLevel();
  }

  mg.display();
}

void movePaddle()
{
  //Move right
  if(xPaddle < WIDTH - 12)
  {
    if (mg.pressed(RIGHT_BUTTON))
    {
      xPaddle+=2;
    }
  }

  //Move left
  if(xPaddle > 0)
  {
    if (mg.pressed(LEFT_BUTTON))
    {
      xPaddle-=2;
    }
  }
}

void moveBall()
{
  tick++;
  if(released)
  {
    //Move ball
    if (abs(dx)==2) {
      xb += dx/2;
      // 2x speed is really 1.5 speed
      if (tick%2==0)
        xb += dx/2;
    } else {
      xb += dx;
    }
    yb=yb + dy;

    //Set bounds
    leftBall = xb;
    rightBall = xb + 2;
    topBall = yb;
    bottomBall = yb + 2;

    //Bounce off top edge
    if (yb <= 0)
    {
      yb = 2;
      dy = -dy;
      playTone(523, 250);
    }

    //Lose a life if bottom edge hit
    if (yb >= 64)
    {
      mg.drawRect(xPaddle, 63, 11, 1, 0);
      xPaddle = 54;
      yb=60;
      released = false;
      lives--;
      playTone(175, 250);
      if (random(0, 2) == 0)
      {
        dx = 1;
      }
      else
      {
        dx = -1;
      }
    }

    //Bounce off left side
    if (xb <= 0)
    {
      xb = 2;
      dx = -dx;
      playTone(523, 250);
    }

    //Bounce off right side
    if (xb >= WIDTH - 2)
    {
      xb = WIDTH - 4;
      dx = -dx;
      playTone(523, 250);
    }

    //Bounce off paddle
    if (xb+1>=xPaddle && xb<=xPaddle+12 && yb+2>=63 && yb<=64)
    {
      dy = -dy;
      dx = ((xb-(xPaddle+6))/3); //Applies spin on the ball
      // prevent straight bounce
      if (dx == 0) {
        dx = (random(0,2) == 1) ? 1 : -1;
      }
      playTone(200, 250);
    }

    //Bounce off Bricks
    for (byte row = 0; row < ROWS; row++)
    {
      for (byte column = 0; column < COLUMNS; column++)
      {
        if (!isHit[row][column])
        {
          //Sets Brick bounds
          leftBrick = 10 * column;
          rightBrick = 10 * column + 10;
          topBrick = 6 * row + 1;
          bottomBrick = 6 * row + 7;

          //If A collison has occured
          if (topBall <= bottomBrick && bottomBall >= topBrick &&
              leftBall <= rightBrick && rightBall >= leftBrick)
          {
            Score();
            brickCount++;
            isHit[row][column] = true;
            mg.drawRect(10*column, 2+6*row, 8, 4, 0);

            //Vertical collision
            if (bottomBall > bottomBrick || topBall < topBrick)
            {
              //Only bounce once each ball move
              if(!bounced)
              {
                dy =- dy;
                yb += dy;
                bounced = true;
                playTone(261, 250);
              }
            }

            //Hoizontal collision
            if (leftBall < leftBrick || rightBall > rightBrick)
            {
              //Only bounce once brick each ball move
              if(!bounced)
              {
                dx =- dx;
                xb += dx;
                bounced = true;
                playTone(261, 250);
              }
            }
          }
        }
      }
    }
    //Reset Bounce
    bounced = false;
  }
  else
  {
    //Ball follows paddle
    xb=xPaddle + 5;

    //Release ball if FIRE pressed
    pad3 = mg.pressed(Y_BUTTON) || mg.pressed(X_BUTTON);
    if (pad3 == true && oldpad3 == false)
    {
      released = true;

      //Apply random direction to ball on release
      if (random(0, 2) == 0)
      {
        dx = 1;
      }
      else
      {
        dx = -1;
      }
      //Makes sure the ball heads upwards
      dy = -1;
    }
    oldpad3 = pad3;
  }
}

void drawBall()
{
  // mg.setCursor(0,0);
  // mg.print(mg.cpuLoad());
  // mg.print("  ");
  mg.drawPixel(xb,   yb,   0);
  mg.drawPixel(xb+1, yb,   0);
  mg.drawPixel(xb,   yb+1, 0);
  mg.drawPixel(xb+1, yb+1, 0);

  moveBall();

  mg.drawPixel(xb,   yb,   1);
  mg.drawPixel(xb+1, yb,   1);
  mg.drawPixel(xb,   yb+1, 1);
  mg.drawPixel(xb+1, yb+1, 1);
}

void drawPaddle()
{
  mg.drawRect(xPaddle, 63, 11, 1, 0);
  movePaddle();
  mg.drawRect(xPaddle, 63, 11, 1, 1);
}

void drawGameOver()
{
  mg.drawPixel(xb,   yb,   0);
  mg.drawPixel(xb+1, yb,   0);
  mg.drawPixel(xb,   yb+1, 0);
  mg.drawPixel(xb+1, yb+1, 0);
  mg.setCursor(37, 42);
  mg.print("Game Over");
  mg.setCursor(31, 56);
  mg.print("Score: ");
  mg.print(score);
  mg.display();
  delay(4000);
}

void pause()
{
  paused = true;
  //Draw pause to the screen
  mg.setCursor(52, 45);
  mg.print("PAUSE");
  mg.display();
  while (paused)
  {
    delay(150);
    //Unpause if FIRE is pressed
    pad2 = mg.pressed(Y_BUTTON) || mg.pressed(X_BUTTON);
    if (pad2 == true && oldpad2 == false && released)
    {
        mg.fillRect(52, 45, 30, 11, 0);

        paused=false;
    }
    oldpad2 = pad2;
  }
}

void Score()
{
  score += (level*10);
}

void newLevel(){
  //Undraw paddle
  mg.drawRect(xPaddle, 63, 11, 1, 0);

  //Undraw ball
  mg.drawPixel(xb,   yb,   0);
  mg.drawPixel(xb+1, yb,   0);
  mg.drawPixel(xb,   yb+1, 0);
  mg.drawPixel(xb+1, yb+1, 0);

  //Alter various variables to reset the game
  xPaddle = 54;
  yb = 60;
  brickCount = 0;
  released = false;

  //Draws new bricks and resets their values
  for (byte row = 0; row < 4; row++) {
    for (byte column = 0; column < 13; column++)
    {
      isHit[row][column] = false;
      mg.drawRect(10*column, 2+6*row, 8, 4, 1);
    }
  }

  mg.display();
}

//Used to delay images while reading button input
boolean pollFireButton(int n)
{
  for(int i = 0; i < n; i++)
  {
    delay(15);
    pad = mg.pressed(Y_BUTTON) || mg.pressed(X_BUTTON);
    if(pad == true && oldpad == false)
    {
      oldpad3 = true; //Forces pad loop 3 to run once
      return true;
    }
    oldpad = pad;
  }
  return false;
}

//Function by nootropic design to display highscores
boolean displayHighScores(byte file)
{
  byte y = 8;
  byte x = 24;
  // Each block of EEPROM has 7 high scores, and each high score entry
  // is 5 bytes long:  3 bytes for initials and two bytes for score.
  int address = file * 7 * 5 + EEPROM_STORAGE_SPACE_START;
  byte hi, lo;
  mg.clear();
  mg.setCursor(32, 0);
  mg.print("HIGH SCORES");
  mg.display();

  mem.load();

  for(int i = 0; i < 7; i++)
  {
    sprintf(text_buffer, "%2d", i+1);
    mg.setCursor(x,y+(i*8));
    mg.print(text_buffer);
    mg.display();
    hi = mem.read(address + (5 * i));
    lo = mem.read(address + (5 * i) + 1);

    if ((hi == 0xFF) && (lo == 0xFF))
    {
      score = 0;
    }
    else
    {
      score = (hi << 8) | lo;
    }

    initials[0] = (char)mem.read(address + (5 * i) + 2);
    initials[1] = (char)mem.read(address + (5 * i) + 3);
    initials[2] = (char)mem.read(address + (5 * i) + 4);

    if (score > 0)
    {
      sprintf(text_buffer, "%c%c%c %u", initials[0], initials[1], initials[2], score);
      mg.setCursor(x + 24, y + (i*8));
      mg.print(text_buffer);
      mg.display();
    }
  }
  if (pollFireButton(300))
  {
    return true;
  }
  return false;
  mg.display();
}

boolean titleScreen()
{
  //Clears the screen
  mg.clear();
  mg.setCursor(16,22);
  mg.setTextSize(2);
  mg.print("BREAKOUT");
  mg.setTextSize(1);
  mg.display();
  if (pollFireButton(25))
  {
    return true;
  }

  //Flash "Press FIRE" 5 times
  for(byte i = 0; i < 5; i++)
  {
    //Draws "Press FIRE"
    mg.setCursor(31, 53);
    mg.print("PRESS FIRE!");
    mg.display();

    if (pollFireButton(50))
    {
      return true;
    }

    //Removes "Press FIRE"
    mg.setCursor(31, 53);
    mg.print("           ");
    mg.display();

    if (pollFireButton(25))
    {
      return true;
    }
  }

  return false;
}

//Function by nootropic design to add high scores
void enterInitials()
{
  byte index = 0;

  mg.clear();

  initials[0] = ' ';
  initials[1] = ' ';
  initials[2] = ' ';

  while (true)
  {
    mg.display();
    mg.clear();

    mg.setCursor(16,0);
    mg.print("HIGH SCORE");
    sprintf(text_buffer, "%u", score);
    mg.setCursor(88, 0);
    mg.print(text_buffer);
    mg.setCursor(56, 20);
    mg.print(initials[0]);
    mg.setCursor(64, 20);
    mg.print(initials[1]);
    mg.setCursor(72, 20);
    mg.print(initials[2]);
    for(byte i = 0; i < 3; i++)
    {
      mg.drawLine(56 + (i*8), 27, 56 + (i*8) + 6, 27, 1);
    }
    mg.drawLine(56, 28, 88, 28, 0);
    mg.drawLine(56 + (index*8), 28, 56 + (index*8) + 6, 28, 1);
    delay(150);

    if (mg.pressed(LEFT_BUTTON) || mg.pressed(X_BUTTON))
    {
      if (index > 0)
      {
        index--;
        playTone(1046, 250);
      }
    }

    if (mg.pressed(RIGHT_BUTTON))
    {
      if (index < 2)
      {
        index++;
        playTone(1046, 250);
      }
    }

    if (mg.pressed(DOWN_BUTTON))
    {
      initials[index]++;
      playTone(523, 250);
      // A-Z 0-9 :-? !-/ ' '
      if (initials[index] == '0')
      {
        initials[index] = ' ';
      }
      if (initials[index] == '!')
      {
        initials[index] = 'A';
      }
      if (initials[index] == '[')
      {
        initials[index] = '0';
      }
      if (initials[index] == '@')
      {
        initials[index] = '!';
      }
    }

    if (mg.pressed(UP_BUTTON))
    {
      initials[index]--;
      playTone(523, 250);
      if (initials[index] == ' ') {
        initials[index] = '?';
      }
      if (initials[index] == '/') {
        initials[index] = 'Z';
      }
      if (initials[index] == 31) {
        initials[index] = '/';
      }
      if (initials[index] == '@') {
        initials[index] = ' ';
      }
    }

    if (mg.pressed(Y_BUTTON))
    {
      if (index < 2)
      {
        index++;
        playTone(1046, 250);
      } else {
        playTone(1046, 250);
        return;
      }
    }
  }

}

void enterHighScore(byte file)
{
  // Each block of EEPROM has 7 high scores, and each high score entry
  // is 5 bytes long:  3 bytes for initials and two bytes for score.
  int address = file * 7 * 5 + EEPROM_STORAGE_SPACE_START;
  byte hi, lo;
  char tmpInitials[3];
  unsigned int tmpScore = 0;

  mem.load();

  // High score processing
  for(byte i = 0; i < 7; i++)
  {
    hi = mem.read(address + (5 * i));
    lo = mem.read(address + (5 * i) + 1);
    if ((hi == 0xFF) && (lo == 0xFF))
    {
      // The values are uninitialized, so treat this entry
      // as a score of 0.
      tmpScore = 0;
    } else
    {
      tmpScore = (hi << 8) | lo;
    }
    if (score > tmpScore)
    {
      enterInitials();
      for(byte j = i; j < 7; j++)
      {
        hi = mem.read(address + (5 * j));
        lo = mem.read(address + (5 * j) + 1);

        if ((hi == 0xFF) && (lo == 0xFF))
        {
        tmpScore = 0;
        }
        else
        {
          tmpScore = (hi << 8) | lo;
        }

        tmpInitials[0] = (char)mem.read(address + (5 * j) + 2);
        tmpInitials[1] = (char)mem.read(address + (5 * j) + 3);
        tmpInitials[2] = (char)mem.read(address + (5 * j) + 4);

        // write score and initials to current slot
        mem.write(address + (5 * j), ((score >> 8) & 0xFF));
        mem.write(address + (5 * j) + 1, (score & 0xFF));
        mem.write(address + (5 * j) + 2, initials[0]);
        mem.write(address + (5 * j) + 3, initials[1]);
        mem.write(address + (5 * j) + 4, initials[2]);

        // tmpScore and tmpInitials now hold what we want to
        //write in the next slot.
        score = tmpScore;
        initials[0] = tmpInitials[0];
        initials[1] = tmpInitials[1];
        initials[2] = tmpInitials[2];
      }

      score = 0;
      initials[0] = ' ';
      initials[1] = ' ';
      initials[2] = ' ';
      mem.save();

      return;
    }
  }
}

// Wrap the Arduino tone() function so that the pin doesn't have to be
// specified each time. Also, don't play if audio is set to off.
void playTone(unsigned int frequency, unsigned long duration)
{
  if (mg.audio.enabled() == true)
  {
    audio.tone( frequency, duration);
  }
}
