#include "OneButton.h"
OneButton powerbtn(22, true);
OneButton touchbtn(23, true);

#include <SPI.h>

#define _0 0xC0     // Data_code_0 = SPI byte 0xC0
#define _1 0xFC     // Data_code_1 = SPI byte 0xFC
#define _RESET 0x00 // Data_reset  = SPI byte 0x00

int Red[8], Green[8], Blue[8]; //declare RGB
int Dec_Bin[8];  //convert decimal to binary
int Bin_Off[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //off in binary form
int Bin_On[8] = {1, 1, 1, 1, 1, 1, 1, 1};  //on in binary form

bool _isLongPressed = true;

enum States {IS_OFF, IS_WHITE, IS_YELLOW, IS_RGB, IS_RGBCYC, IS_SLEEP, IS_BLUE, IS_RED, IS_PINK, IS_GREEN};
States state = IS_OFF;
States curr_state = IS_WHITE;

enum PowerBtnStates {PWR_NONE, PWR_CLK, PWR_LPRESS};
PowerBtnStates pwrbtnState = PWR_NONE;

enum TouchBtnStates {T_NONE, T_DBLCLK, T_LPRESS, T_LPRSTOP};
TouchBtnStates tchbtnState = T_NONE;

void switchMachine(){
  switch (state){
    case IS_OFF:
      lightOff();
      if (pwrbtnState == PWR_LPRESS){
        state = IS_WHITE;
        lightWhite();
        pwrbtnState = PWR_NONE;
      }
      break;

    case IS_WHITE:
      switch (pwrbtnState){
        case PWR_CLK:
          state = IS_YELLOW;
          curr_state = IS_YELLOW;
          lightYellow();
          pwrbtnState = PWR_NONE;
          break;
        case PWR_LPRESS:
          state = IS_OFF;
          pwrbtnState = PWR_NONE;
          break;
      }
      switch (tchbtnState){
        case T_DBLCLK:
          state = IS_SLEEP;
          tchbtnState = T_NONE;
          break;
        case T_LPRESS:
          if (_isLongPressed){
            increaseWhite();
          }
          else{
            decreaseWhite();
          }
          tchbtnState = T_NONE;
          break;
      }
      break;

    case IS_YELLOW:
      switch (pwrbtnState){
        case PWR_CLK:
          state = IS_RGB;
          curr_state = IS_RGB;
          lightGreen();
          pwrbtnState = PWR_NONE;
          break;
        case PWR_LPRESS:
          state = IS_OFF;
          pwrbtnState = PWR_NONE;
          break;
      }
      switch (tchbtnState){
        case T_DBLCLK:
          state = IS_SLEEP;
          tchbtnState = T_NONE;
          break;
        case T_LPRESS:
          if (_isLongPressed){
            increaseYellow();
          }
          else{
            decreaseYellow();
          }
          tchbtnState = T_NONE;
          break;
      }
      break;

    case IS_RGB:
      switch (pwrbtnState){
        case PWR_CLK:
          state = IS_RGBCYC;
          curr_state = IS_RGBCYC;
          pwrbtnState = PWR_NONE;
          break;
        case PWR_LPRESS:
          state = IS_OFF;
          pwrbtnState = PWR_NONE;
          break;
      }
      switch (tchbtnState){
        case T_DBLCLK:
          state = IS_SLEEP;
          tchbtnState = T_NONE;
          break;
        case T_LPRESS:
          Serial.println("start with RED");
          if (_isLongPressed){
              IntensityRed();
              IntensityBlue();
              IntensityGreen();
          }
          tchbtnState = T_NONE;
          break;
      }
      break;

    case IS_RGBCYC:
      ColorsIntensityCycling();
      switch (pwrbtnState){
        case PWR_CLK:
          state = IS_WHITE;
          curr_state = IS_WHITE;
          pwrbtnState = PWR_NONE;
          break;
        case PWR_LPRESS:
          state = IS_OFF;
          pwrbtnState = PWR_NONE;
          break;
      }
      switch (tchbtnState){
        case T_DBLCLK:
          state = IS_SLEEP;
          tchbtnState = T_NONE;
          break;
      }
      break;

    case IS_SLEEP:
      lightOff();
      if ((pwrbtnState == PWR_CLK) || (tchbtnState == T_DBLCLK)){
        state = curr_state;
        switch (curr_state){
          case IS_WHITE:
            lightWhite();
            break;
          case IS_YELLOW:
            lightYellow();
            break;
          case IS_RGB:
            show_RGB();
            break;
          case IS_RGBCYC:
            ColorsIntensityCycling();
            break;
        }
        pwrbtnState = PWR_NONE;
        tchbtnState = T_NONE;
      }
      break;
  }
}

//convert decimal to binary
void convertBin (int num) {
  for (int i = 0; i < 8; i++){
    Dec_Bin[i] = 0;
  }
  int index = 0;
  while (num > 0) {
    index++;
    Dec_Bin[8 - index] = num % 2;
    num = num / 2;
  }
}

//function for RGB colors in binary form
void colors(int bin_col[8]) {
  for (int i = 0; i < 8; i++) {
    if (bin_col[i] == 1) {
      SPI.transfer(_1);
    } else
      SPI.transfer(_0);
  }
}

//functions for iled colors
void lightOff() {
  //codes for led off
  Serial.println("turn off the light");
  colors(Bin_Off);
  colors(Bin_Off);
  colors(Bin_Off);
}

void lightWhite() {
  //codes for led on
  Serial.println("here is the white light");
  colors(Bin_On);
  colors(Bin_On);
  colors(Bin_On);
}

void lightYellow() {
  //code for led yellow
  Serial.println("here is the yellow light");
  colors(Bin_On);
  colors(Bin_On);
  colors(Bin_Off);
}

void lightPink() {
  //code for led pink
  Serial.println("here is the pink light");
  colors(Bin_Off);
  colors(Bin_On);
  colors(Bin_On);
}

void lightGreen() {
  //code for led green
  Serial.println("here is the green light");
  colors(Bin_On);
  colors(Bin_Off);
  colors(Bin_Off);
}

void lightPurple() {
  //code for led purple
  Serial.println("here is the purple light");
  colors(Bin_Off);
  colors(Bin_On);
  colors(Bin_Off);
}

void lightBlue() {
  //code for led blue
  Serial.println("here is the blue light");
  colors(Bin_On);
  colors(Bin_Off);
  colors(Bin_On);
}

void IntensityBlue() {
  SPI.transfer(_RESET);
  Serial.println("Changing Intesity Of Blue");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Bin_Off);
    colors(Dec_Bin);
    delay(10);
  }
}

void IntensityYellow() {
  SPI.transfer(_RESET);
  Serial.println("Changing Intesity Of Yellow");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Dec_Bin);
    colors(Bin_Off);
    delay(10);
  }
}

void IntensityPink() {
  SPI.transfer(_RESET);
  Serial.println("Changing Intesity Of Pink");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Bin_Off);
    colors(Dec_Bin);
    colors(Dec_Bin);
    delay(10);
  }
}

void IntensityGreen() {
  SPI.transfer(_RESET);
  Serial.println("Changing Intesity Of Green");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Bin_Off);
    colors(Bin_Off);
    delay(10);
  }
}

void IntensityRed() {
  SPI.transfer(_RESET);
  Serial.println("Changing Intesity Of Red");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Bin_Off);
    colors(Dec_Bin);
    colors(Bin_Off);
    delay(10);
  }
}

void ColorsIntensityCycling() {
  SPI.transfer(_RESET);
  Serial.println("Cycling Colors' Intesity");
  for (;;) {    //infinite looping for colors with intensity
    IntensityYellow();
    IntensityGreen();
    IntensityBlue();
    IntensityPink();
    IntensityRed();
  }
}

//show rgb mode start from light green
void show_RGB(){
  lightGreen();
}

//change color intensity of RGB
void changecolorintensity(){
  Serial.println("change color intensity");
}

//increase intensity of white light
void increaseWhite(){
//  SPI.transfer(_RESET);
  Serial.println("Increase Intesity Of White");
  for (int i = 0; i <= 255; i++) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Dec_Bin);
    colors(Dec_Bin);
    delay(10);
  }
}

//decrease intensity of white light
void decreaseWhite(){
//  SPI.transfer(_RESET);
  Serial.println("Decrease Intesity Of White");
  for (int i = 255; i >= 0; i--) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Dec_Bin);
    colors(Dec_Bin);
    delay(10);
  }
}

//increase intensity of yellow
void increaseYellow(){
  SPI.transfer(_RESET);
  Serial.println("Increase Intesity Of Yellow");
  for (int i = 10; i <= 255; i++) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Dec_Bin);
    colors(Bin_Off);
    delay(10);
  }
}

//decrease intensity of yellow
void decreaseYellow(){
  SPI.transfer(_RESET);
  Serial.println("Decrease Intesity Of Yellow");
  for (int i = 255; i >=10; i--) {
    convertBin(i);
    colors(Dec_Bin);
    colors(Dec_Bin);
    colors(Bin_Off);
    delay(10);
  }
}

//action of power button
void pwr_longPressStart(){
  pwrbtnState = PWR_LPRESS;
}

void pwr_click(){
  pwrbtnState = PWR_CLK;
}

//action of touch button
void tch_doubleclick(){
  tchbtnState = T_DBLCLK;
}

void tch_longPressStart(){
  tchbtnState = T_LPRESS;
}

void tch_longPressStop(){
    _isLongPressed = !_isLongPressed;
    tchbtnState = T_LPRSTOP;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Power and Mode button
  powerbtn.attachLongPressStart(pwr_longPressStart);
  powerbtn.attachClick(pwr_click);
  SPI.begin();
  // Touch sensor
  touchbtn.attachDoubleClick(tch_doubleclick);
  touchbtn.attachDuringLongPress(tch_longPressStart);
  touchbtn.attachLongPressStop(tch_longPressStop);
}

void loop() {
 //Power Button
  powerbtn.tick();
  //Touch sensor
  touchbtn.tick();
  SPI.beginTransaction (SPISettings (8000000, MSBFIRST, SPI_MODE3));
  SPI.transfer(_RESET);
  //Turn off the led
  //turnOff();
  switchMachine();
  delay(10);
  SPI.endTransaction();
}
