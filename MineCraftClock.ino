#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define snz A3
#define set A1
#define up A2
#define dwn A0

#define almHrADDR 512
#define almMnADDR 513
#define almTypeADDR 514

#define rbday 7
#define alm 8

LiquidCrystal lcd(12,11,5,4,3,2);
SoftwareSerial mp3(9, 10);
RTC_DS1307 RTC;

char cr = 13;
String ext = ".mp3";
String files[] = 
{
"calm1",
"calm2",
"calm3",
"hal2",
"hal1", 
"hal3", 
"hal4", 
"piano3", 
"piano2", 
"nuance1", 
"nuance2",
"piano1",
"cat"};

byte moon[8] = {
  B00000,
  B10001,
  B00011,
  B00011,
  B00111,
  B01110,
  B11100,
};

byte sun[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100
};

boolean alarming = false;
boolean alarmOn = false;
unsigned int lastSecond;
unsigned int alarm[] = {6,0};
boolean almType = true;

void setup() {
  Serial.begin(9600);
  mp3.begin(9600);
    Wire.begin();
  RTC.begin();
  lcd.begin(16,2);
  lcd.createChar(0,sun);
  lcd.createChar(1, moon);
  delay(10);
  recallEEPROM();
  startSound();
  declarePins();
  lastSecond = RTC.now().second();
  updateTime();
}

void recallEEPROM() {
  alarm[0] = EEPROM.read(almHrADDR);
  alarm[1] = EEPROM.read(almMnADDR);
  if(EEPROM.read(almTypeADDR) == 0) {
    almType = true;
  }
  else {
    almType = false;
  }
}

void setEEPROM() {
  EEPROM.write(almHrADDR, alarm[0]);
  EEPROM.write(almMnADDR, alarm[1]);
 if(almType) {
   EEPROM.write(almTypeADDR, 0);
 }
  else {
    EEPROM.write(almTypeADDR, 1);
  }
}

void declarePins() {
 pinMode(snz, OUTPUT);
 pinMode(set, OUTPUT);
 pinMode(up, OUTPUT);
 pinMode(dwn, OUTPUT);
 pinMode(alm, OUTPUT);
 pinMode(rbday, OUTPUT);
 digitalWrite(snz, HIGH);
 digitalWrite(set, HIGH);
 digitalWrite(up, HIGH);
 digitalWrite(dwn, HIGH);
}

void loop() {
 chkInputs();
 if(RTC.now().second()-lastSecond >= 1) {
   lastSecond = RTC.now().second();
   updateTime();
   if(alarming) {
    if(!isPlaying()) {
      alarming = false;
      //sendMp3Command("PC F creeperBoom.mp3");
      Serial.print("SSSSSSSSS");
    } 
   }
 }
 delay(100);
}

void chkInputs() {
 if(digitalRead(snz) == LOW) {
   if(!isPlaying()) {
     if(checkHeld(snz, 20 , true)) {
       playRandomSong();
     }
   }
   else {
    sendMp3Command("PC S"); 
    alarming = false;
   }
 }
if(digitalRead(dwn) == LOW) {
  if(checkHeld(dwn,8,true)) {
    if(alarmOn){alarmOn = false;}
    else {alarmOn = true;}
  }
}

if(digitalRead(up) == LOW) {
  if(checkHeld(up,12,true)) {
   userSetTime();
  }
}

if(digitalRead(set) == LOW) {
 if(checkHeld(set,12, true)) {
   setAlarmTime();
 }
}
}

void setAlarmTime() {
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Use up/down to");
 lcd.setCursor(0,1);
 lcd.print("set alarm time");
 delay(2200);
 lcd.clear();
 lcd.setCursor(7,0);
 lcd.print(alarm[0]);
 
 while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
    if(alarm[0] < 23){ alarm[0]++;} 
   }
   else if(digitalRead(dwn) == LOW) {
      if(alarm[0] > 0){alarm[0]--;} 
   }
   lcd.clear();
   lcd.setCursor(5,0);
   lcd.print("H:");
   lcd.print(alarm[0]);
   printTime(4,1);
   delay(280);
}
 delay(750);
  while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
     if(alarm[1] < 59) {alarm[1]++;}
   }
   else if(digitalRead(dwn) == LOW) {
    if(alarm[1] > 0 ) {alarm[1]--;} 
   }
   lcd.clear();
   lcd.setCursor(5,0);
   lcd.print("M:");
   lcd.print(alarm[1]);
   printTime(4,1);
   delay(280);
  }
  delay(750);
  while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
    if(almType){almType = false;}
    else if(!almType){almType = true;}
   }
   else if(digitalRead(dwn) == LOW) {
      if(almType){almType = false;}
      else if(!almType){almType = true;}
   }
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("Alarm Type:");
   lcd.setCursor(3,1);
   if(almType)
     lcd.print("WEEKDAYS");
   else 
     lcd.print("WEEKENDS");
   delay(280);
}
  setEEPROM();
}



void userSetTime() {
  DateTime now = RTC.now();
  int uHour = now.hour() , uMin = now.minute() , uSec = now.second();
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Use up/down to");
 lcd.setCursor(0,1);
 lcd.print("set the time");
 delay(2200);
 lcd.clear();
 lcd.setCursor(7,0);
 lcd.print(uHour);
 
 while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
    if(uHour < 23){ uHour++;} 
   }
   else if(digitalRead(dwn) == LOW) {
      if(uHour > 0){uHour--;} 
   }
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("T:");
   lcd.print(uHour);
   printTime(4,1);
   delay(280);
}
 delay(750);
  while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
     if(uMin < 59) {uMin++;}
   }
   else if(digitalRead(dwn) == LOW) {
    if(uMin > 0 ) {uMin--;} 
   }
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("T:");
   lcd.print(uHour);
   lcd.print(":");
   lcd.print(uMin);
   printTime(4,1);
   delay(280);
  }
  delay(750);
  while(digitalRead(set) != LOW) {
   if(digitalRead(up) == LOW) {
     if(uSec < 59) {uSec++;}
   }
   else if(digitalRead(dwn) == LOW) {
    if(uSec > 0 ) {uSec--;} 
   }
   lcd.clear();
   lcd.setCursor(3,0);
    lcd.print("T:");
    lcd.print(uHour);
    lcd.print(":");
    lcd.print(uMin);
    lcd.print(":");
   lcd.print(uSec);
   printTime(4,1);
   delay(280);
  }
  delay(750);
  RTC.adjust(DateTime(now.year(), now.month(), now.day(), uHour, uMin, uSec));
}

void printTime(int col, int row) {
  DateTime now = RTC.now();
  lcd.setCursor(col,row);
  lcd.print(now.hour());
  lcd.print(":");
  if(now.minute() < 10) { lcd.print("0");}
  lcd.print(now.minute());
  lcd.print(":");
  if(now.second() < 10) { lcd.print("0");}
  lcd.print(now.second());
}


void updateTime() {
  lcd.clear();
  DateTime now = RTC.now();
  lcd.setCursor(4,0);
  lcd.print(now.hour());
  lcd.print(":");
  if(now.minute() < 10) { lcd.print("0");}
  lcd.print(now.minute());
  lcd.print(":");
  if(now.second() < 10) { lcd.print("0");}
  lcd.print(now.second());
  lcd.setCursor(1,1);
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.year());
  lcd.print(" ");
  lcd.print(getDayOfWeek(now.dayOfWeek()));
   lcd.setCursor(15,0);
  if(now.hour() > 6 && now.hour() < 19) {
  lcd.write((uint8_t)0);
  }
  else {
    lcd.write((uint8_t)1);
  }
  if(alarmOn) {
    digitalWrite(alm, HIGH);
    chkAlarm(now);
  }
  else {
    digitalWrite(alm, LOW);
  }
}

String getDayOfWeek(int dayOfWeekval) {
  switch(dayOfWeekval) {
   case 0: return "sun";
     break;
   case 1: return "mon";
     break;
   case 2: return "tues";
     break;
   case 3: return "wed";
     break;
   case 4: return "thrs";
     break;
   case 5: return "fri";
     break;
   case 6: return "sat";
     break;
   default: return "WTF";//LOL
     break;
  }

}

void chkAlarm(DateTime now) {
  if(now.hour() == alarm[0] && now.minute() == alarm[1] && now.second() <= 1) {
   if(almType && now.dayOfWeek() != 0 && now.dayOfWeek() != 6) {
      if(!isPlaying()) { 
        playRandomSong();
        alarming = true;
    }
   }
    else if(!almType) {
     if(now.dayOfWeek() == 0 || now.dayOfWeek() == 6){
         if(!isPlaying()) {
          playRandomSong();
          alarming = true;
        }
      }
    }
  }
}

boolean checkHeld(int pin, int loops, boolean update) {
  for(int i = 0; i < loops; i++){
    if(update) { updateTime(); }
    if(digitalRead(pin) == HIGH) {
      return false;
    }
    delay(100);
  }
   return true; 
}

void startSound() {
sendMp3Command("PC T 200");
delay(200);
sendMp3Command("PC T 99");
delay(200);
sendMp3Command("PC T 100");
delay(200);
sendMp3Command("PC T");
}

void playRandomSong() {
 randomSeed(millis());
  int  r = random(0,13); 
 if(r > 12) 
   return;
 String out = "PC F ";
 out += files[r];
 out += ext;
 Serial.print("Playing: ");
 Serial.println(files[r]);
 sendMp3Command(out);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("NOW PLAYING");
  lcd.setCursor(5,1);
  lcd.print(files[r]);
  delay(5000);
 }
 
boolean isPlaying() {
 sendMp3Command("PC Z"); 
 delay(4);
  String rcvString = "";
 if(mp3.available()) {
   while(mp3.available()) {
     char cChar = (char)mp3.read();
     if(cChar != '>' && cChar != cr) {
       rcvString += cChar;
     }
   }
 }
 if(rcvString.equals("") || rcvString.equals(" "))  {
   delay(1);
     return isPlaying();
   }
 for(int i =0; i<rcvString.length(); i++) {
  if(rcvString.charAt(i) == 'P')   { 
   Serial.println(rcvString); 
   Serial.println(" true");
   return true; 
  }
 }
    Serial.println(rcvString); 
     Serial.println(" false");
   return false;
}

void sendMp3Command(String Command) {
  mp3.print(Command);
  mp3.print(cr);
  delay(1);
}

