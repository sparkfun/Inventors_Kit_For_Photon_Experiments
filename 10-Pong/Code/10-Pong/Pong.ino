/*  SparkFun Inventor's Kit for Photon
        Experiment 10 - Part 1
        This sketch was written by SparkFun Electronics
      Ben Leduc-Mills
      August 31, 2015
      https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

      This is an example sketch for an analog sensor Pong game.

      Development environment specifics:
      Particle Build environment (https://www.particle.io/build)
      Particle Photon RedBoard
      Released under the MIT License(http://opensource.org/licenses/MIT)
    */
//////////////////////////
// MicroOLED Definition //
//////////////////////////

#define PIN_RESET D6  // Connect RST to pin 6
#define PIN_DC    D5  // Connect DC to pin 5 (required for SPI)
#define PIN_CS    A2 // Connect CS to pin A2 (required for SPI)
//MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);
MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);

//#define SINGLE_PLAYER

const int player1Pin = A1;
#ifndef SINGLE_PLAYER
const int player2Pin = A0;
#endif

/*** Sensor Calibration ****/

int sensor1Calibration = LCDHEIGHT; //photocell w/330
int sensor2Calibration = LCDHEIGHT; //potentiometer

//potentiometer
int sensor1Min = 0;
int sensor1Max = 4096;
//photocell
int sensor2Min = 100;
int sensor2Max = 1000;


/*** Game Settings ***/
const int renderDelay = 16;
const int startDelay = 2000;
const int gameOverDelay = 3000;
const int scoreToWin = 10;

int player1Score = 0;
int player2Score = 0;

const float paddleWidth = LCDWIDTH / 16.0;
const float paddleHeight = LCDHEIGHT / 3.0;
const float halfPaddleWidth = paddleWidth / 2.0;
const float halfPaddleHeight = paddleHeight / 2.0;

float player1PosX = 1.0 + halfPaddleWidth;
float player1PosY = 0.0;
float player2PosX = LCDWIDTH - 1.0 - halfPaddleWidth;
float player2PosY = 0.0;

// This is only used in SINGLE_PLAYER mode:
#ifdef SINGLE_PLAYER
float enemyVelY = 0.5;
#endif


/*** Ball Physics ***/
const float ballRadius = 2.0;
const float ballSpeedX = 1.0;
float ballPosX = LCDWIDTH / 2.0;
float ballPosY = LCDHEIGHT / 2.0;
float ballVelX = -1.0 * ballSpeedX;
float ballVelY = 0;

void setup()
{
  // in our setup, we call a few custom functions to get everything ready
  initializeGraphics();
  initializeInput();
  displayGameStart();
  Serial.begin(9600);
}


void loop()
{
  updateGame(); //custom function to advance the game logic
  renderGame(); //custom function to dislay the current game state on the OLED screen
  // print out sensor values via serial so we can calibrate
  Serial.print(analogRead(A0));
  Serial.print(" <-A0 | A1-> ");
  Serial.println(analogRead(A1));

  //winning conditions
  if (player1Score >= scoreToWin)
  {
    gameOver(true);
  }
  else if (player2Score >= scoreToWin)
  {
    gameOver(false);
  }
}

//start the OLED and set the font type we want
void initializeGraphics()
{
  oled.begin();
  oled.setFontType(1);
}

//get either 1 or 2 pins ready for analog input
void initializeInput()
{
  pinMode(player1Pin, INPUT);
#ifndef SINGLE_PLAYER
  pinMode(player2Pin, INPUT);
#endif
}

//display the start screen
void displayGameStart()
{
  oled.clear(PAGE);
  renderString(20, 10, "Get");
  renderString(10, 30, "Ready!");
  oled.display();
  delay(startDelay);
}

//update the positions of the player paddles and the ball
void updateGame()
{
  updatePlayer1();
  updatePlayer2();
  updateBall();
}

//reset the score, ball position, and paddle positions for a new game
void resetGame()
{
  player2Score = 0;
  player1Score = 0;
  player2PosY = 0.0;
  ballPosX = LCDWIDTH / 2.0;
  ballPosY = LCDHEIGHT / 2.0;
  ballVelX = -1.0 * ballSpeedX;
  ballVelY = 0.0;
}


float clampPaddlePosY(float paddlePosY)
{
  float newPaddlePosY = paddlePosY;

  if (paddlePosY - halfPaddleHeight < 0)
  {
    newPaddlePosY = halfPaddleHeight;
  }
  else if (paddlePosY + halfPaddleHeight > LCDHEIGHT)
  {
    newPaddlePosY = LCDHEIGHT - halfPaddleHeight;
  }

  return newPaddlePosY;
}

void updatePlayer1()
{
  int potVal = analogRead(player1Pin);
  player1PosY = map(potVal, sensor1Min, sensor1Max, 0, sensor1Calibration);
}

void updatePlayer2()
{
  // If it's a single player game, update the AI's position
#ifdef SINGLE_PLAYER
  // Follow the ball at a set speed
  if (player2PosY < ballPosY)
  {
    player2PosY += enemyVelY;
  }
  else if (player2PosY > ballPosY)
  {
    player2PosY -= enemyVelY;
  }

  player2PosY = clampPaddlePosY(player2PosY);
#else  // Else if this is multiplayer, get player 2's position
  int lightVal = analogRead(player2Pin);
  player2PosY = map(lightVal, sensor2Min, sensor2Max, 0, sensor2Calibration);
  //Serial.println(player2PosY);
#endif
}

void updateBall()
{
  ballPosY += ballVelY;
  ballPosX += ballVelX;

  // Top and bottom wall collisions
  if (ballPosY < ballRadius)
  {
    ballPosY = ballRadius;
    ballVelY *= -1.0;
  }
  else if (ballPosY > LCDHEIGHT - ballRadius)
  {
    ballPosY = LCDHEIGHT - ballRadius;
    ballVelY *= -1.0;
  }

  // Left and right wall collisions
  if (ballPosX < ballRadius)
  {
    ballPosX = ballRadius;
    ballVelX = ballSpeedX;
    player2Score++;
  }
  else if (ballPosX > LCDWIDTH - ballRadius)
  {
    ballPosX = LCDWIDTH - ballRadius;
    ballVelX *= -1.0 * ballSpeedX;
    player1Score++;
  }

  // Paddle collisions
  if (ballPosX < player1PosX + ballRadius + halfPaddleWidth)
  {
    if (ballPosY > player1PosY - halfPaddleHeight - ballRadius &&
        ballPosY < player1PosY + halfPaddleHeight + ballRadius)
    {
      ballVelX = ballSpeedX;
      ballVelY = 2.0 * (ballPosY - player1PosY) / halfPaddleHeight;
    }
  }
  else if (ballPosX > player2PosX - ballRadius - halfPaddleWidth)
  {
    if (ballPosY > player2PosY - halfPaddleHeight - ballRadius &&
        ballPosY < player2PosY + halfPaddleHeight + ballRadius)
    {
      ballVelX = -1.0 * ballSpeedX;
      ballVelY = 2.0 * (ballPosY - player2PosY) / halfPaddleHeight;
    }
  }
}


void renderGame()
{
  oled.clear(PAGE);

  renderScores(player1Score, player2Score);
  renderPaddle(player1PosX, player1PosY);
  renderPaddle(player2PosX, player2PosY);
  renderBall(ballPosX, ballPosY);

  oled.display();
  delay(renderDelay);
}

void renderString(int x, int y, String string)
{
  oled.setCursor(x, y);
  oled.print(string);
}

void renderPaddle(int x, int y)
{
  oled.rect(
    x - halfPaddleWidth,
    y - halfPaddleHeight,
    paddleWidth,
    paddleHeight);
}

void renderBall(int x, int y)
{
  oled.circle(x, y, 2);
}

void renderScores(int firstScore, int secondScore)
{
  renderString(10, 0, String(firstScore));
  renderString(LCDWIDTH - 14, 0, String(secondScore));
}

void gameOver(bool didWin)
{
  if (didWin)
  {
#ifdef SINGLE_PLAYER
    renderString(20, 10, "You");
    renderString(20, 30, "Win!");
#else
    renderString(0, 10, "Playr 1");
    renderString(15, 30, "Wins");
#endif
  }
  else
  {
#ifdef SINGLE_PLAYER
    renderString(20, 10, "You");
    renderString(15, 30, "Lose!");
#else
    renderString(0, 10, "Playr 2");
    renderString(15, 30, "Wins");
#endif
  }

  oled.display();
  delay(gameOverDelay);

  // Get ready to start the game again.
  resetGame();
  displayGameStart();
}
