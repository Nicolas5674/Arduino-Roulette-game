#include <LiquidCrystal.h>

const int rs = 13, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte data[3] = {0, 0, 0}; // Declare the data array at the top

// Function prototypes
void determineFinalColor(int index);
void shiftWrite(int desiredPin, boolean desiredState);
//pins definition
const int dataPin = 3;
const int latchPin = 5;
const int clockPin = 4;
const int buzzerPin = 6;
const int joystickButton  = A0;
const int joystickHorz = A1;
const int joystickVert = A2;

const int numRegisters = 3;
const int numLEDs = numRegisters * 8;
//variables to track the color information
enum LEDColor { RED, GREEN, BLUE, YELLOW, NONE };
//variables to track the status of system
enum GameState {
  INIT,
  SELECT_COLOR,
  SELECT_BET,
  LED_MOVEMENT,
  CALCULATE_RESULT
};
//variables to track color at different states
LEDColor betColor = RED;
LEDColor finalColor = NONE;
GameState currentState = INIT;

LEDColor selectedColor = NONE;
int currentBet = 10;

int balance = 100;//initial balance
int betAmount = 10;//bet amount intial

void setup() {
  //setting pins as output and input
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(joystickButton , INPUT_PULLUP);
//initialize LCD
  lcd.begin(16, 2);
  //show initial balance
  lcd.print("Balance: $");
  lcd.print(balance);
  //initiate random number to avoid same result everytime
  randomSeed(analogRead(0)); // Seed the random number generator
}

void loop() {
//cases to perform tasks at every state
switch (currentState) {
  //case 1 to initiate betting
    case INIT:
      if (digitalRead(joystickButton) == LOW) {
        currentState = SELECT_COLOR;
        lcd.clear();
        lcd.print("Select Color");
        selectedColor = RED;
        lcd.setCursor(0, 1);
        lcd.print("Red     ");
      }
      break;
//task two for selecting color for bet
    case SELECT_COLOR:
      if (analogRead(joystickHorz) > 600) {
        selectedColor = RED;
        lcd.setCursor(0, 1);
        lcd.print("Red     ");
      } else if (analogRead(joystickHorz) < 400) {
        selectedColor = BLUE;
        lcd.setCursor(0, 1);
        lcd.print("Blue    ");
      } else if (analogRead(joystickVert) > 600) {
        selectedColor = GREEN;
        lcd.setCursor(0, 1);
        lcd.print("Green   ");
      } else if (analogRead(joystickVert) < 400) {
        selectedColor = YELLOW;
        lcd.setCursor(0, 1);
        lcd.print("Yellow  ");
      }

      if (digitalRead(joystickButton) == LOW) {
        currentState = SELECT_BET;
        lcd.clear();
        lcd.print("Bet: $");
        lcd.print(currentBet);
      }
      break;
//task three to select betting amount
    case SELECT_BET:
      if (analogRead(joystickVert) > 600 && currentBet < 30) {
        currentBet += 5;
        if(currentBet == 6) currentBet = 5;
        lcd.setCursor(6, 0);
        lcd.print(currentBet);
        if(currentBet<10)lcd.print(" ");
      } else if (analogRead(joystickVert) < 400 && currentBet > 1) {
        currentBet -= 5;
        lcd.setCursor(6, 0);
        if(currentBet == 0) currentBet = 1;
        lcd.print(currentBet);
        if(currentBet<10)lcd.print(" ");
      }

      if (digitalRead(joystickButton) == LOW) {
        currentState = LED_MOVEMENT;
        lcd.clear();
        lcd.print("Spinning...");
      }
      break;
//task four for LED movement
    case LED_MOVEMENT:
      simulateBallMovement();
      currentState = CALCULATE_RESULT;
      break;
// getting results
    case CALCULATE_RESULT:
      if (finalColor == selectedColor) {
        balance += currentBet * 3;
        lcd.clear();
        lcd.print("You Win!");
        lcd.setCursor(0, 1);
        lcd.print("Balance: $");
        lcd.print(balance);
        delay(2000);
        tone(buzzerPin, 1000, 500);  // Add buzzer sound for win
      } else {
        balance -= currentBet;
        lcd.clear();
        lcd.print("You Lose!");
        lcd.setCursor(0, 1);
        lcd.print("Balance: $");
        lcd.print(balance);
        delay(2000);
      }

      if (balance <= 0) {
        lcd.clear();
        lcd.print("Out of Casino!");
        tone(buzzerPin, 300, 500);  // Game over sound
        delay(500);
        tone(buzzerPin, 200, 500);
        while (1);  // End the game
      } else {
        currentState = INIT;  // Reset the game state to SELECT_COLOR
      }
      break;
    
  }
  delay(200);
}
//start gaem function to read button value to start the game
void startGame() {
  lcd.clear();
  lcd.print("Bet on: ");
  while (true) {
    int horzValue = analogRead(joystickHorz);
    if (horzValue < 300) {
      betColor = RED;
      lcd.setCursor(8, 0);
      lcd.print("RED   ");
    } else if (horzValue > 700) {
      betColor = YELLOW;
      lcd.setCursor(8, 0);
      lcd.print("YELLOW");
    } else {
      int vertValue = analogRead(joystickVert);
      if (vertValue < 300) {
        betColor = BLUE;
        lcd.setCursor(8, 0);
        lcd.print("BLUE  ");
      } else if (vertValue > 700) {
        betColor = GREEN;
        lcd.setCursor(8, 0);
        lcd.print("GREEN ");
      }
    }

    if (digitalRead(joystickButton ) == LOW) {
      delay(10); // debounce
      if (digitalRead(joystickButton ) == LOW) {
        break;
      }
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("Bet: $");
  lcd.print(betAmount);

  while (true) {
    int vertValue = analogRead(joystickVert);
    if (vertValue < 300 && betAmount < 30) {
      betAmount += 5;
      lcd.setCursor(6, 1);
      lcd.print(betAmount);
      lcd.print("  ");
      delay(200); // delay for better control
    } else if (vertValue > 700 && betAmount > 1) {
      betAmount -= 5;
      lcd.setCursor(6, 1);
      lcd.print(betAmount);
      lcd.print("  ");
      delay(200); // delay for better control
    }

    if (digitalRead(joystickButton ) == LOW) {
      delay(10); // debounce
      if (digitalRead(joystickButton ) == LOW) {
        break;
      }
    }
  }

  // Start the spin
  simulateBallMovement();

  // Check win or lose
  if (betColor == finalColor) {
    balance += betAmount * 3;
    lcd.clear();
    lcd.print("You Win!");
    lcd.setCursor(0, 1);
    lcd.print("Balance: $");
    lcd.print(balance);
    // Play win sound
    tone(buzzerPin, 1000, 500);
    delay(500);
    tone(buzzerPin, 1500, 500);
  } else {
    balance -= betAmount;
    lcd.clear();
    lcd.print("You Lose!");
    lcd.setCursor(0, 1);
    lcd.print("Balance: $");
    lcd.print(balance);
    // Play lose sound
    tone(buzzerPin, 500, 1000);
  }

  if (balance <= 0) {
    lcd.clear();
    lcd.print("Out of Casino!");
    // Play game over sound
    tone(buzzerPin, 300, 500);
    delay(500);
    tone(buzzerPin, 200, 500);
    while (true); // End the game
  }
}
//show ball movement by displaying LEDs
void simulateBallMovement() {
  int delayTime = 50;
  unsigned long startTime = millis();
  unsigned long randomDuration = random(2000, 15000);

  for (int index = 0; ; index = (index + 1) % 24) {
    shiftWrite(index, HIGH);
    delay(delayTime);
    shiftWrite(index, LOW);

    if (millis() - startTime > randomDuration) {
      delayTime += 5;
      if (delayTime > 500) {
        shiftWrite(index, HIGH);
        determineFinalColor(index);
        break;
      }
    }
  }
}
//shift register IC code
void shiftWrite(int desiredPin, boolean desiredState) {
  int byteNumber = desiredPin / 8;
  int bitNumber = desiredPin % 8;

  if (desiredState) {
    data[byteNumber] |= (1 << bitNumber);
  } else {
    data[byteNumber] &= ~(1 << bitNumber);
  }

  digitalWrite(latchPin, LOW);
  for (int i = 2; i >= 0; i--) {
    shiftOut(dataPin, clockPin, MSBFIRST, data[i]);
  }
  digitalWrite(latchPin, HIGH);
}


void determineFinalColor(int index) {
  if (index >= 0 && index <= 4)
    finalColor = YELLOW;
  else if (index >= 5 && index <= 9)
    finalColor = RED;
  else if (index >= 10 && index <= 14)
    finalColor = BLUE;
  else if (index >= 15 && index <= 19)
    finalColor = GREEN;
  else
    finalColor = NONE;
}
