// hello
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

Adafruit_LiquidCrystal lcd(0);   

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


const int buzzerPin = 10;


String correctPass = "9276";
unsigned int alarmFreq = 500;

String inputPass;

// Modes
enum Mode {
  MODE_NORMAL,
  MODE_CHANGE_PASS_OLD,
  MODE_CHANGE_PASS_NEW,
  MODE_CHANGE_FREQ
};

Mode mode = MODE_NORMAL;
bool isRinging = true;

// Functions 
void startAlarm() {
  tone(buzzerPin, alarmFreq);
  isRinging = true;
  mode = MODE_NORMAL;
  inputPass = "";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter password:");
  lcd.setCursor(0, 1);
}

void stopAlarm() {
  noTone(buzzerPin);
  isRinging = false;
  mode = MODE_NORMAL;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm OFF");
}

void wrongPassword() {
  noTone(buzzerPin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong password");
  tone(buzzerPin, alarmFreq, 500);
  delay(500);
  startAlarm();
}

void askOldPassword() {
  inputPass = "";
  mode = MODE_CHANGE_PASS_OLD;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Old password:");
  lcd.setCursor(0, 1);
}

void askNewPassword() {
  inputPass = "";
  mode = MODE_CHANGE_PASS_NEW;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New password:");
  lcd.setCursor(0, 1);
}

void askNewFreq() {
  inputPass = "";
  mode = MODE_CHANGE_FREQ;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Freq Hz:");
  lcd.setCursor(0, 1);
}

// Check password, when buzzer is ON
void handlePasswordChar(char key) {
  if (key == '#') {
    if (inputPass == correctPass) {
      stopAlarm();
    } else {
      wrongPassword();
    }
  } else if (key == '*') {
    inputPass = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  } else {
    if (inputPass.length() < correctPass.length()) {
      inputPass += key;
      lcd.print('*');
    }
  }
}

// Check old password
void handleChangePassOld(char key) {
  if (key == '#') {
    if (inputPass == correctPass) {
      askNewPassword();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong old pass");
      delay(800);
      stopAlarm();
    }
  } else if (key == '*') {
    inputPass = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  } else {
    inputPass += key;
    lcd.print('*');
  }
}

// Enter new password
void handleChangePassNew(char key) {
  if (key == '#') {
    if (inputPass.length() > 0) {
      correctPass = inputPass;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pass changed");
      delay(800);
    }
    stopAlarm();
  } else if (key == '*') {
    inputPass = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  } else {
    inputPass += key;
    lcd.print('*');
  }
}

// Change frequency
void handleChangeFreq(char key) {
  if (key == '#') {
    if (inputPass.length() > 0) {
      alarmFreq = inputPass.toInt();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Freq: ");
      lcd.print(alarmFreq);
      tone(buzzerPin, alarmFreq, 500);
      delay(800);
    }
    stopAlarm();
  } else if (key == '*') {
    inputPass = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  } else if (key >= '0' && key <= '9') {
    inputPass += key;
    lcd.print(key);
  }
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16, 2);
  startAlarm();
}

void loop() {
  char key = keypad.getKey();
  if (key == NO_KEY) return;

  // When buzzer is ON - only password
  if (isRinging && mode == MODE_NORMAL) {
    handlePasswordChar(key);
    return;
  }

  // Global B / C
  if (mode == MODE_NORMAL) {
    if (key == 'B') {
      askOldPassword();
      return;
    }
    if (key == 'C') {
      askNewFreq();
      return;
    }
    if (key == 'D') {
		
    }
  }

  // just switch
  switch (mode) {
    case MODE_CHANGE_PASS_OLD:
      handleChangePassOld(key);
      break;
    case MODE_CHANGE_PASS_NEW:
      handleChangePassNew(key);
      break;
    case MODE_CHANGE_FREQ:
      handleChangeFreq(key);
      break;
    case MODE_NORMAL:
      // Any other - just ignore
      break;
  }
}
