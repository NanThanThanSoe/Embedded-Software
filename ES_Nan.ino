#include <EEPROM.h>

const byte BUTTON = 0;
byte a,b;

String characters[50] = {"S", "L", "S", "L", "S", "L"};
String patternString = "";  //for random s and L
String userString = "";  //for input S and L from users

boolean buttonClick = false;  
boolean longPressClick = false;

long startTimer = 0;
long buttonTimer = 0;
long PressTime = 750;
long playtime = 0;

void writePattern(char add,String data);
String readPattern(char add);
char buffer[64]={};
long buffertime = 0;
char addr = 10;
int str;
int highestScore = 0;

String getData;
String data = "";
String record = "";
String score = "";
int minutes=0;
int seconds=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(BUTTON, INPUT);
  startGame();
}

void startGame(){
  patternString = "";
  userString = "";
  if ((readPattern(addr) != "") && (readPattern(addr+10) != "") && (readPattern(addr+20) != "")) {
    record = readPattern(addr+20);  //get playtime
    buffertime = record.toInt();
    Serial.print("\nHighest score by ");  
    Serial.print(readPattern(addr));  //name
    Serial.print(", Length = ");  
    Serial.print(readPattern(addr+10));  //length
    Serial.print(", Time = ");
    Serial.print(buffertime/60000); Serial.print(" minutes, ");
    Serial.print(buffertime * (0.001) - (buffertime/60000) * 60); Serial.println(" seconds");
    enterS();
  } else {
    enterS();
  }
}

void enterS() {
  Serial.println("\nPlease press s then enter to start the game.");
  delay(1000);
  startPlay();
}

//enter s to start the game
void startPlay() {
  while (Serial.available() == 0){ }
  b = Serial.readString()[0];
  if (b == 'S' || b == 's' && a == 10) {
    playGame();
  } 
}

void shortledSignal(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
}

void longledSignal(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);  
}

void endledSignal(){
  for (int i=0; i< 3; i++){
    digitalWrite(LED_BUILTIN, LOW);   
    delay(50);                      
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(50);                       
  }  
}

void generateSL() {
  patternString += characters[random(0, 2)] + " ";
  Serial.print("Length = "); 
  Serial.println(patternString.length()/2); //display length
  delay(750);
}

//during the game
void playGame() {
  generateSL();
  for (int i = 0; i < patternString.length(); i++){
     if (patternString.charAt(i) != 0){           //check if space
        Serial.print(patternString.charAt(i));
        if (patternString.charAt(i) == 83){        //check if S
            shortledSignal();
        }
        else if (patternString.charAt(i) == 76){   //check if L
            longledSignal();
       }
     }
     delay(750);
  }
  delay(750);
  endledSignal();
  Serial.println("\nYour turn !");
  startTimer = millis();
  userString = "";
}

void buttonSignal() {
  if (digitalRead(BUTTON) == LOW) {
      if (buttonClick == false) {
        buttonClick = true;
        buttonTimer = millis();
      }
      if ((millis() - buttonTimer > PressTime) && (longPressClick == false)) {
        longPressClick = true;
        delay(1000);
        Serial.print("L ");
        userString += "L ";
        longledSignal();
      }
  } else {
    if (buttonClick == true) {
      if (longPressClick == true) {
        longPressClick = false;
      } else {
        delay(1000);
        Serial.print("S ");
        userString += "S ";
        shortledSignal();
      }
      buttonClick = false;
    }
  }
  if (userString.length() == patternString.length()) {
    checkString();
  } 
}

void checkString() {
  if (userString == patternString){
    Serial.println(" - Good job!");
    Serial.println("______________________");
    delay(1000);
    playGame();
  } else {
    playtime = millis()+ (buttonTimer-startTimer);
    minutes = floor(playtime / 60000);
    seconds = round((playtime * (0.001))- minutes*60);
    Serial.println(" - Wrong input!"); 
    Serial.println("\nGame over!"); 
    Serial.print("You've made it to "); 
    Serial.print((patternString.length()/2)); 
    Serial.print(" signals in "); 
    Serial.print(minutes); 
    Serial.print(" minutes,"); 
    Serial.print(seconds); 
    Serial.println(" seconds");
    gameScore();
  }
}

//Game score
void gameScore() {
//  Serial.println("Game score is ....");
  score = readPattern(addr+10);  //get length N 
  highestScore = score.toInt();
    if (((patternString.length()/2)-1) > highestScore){
    highestScore = (patternString.length()/2) - 1;
    Serial.print("Congratulation! You have made a new highest score: ");
    Serial.println(highestScore);
    Serial.print("Enter your name : ");
    delay(6000);
    if (Serial.available()>0){
      str=Serial.available();
      Serial.readBytes(buffer, str);
      for(int i=0; i<str+1; i++) {
        data += buffer[i];
      }
    } 
    writePattern(addr, data);  //Address 10 and data
    delay(10);
    writePattern(addr+10, String(highestScore)); 
    delay(10);
    writePattern(addr+20, String(playtime));
    delay(10);

    getData = readPattern(addr);
    if (getData != ""){
      Serial.print(getData);
    }  
    delay(1000);
    startGame();
  } else {
    delay(1000);
    startGame();
  }
}

String readPattern(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int m=0;
  unsigned char n;
  n = EEPROM.read(add);
  while(n != '\0' && m<500)   
  {    
    n = EEPROM.read(add + m);
    data[m]=n;
    m++;
  }
  data[m]='\0';
  return String(data);
}

void writePattern(char add,String data)
{
  int _r = data.length();
  int i;
  for(i=0; i<_r; i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_r,'\0');   //Add null 
  EEPROM.commit();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonSignal();
}
