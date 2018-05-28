#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal.h>
//#include <EEPROM.h>
#include <DS3231.h>
DS3231 Clock;
RTClib RTC;

Servo myservo;

const int rs=13, en=12, d4=9, d5=8, d6=7, d7=6;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

const int pos=90, posoff=0, poson=180;
const int gled=3, buzz=4, servo=5;

int motor=0, prevmotor=0;

//int addr=0;




byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
    byte& Hour, byte& Minute, byte& Second) {
  // Call this if you notice something coming in on 
  // the serial port. The stuff coming in should be in 
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j=0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] -48;
  Temp2 = (byte)InString[1] -48;
  Year = Temp1*10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] -48;
  Temp2 = (byte)InString[3] -48;
  Month = Temp1*10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] -48;
  Temp2 = (byte)InString[5] -48;
  Day = Temp1*10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;   
  // now Hour
  Temp1 = (byte)InString[7] -48;
  Temp2 = (byte)InString[8] -48;
  Hour = Temp1*10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] -48;
  Temp2 = (byte)InString[10] -48;
  Minute = Temp1*10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] -48;
  Temp2 = (byte)InString[12] -48;
  Second = Temp1*10 + Temp2;
}


//int prevdd=0;
int hrset=0, miset=0, hrdur=0, midur=0;
int hrset1=7, miset1=0, hrdur1=0, midur1=15;
int hrset2=16, miset2=0, hrdur2=0, midur2=15;

int set=0;

int dd,mm,yy,mi,se,hr;


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  Wire.begin();
  
  lcd.begin(16, 2);
  
  myservo.attach(servo);
  
  lcd.print("Automated");
  lcd.setCursor(0,1);
  lcd.print("Watering");
  lcd.setCursor(0,0);
  delay(2000);
  lcd.clear();
  
  pinMode(gled,OUTPUT);
  pinMode(buzz,OUTPUT);

  myservo.write(posoff);
  delay(500);
  myservo.write(pos);

  lcd.print("Initializing...");
  delay (1000);

  

  //eepromrd();


 
}




void loop() {
  // put your main code here, to run repeatedly:

  // If something is coming in on the serial line, it's
  // a time correction so set the clock accordingly.
  if (Serial.available()) 
  {
    GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

    Clock.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h

    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setDoW(DoW);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
  }

    DateTime now = RTC.now();
    
    yy=now.year();
    mm=now.month();
    dd=now.day();
    hr=now.hour();
    mi=now.minute();
    se=now.second();
    
  if (set==1)
  {
    hrset=hrset1;
    miset=miset1;
    hrdur=hrdur1;
    midur=midur1;
    datetime();
    Serial.print(set);
    if (motor==1)
    {
      set=1;
    }
    else
    {
      set=0;
    }
  }
  if (set==0)
  {
    hrset=hrset2;
    miset=miset2;
    hrdur=hrdur2;
    midur=midur2;
    datetime();
    Serial.print(set);
    if (motor==1)
    {
      set=0;
    }
    else
    {
      set=1;
    }
  }

  lcdtime();
  delay(1000);

    
}

void motorstate()
{
  if (motor==0)
  {
      myservo.write(posoff);
      digitalWrite(gled,LOW);
      lcd.clear();
      lcd.print("Motor OFF");
      delay(1000);
      myservo.write(pos);
  }

  else if (motor==1)
  {
      digitalWrite(buzz, HIGH);
      delay (200);
      digitalWrite(buzz, LOW);
      delay (800);
      digitalWrite(buzz, HIGH);
      delay (200);
      digitalWrite(buzz, LOW);
      delay (800);
      digitalWrite(buzz, HIGH);
      delay (200);
      digitalWrite(buzz, LOW);
      delay(800);
      digitalWrite(buzz, HIGH);
      delay (1000);
      digitalWrite(buzz, LOW);
      digitalWrite(gled, HIGH);
      myservo.write(posoff);
      delay(500);
      myservo.write(poson);
      lcd.clear();
      lcd.print("Motor ON");
      delay(1000);
      myservo.write(pos);
  }
}

void datetime()
{
  if(hr==hrset || hr<hrset+hrdur && hr>=hrset)
  {
    if(mi==miset || mi<miset+midur && mi>=miset)
    {
      motor=1;
      if(motor!=prevmotor)
      {
        prevmotor=motor;
        motorstate();
        Serial.print("MOTOR ON");
      }
    }
    else
    {
      motor=0;
      if(prevmotor!=motor)
      {
        prevmotor=motor;
        motorstate();
        Serial.print("MOTOR OFF");
      }
    }
  }
  else
  {
    motor=0;
    if(prevmotor!=motor)
    {
      prevmotor=motor;
      motorstate();
      Serial.print("MOTOR OFF");
    }
  }
}

void lcdtime()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time: ");
  lcd.print(hr);
  lcd.print(":");
  lcd.print(mi);
  lcd.print(":");
  lcd.print(se);
  lcd.setCursor(0,1);
  lcd.print("Date: ");
  lcd.print(dd);
  lcd.print("/");
  lcd.print(mm);
  lcd.print("/");
  lcd.print(yy);
}

/*void eepromsv()
{
  addr=1;
  EEPROM.write(addr, hrset);
  addr=addr+1;
  EEPROM.write(addr, miset);
  addr=addr+1;
  EEPROM.write(addr, hrdur);
  addr=addr+1;
  EEPROM.write(addr, midur);
  addr=addr+1;
}

void eepromrd()
{
  addr=1;
  hrset = EEPROM.read(addr);
  addr=addr+1;
  miset = EEPROM.read(addr);
  addr=addr+1;
  hrdur = EEPROM.read(addr);
  addr=addr+1;
  midur = EEPROM.read(addr);
  addr=addr+1;
}*/
