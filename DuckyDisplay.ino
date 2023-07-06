#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Intern Editor:
#include <EEPROM.h>

//Power Controll
#include <LowPower.h>

//keyboard
#include "Keyboard_master.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum MenuState {
  MENU_MAIN,
  MENU_VAULT,
  MENU_EDIT_SECRET,
  MENU_EDIT_PASSWORD,
  MENU_EXIT
};

MenuState currentMenu = MENU_MAIN;
int menuOption = 1;

char personalSecret[4] = {'P','I','N','0'};
char pwChar[4] = {'0','0','0','0'}; // inits pin with: 0 0 0 0
int pointer = 0;
int spacer = 0;
int cursorX = 7;
int cursorY = 15;
int textSize = 5;
bool getData = false;
String passPhrase = "myPassword";

void writeStringPass(){
  int strLength = passPhrase.length();
  char thisChar = ' ';
  for( int i = 0; i<strLength; i++){
    thisChar = passPhrase.charAt(i);
    Keyboard.write(thisChar);
  }
}

int checkDigit(int digit){
  if(digit<32){
    return 126;
  }
  else if(digit>126){
    return 32;
  }
  else{
    return digit;
  }
}
void printArray(char thisArray[], int size) {
  for (int i = 0; i < size; i++) {
    display.print(thisArray[i]);
  }
}

bool getPW(String title=""){
  int blinker = 0;
  display.display();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(textSize);
  display.setCursor(cursorX, cursorY);
  printArray(pwChar, 4);
  while (digitalRead(6) == HIGH) {
    
    display.clearDisplay();

    display.setCursor(10,0);
    display.setTextSize(1);
    display.print(title);

    display.setTextSize(textSize);
    display.setCursor(cursorX, cursorY);
    printArray(pwChar, 4);
    if(pointer != 0){
      spacer = textSize;
    }else{
      spacer = 0;
    }
    blinker++;
    if((blinker%2)==0){
      //thiker underline
      display.fillRect((cursorX+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+textSize, 5*textSize, textSize, SSD1306_WHITE);
      //underline
      //display.drawLine((cursorX+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+1, (cursorX+(textSize*5-1)+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+1, SSD1306_WHITE);
      blinker =0;
    }
    display.display();
    
    delay(100);
    if (digitalRead(8)==LOW){
      //if controll button pressed -> pointer +1 -> jumps to next pin digit
      if(pointer<4){
        pointer++;
      }
      if(pointer >= 4){
        pointer = 0;
      }
      while(digitalRead(8)==LOW){}
    }
    else if (digitalRead(9)==LOW){
      //increase pin digit +1
      pwChar[pointer] = checkDigit(1+pwChar[pointer]);
    }
    else if (digitalRead(7)==LOW){
      //decrease pin digit -1
      pwChar[pointer] = checkDigit(pwChar[pointer]-1);
    }    
  }
  display.clearDisplay();
  display.setCursor(cursorX, cursorY);
  display.print("****");
  display.display();
  return true;
}

void resetPW(){
  for(int i=0; i<4;i++){
    pwChar[i] = '0';
  }
  pointer = 0;
}


void showMainMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Menu:");
  display.setCursor(10, 20);
  if (menuOption == 1) {
    display.print("> ");
  }
  display.println("1. Enter Vault");
  display.setCursor(10, 30);
  if (menuOption == 2) {
    display.print("> ");
  }
  display.println("2. Edit Secret");
  display.setCursor(10, 40);
  if (menuOption == 3) {
    display.print("> ");
  }
  display.println("3. Edit Password");
  display.setCursor(10, 50);
  if (menuOption == 4) {
    display.print("> ");
  }
  display.println("4. Shut down");
  display.display();
}

void moveIndicatorUp() {
  menuOption--;
  if (menuOption < 1) {
    menuOption = 2;
  }
}

void moveIndicatorDown() {
  menuOption++;
  if (menuOption > 4) {
    menuOption = 1;
  }
}

void EditSecret(){
  resetPW();
  getData = getPW("Enter PIN:");
  if(getData){
    getData = false;
    if(compareArray()){
      resetPW();
      delay(700);
      // char oldPW[4] = personalSecret;
      int blinker = 0;
      display.display();
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);

      display.setTextSize(textSize);
      display.setCursor(cursorX, cursorY);
      printArray(pwChar, 4);
      while (digitalRead(6) == HIGH) {
        
        display.clearDisplay();

        display.setCursor(10,0);
        display.setTextSize(1);
        display.print("Enter new PIN:");

        display.setTextSize(textSize);
        display.setCursor(cursorX, cursorY);
        printArray(pwChar, 4);
        if(pointer != 0){
          spacer = textSize;
        }else{
          spacer = 0;
        }
        blinker++;
        if((blinker%2)==0){
          //thiker underline
          display.fillRect((cursorX+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+textSize, 5*textSize, textSize, SSD1306_WHITE);
          //underline
          //display.drawLine((cursorX+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+1, (cursorX+(textSize*5-1)+(pointer*(textSize*5))+(pointer*spacer)), cursorY+(textSize*7)+1, SSD1306_WHITE);
          blinker =0;
        }
        display.display();
        
        delay(100);
        if (digitalRead(8)==LOW){
          //if controll button pressed -> pointer +1 -> jumps to next pin digit
          if(pointer<4){
            pointer++;
          }
          if(pointer >= 4){
            pointer = 0;
          }
          while(digitalRead(8)==LOW){}
        }
        else if (digitalRead(9)==LOW){
          //increase pin digit +1
          pwChar[pointer] = checkDigit(1+pwChar[pointer]);
        }
        else if (digitalRead(7)==LOW){
          //decrease pin digit -1
          pwChar[pointer] = checkDigit(pwChar[pointer]-1);
        }    
      }
      
      //Serial.println("Password Check");
      //Serial.print("Secret: ");
      //Serial.println(personalSecret);
      //Serial.print("PIN given: ");
      //Serial.println(pwChar);

      EEPROM.begin();
      for(int i=0; i<sizeof(personalSecret); i++){
        EEPROM.get(0, personalSecret[i-1]);
        EEPROM.put(0, pwChar[i]);
      }
      //EEPROM.commit();
      EEPROM.end();
      
      display.clearDisplay();
      display.setCursor(10,0);
      display.setTextSize(1);
      display.print("New PIN saved.");
      display.setTextSize(textSize);
      display.setCursor(cursorX, cursorY);
      display.print("****");
      display.display();

      //Serial.println("Password changed");
      //Serial.print("Secret: ");
      //Serial.println(personalSecret);
      //Serial.print("PIN given: ");
      //Serial.println(pwChar);
      //Serial.println();
      delay(1500);
    }
  }else{
    display.clearDisplay();
    display.setCursor(10,0);
    display.setTextSize(1);
    display.print("Wrong PIN.");
    display.setTextSize(textSize);
    display.setCursor(cursorX, cursorY);
    display.print("****");
    display.display();


    //Serial.println("Password Wrong");
    //Serial.print("Secret: ");
    //Serial.println(personalSecret);
    //Serial.print("PIN given: ");
    //Serial.println(pwChar);
    delay(2000);
  }
  resetPW();
}

void setup() {
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  Serial.begin(9600);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;) {}
  }
  display.clearDisplay();
  display.display();
}

bool compareArray(){
  for (int i = 0;  i < 4; i++){
    if( pwChar[i] != personalSecret[i]){
      return false;
    }
  }
  return true;
  
}

void EditPassphrase() {
  resetPW();
  char passArr[18]; 
  for (int i = 0; i < sizeof(passArr); i++) {
    int intChar = passPhrase.charAt(i);
    if(intChar > 126 || intChar < 32){
      intChar = 32;
    }
    passArr[i] = intChar;
  }
  for(int i=0; i<sizeof(passArr); i++){
    //Serial.print((int)passArr[i]);
    //Serial.print(" - ");
    passArr[i] = checkDigit(passArr[i]);
    //Serial.print(i);
    //Serial.print(" - ");
    //Serial.print(passArr[i]);
    //Serial.print(" - ");
    //Serial.print((int)passArr[i]);
    //Serial.println();
  }
  //Serial.println();
  bool getData = getPW("Enter PIN:");
  if (getData) {
    getData = false;
    if (compareArray()) {
      resetPW();
      delay(700);
      int blinker = 0;
      display.display();
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);

      display.setTextSize(textSize);
      display.setCursor(cursorX, cursorY);
      printArray(pwChar, 4);
      while (digitalRead(6) == HIGH) {

        display.clearDisplay();

        display.setCursor(10, 0);
        display.setTextSize(1);
        display.print("Enter new Password:");

        display.setCursor(10, 20);
        
        printArray(passArr, 18);
        if (pointer != 0) {
          spacer = 1;
        } else {
          spacer = 0;
        }
        blinker++;
        if ((blinker % 2) == 0) {
          //thiker underline
          display.fillRect((10 + (pointer * (1 * 5)) + (pointer * spacer)), 20 + (1 * 7) + 1, 5 * 1, 1, SSD1306_WHITE);
          blinker = 0;
        }
        display.display();

        delay(100);
        unsigned int digit = 0;
        if (digitalRead(8) == LOW) {
          //if controll button pressed -> pointer +1 -> jumps to next pin digit
          if (pointer < sizeof(passArr)) {
            pointer++;
          }
          if (pointer >= sizeof(passArr)) {
            pointer = 0;
          }
          while (digitalRead(8) == LOW) {}
        } else if (digitalRead(9) == LOW) {
          //increase passArr digit +1
          digit = checkDigit(1 + passArr[pointer]);
          passArr[pointer] = checkDigit(digit);
        } else if (digitalRead(7) == LOW) {
          //decrease passArr digit -1
          digit = checkDigit(passArr[pointer] - 1);
          passArr[pointer] = checkDigit(digit);
        }
      }

      String passStr(passArr);
      EEPROM.begin();
      for(int i=0; i<sizeof(passPhrase); i++){
        EEPROM.get(0, passPhrase[i-1]);
        EEPROM.put(0, passStr[i]);
      }
      //EEPROM.commit();
      EEPROM.end();


      display.clearDisplay();
      display.setCursor(5, 0);
      display.setTextSize(1);
      display.print("New Password saved.");  
      display.setTextSize(textSize);
      display.setCursor(cursorX, cursorY);
      display.print("****");
      display.display();

      //Serial.println("Passphrase changed");   
      //Serial.print("Passphrase given: ");
      //Serial.println(passPhrase);
      //Serial.println();
      delay(1500);
    }
  } else {
    display.clearDisplay();
    display.setCursor(10, 0);
    display.setTextSize(1);
    display.print("Wrong PIN.");
    display.setTextSize(textSize);
    display.setCursor(cursorX, cursorY);
    display.print("****");
    display.display();

    //Serial.println("Password Wrong");
    //Serial.print("Secret: ");
    //Serial.println(personalSecret);
    //Serial.print("PIN given: ");
    //Serial.println(pwChar);
    delay(2000);
  }
  resetPW();
}

void loop() {
  switch (currentMenu) {
    case MENU_MAIN:
      showMainMenu();
      while (currentMenu == MENU_MAIN) {
        if (digitalRead(8) == LOW) {
          if (menuOption == 1) {
            display.clearDisplay();
            display.display();
            delay(500);
            currentMenu = MENU_VAULT;
          } else if (menuOption == 2) {
            currentMenu = MENU_EDIT_SECRET;
          }
          else if (menuOption == 3) {
            currentMenu = MENU_EDIT_PASSWORD;
          }
          else if (menuOption == 4) {
            currentMenu = MENU_EXIT;
          }
        } else if (digitalRead(9) == LOW) {
          delay(100);
          if (digitalRead(9) == LOW) {
            moveIndicatorUp();
            showMainMenu();
          }
        } else if (digitalRead(7) == LOW) {
          delay(100);
          if (digitalRead(7) == LOW) {
            moveIndicatorDown();
            showMainMenu();
          }
        }
      }
      break;

    case MENU_VAULT:
      // Code for the vault menu
      getData = getPW("Enter PIN:");
      if(getData){
        getData = false;
        if(compareArray()){
          writeStringPass();
        }
        resetPW();
      }
      delay(500);
      currentMenu = MENU_MAIN;  // Go back to the main menu after entering the vault
      break;

    case MENU_EDIT_SECRET:
      currentMenu = MENU_EDIT_SECRET;
      EditSecret();
      currentMenu = MENU_MAIN;
      break;

    case MENU_EDIT_PASSWORD:
      currentMenu = MENU_EDIT_SECRET;
      EditPassphrase();
      currentMenu = MENU_MAIN;
      break;

    case MENU_EXIT:
      currentMenu = MENU_EDIT_SECRET;
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("Ready to unplug");
      display.display();
      delay(5000);
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
      while(true){}
  }
}
