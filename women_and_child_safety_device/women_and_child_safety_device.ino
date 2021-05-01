#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 3);
#define TempPin A1

int sw1 = 7;

SoftwareSerial SIM900(7, 8); 
int HBSensor = 4;
int HBCount = 0;
int HBCheck = 0;
int TimeinSec = 0;
int HBperMin = 0;
int HBStart = 2;
int HBStartCheck = 0;
int TempValue; 
int DataCheck = 0;
int value = 0;     // Variable for reading pushbutton status
TinyGPS gps;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);
  Serial.print("WOMEN AND CHILD SAFETY DEVICE");
  Serial.println();
  pinMode(HBSensor, INPUT);
  pinMode(HBStart, INPUT_PULLUP);
  Timer1.initialize(800000); 
  Timer1.attachInterrupt( timerIsr );
   pinMode(sw1, INPUT);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WOMEN AND CHILD");
  lcd.setCursor(0,1);
  lcd.print("SAFETY DEVICE");
   digitalWrite(sw1, HIGH);
   delay(300);
}

void loop() {
  /**/
  if(digitalRead(HBStart) == LOW){lcd.setCursor(0,3);
  lcd.clear();
  lcd.print("HB Counting ..");
  lcd.setCursor(0,0);
  lcd.print("Current HB  : ");
  lcd.setCursor(0,1);
  lcd.print("Time in Sec : ");
  lcd.setCursor(0,2);
  lcd.print("HB per Min  : 0.0");
  Serial.print(" calculating heart beat ");
  HBStartCheck = 1;
  delay(200);
  }
  else if(HBStartCheck == 0)
  {
    if(!digitalRead(sw1))                         // checking if reset button is pressed or not
   {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Switch pressed");
              lcd.setCursor(0,1);
              lcd.print("GPS activate");
              Serial.print(" Switch Pressed emergency and gps activated ");
              gpsgsm();
    }
   TempValue = analogRead(TempPin); // Getting LM35 value and saving it in variable
   float TempCel = ( TempValue/1024.0)*500; // Getting the celsius value from 10 bit analog value
   float TempFarh = (TempCel*9)/5 + 32; // Converting Celsius into Fahrenhiet
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Temp  : ");
   lcd.print(TempCel);
   delay(500);
   Serial.print("temp:");
   Serial.println(TempCel);
   if (TempCel>40)
      {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("High");
              lcd.setCursor(0,1);
              lcd.print("Temperature");
              Serial.println(" High temperature is detected ");
              gpsgsm();
       }
  }
  if(HBStartCheck == 1)
  {
    while(1)
    {
      if((digitalRead(HBSensor) == HIGH) && (HBCheck == 0))
      {
        HBCount = HBCount + 1;
        HBCheck = 1;
        lcd.setCursor(14,0);
        lcd.print(HBCount);
        lcd.print(" ");
      }
      if((digitalRead(HBSensor) == LOW) && (HBCheck == 1))
      {
        HBCheck = 0;   
      }
      if(TimeinSec == 10)
      {
          HBperMin = HBCount * 6;
          HBStartCheck = 0;
          lcd.setCursor(14,2);
          lcd.print(HBperMin);
          Serial.print("HB per min:");
          Serial.println(HBperMin);
          if(HBperMin>90)
          {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Greater HB");
              lcd.setCursor(0,1);
              lcd.print("GPS activate");
              Serial.print(" heart beat is greater than treshold value ");
              gpsgsm();
          }
          lcd.print(" ");
          lcd.setCursor(0,3);
          lcd.print("Press Button again.");
          HBCount = 0;
          TimeinSec = 0; 
          break;     
      }
    
    } 
  }
  
}
void timerIsr()
{
  if(HBStartCheck == 1)
  {
      TimeinSec = TimeinSec + 1;
      lcd.setCursor(14,1);
      lcd.print(TimeinSec);
      lcd.print(" ");
  }
}

void gpsgsm()
{
                                 Serial.begin(9600);
                                 SIM900.begin(9600);  
                                  bool newData = false;
                                  unsigned long chars;
                                  unsigned short sentences, failed;
                                
                                  // For one second we parse GPS data and report some key values
                                  for (unsigned long start = millis(); millis() - start < 1000;)
                                  {
                                        while (Serial.available())
                                        {
                                              char c = Serial.read();
                                              //Serial.print(c);
                                              if (gps.encode(c)) 
                                                newData = true;  
                                        }
                                  }
                                
                                  if (newData)      //If newData is true
                                  {
                                            float flat, flon;
                                            unsigned long age;
                                            gps.f_get_position(&flat, &flon, &age);   
                                            SIM900.print("AT+CMGF=1\r"); 
                                            delay(40);
                                            SIM900.println("AT + CMGS = \"+919845529179\"");// recipient's mobile number with country code
                                            delay(30);
                                            SIM900.print("Latitude = ");
                                            SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
                                            SIM900.print(" Longitude = ");
                                            SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
                                            delay(20);
                                            SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
                                            delay(20);
                                            SIM900.println();
                                  }     
                                  Serial.println(failed);
                                 // if (chars == 0)
                                 // Serial.println("** No characters received from GPS: check wiring **");
                                 
                            }
