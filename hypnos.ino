//We always have to include the library
#include "LedControl.h"
#include "Wire.h"

#define DS3231_I2C_ADDRESS 0x68

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=1000;

const byte kidsAwoken = 0;
const byte kidsGoingToBed = 1;
const byte kidsSleeping = 2;
const byte kidsWakingUp = 3;

const byte Sunday =1;
const byte Monday = 2;
const byte Tuesday=3;
const byte Wednesday=4;
const byte Thursday=5;
const byte Friday=6;
const byte Saturday=7;


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
    
  Wire.begin();
  Serial.begin(9600);  
  
  // Clock initialization
  // Serial.print("Clock initialization\n");
  //setDS3231time(0, 58, 15, 1, 20, 3, 16);
}

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  }
}

void display(const byte* picture, int delayTime) {
  /* now display them one by one with a small delay */
  lc.setRow(0,0,picture[0]);
  lc.setRow(0,1,picture[1]);
  lc.setRow(0,2,picture[2]);
  lc.setRow(0,3,picture[3]);
  lc.setRow(0,4,picture[4]);
  lc.setRow(0,5,picture[5]);
  lc.setRow(0,6,picture[6]);
  lc.setRow(0,7,picture[7]);
  delay(delayTime);
}

const byte moon[]={ B00111100,
                    B01111110,
                    B11111111,
                    B11100011,
                    B11000001,
                    B11000001,
                    B01000000,
                    B00100000};

const byte glyphSize = sizeof(moon) / sizeof(moon[0]);

byte sun[]={ 
                B10001000,
                B01001001,
                B00101010,
                B10000001,
                B00011000,
                B01111110,
                B11111111,
                B11111111};

byte smiley[]={ 
                B00000000,
                B00000100,
                B00100100,
                B01100000,
                B01100000,
                B00100100,
                B00000100,
                B00000000};

void moveDown(const byte* glyph, byte count) {
  byte settingGlyph[count];
  memcpy(settingGlyph, glyph, count);

  for (int i = 0; i < count; ++i) {
    display(settingGlyph, 1000);
    // shift one row down
    for (int j = count - 1; j >= 0; --j)
      settingGlyph[j] = glyph[j] << i;
  }
  display(settingGlyph, 1000);
}

void moveUp(const byte* glyph, byte count) {
  byte risingGlyph[count];
  memset(risingGlyph, 0, count);

  for (int i = count - 1; i >= 0; --i) {
    display(risingGlyph, 1000);
    // shift one row up
    for (int j = 0; j < count; ++j)
      risingGlyph[j] = glyph[j] << i;
  }
  display(risingGlyph, 1000);
}

byte getKidsState() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  byte goingToBedHour = 20;
  byte goingToBedMin = 00;
  byte sleepingHour = 20;
  byte sleepingMin = 20;
  
  byte wakingUpHour = 7;
  byte wakingUpMin = 00;
  byte awokenHour = 7;
  byte awokenMin = 30;
  if (dayOfWeek == Saturday || dayOfWeek == Sunday) {
    wakingUpHour = 8;
    wakingUpMin = 0;
    awokenHour = 8;
    awokenMin = 30;
  }
  
  // Assumptions :
  // - kids are sleeping at midnight 
  // - kidsWakingUpTime < kidsAwokenTime < kidsGoingToBedTime < kidsSleepingTime
  if (hour > sleepingHour || (hour == sleepingHour && minute > sleepingMin) || (hour < wakingUpHour) || (hour == wakingUpHour && minute < wakingUpMin))
    return kidsSleeping;
    
  if (hour > goingToBedHour || (hour == goingToBedHour && minute > goingToBedMin))
    return kidsGoingToBed;

  if (hour > awokenHour || (hour == awokenHour && minute > awokenMin))
    return kidsAwoken;

  return kidsWakingUp;
}


void loop() { 
  displayTime();
  byte kidsState = getKidsState();

  switch (kidsState) {
    case kidsAwoken:
      lc.setIntensity(0,15);    
      display(smiley, 3000);
      break;
    case kidsGoingToBed:;
      lc.setIntensity(0,8);    
      moveUp(moon, glyphSize);
      break;
    case kidsSleeping:
      lc.setIntensity(0,1);    
      display(moon, 3000);
      break;
    case kidsWakingUp:
      lc.setIntensity(0,15);    
      moveUp(smiley, glyphSize);
      break;
  }  
}
