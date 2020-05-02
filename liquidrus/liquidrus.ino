#include <LiquidCrystalRus.h>
#include <DS3231.h>
#include <Wire.h>

LiquidCrystalRus lcd(8,9,4,5,6,7);

DS3231 Clock;
bool Century=false;
bool h12 = false;
bool PM = false;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

const char *week[] = {
  "Воскресенье",
  "Понедельник",
  "Вторник",
  "Среда",
  "Четверг",
  "Пятница",
  "Суббота",
};

const char *month[] = {
  "Января",
  "Февраля",
  "Марта",
  "Апреля",
  "Мая",
  "Июня",
  "Июля",
  "Августа",
  "Сентября",
  "Октября",
  "Ноября",
  "Декабря",
};

const byte ButtonPin = A0;
      int Button = 0;
      byte numButton, oldButton;

String Line1, Line2; // строки дисплея

byte pressButton(byte Pin)      {
  // right  0; 0 - 120
  // up     143 130 - 210
  // down   329 280 - 380
  // left   504 420 - 580
  // select 741 700 - 880
  Button = analogRead(Pin);
  if (Button < 120){
    return 1;
  }
  if (Button > 130 && Button < 210) {
    return 2;
  }
  if (Button > 280 && Button < 380) {
    return 3;
  }
  if (Button > 420 && Button < 580) {
    return 4;
  }
  if (Button > 700 && Button < 880) {
    return 5;
  }
  if (Button > 900) {
    return 0;
  }
}

void printlcd(String lcdLine1, String lcdLine2) 
{
  lcd.setCursor(0, 0);     // столбец, строка
  lcd.print("                ");
  lcd.setCursor(0, 0);     // столбец, строка
  lcd.print(lcdLine1);
  lcd.setCursor(0, 1);     // столбец, строка
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(lcdLine2);
  delay(2000);
}

void setup() {
  Wire.begin();
  lcd.begin(16,2);
  Line1 = String();
  Line2 = String();
}

void loop() {
  
  Line1 = week[Clock.getDoW() - 1];
  Line2 = Clock.getDate();
  Line2 += " ";
  Line2 += month[Clock.getMonth(Century) - 1];
  Line2 += " 20";
  Line2 += Clock.getYear();
  printlcd(Line1, Line2);
  Line1 = Line2;
  Line2 = ""; 
  
  if (Clock.getHour(h12,PM) < 10)
    Line2 += "0";  
  Line2 += Clock.getHour(h12,PM);
  Line2 += ":";
  if (Clock.getMinute() < 10)
    Line2 += "0";
  Line2 += Clock.getMinute();
  Line2 += ":";
  if (Clock.getSecond() < 10)
    Line2 += "0";
  Line2 += Clock.getSecond();
  printlcd(Line1, Line2);
 
  Line1 = Line2;
  Line2 = "Температура ";
  Line2 += Clock.getTemperature();
  printlcd(Line1,Line2);
  Line1 = Line2;

  Button = analogRead(ButtonPin);

  Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
  Line2 = "Будильник 1 ";
  Line2 += Clock.checkIfAlarm(1);
  printlcd(Line1, Line2);
  Line1 = Line2;

  Line2 = ADay;
  Line2 += " ";
  Line2 += AHour;
  Line2 += " ";
  Line2 += AMinute;
  Line2 += " ";
  Line2 += ASecond;
  Line2 += " ";
  Line2 += ABits;
  Line2 += " ";
  Line2 += ADy;
  Line2 += " ";
  Line2 += A12h;
  Line2 += " ";
  Line2 += Apm;
  printlcd(Line1, Line2);
  Line1 = Line2;
    
  Line2 = "Будильник 2:";
  Line2 += Clock.checkIfAlarm(2);
  printlcd(Line1, Line2);
  Line1 = Line2;
    
  Clock.getA2Time(ADay, AHour, AMinute, ABits, ADy, A12h, Apm);
  Line2 = ADay;
  Line2 += " ";
  Line2 += AHour;
  Line2 += " ";
  Line2 += AMinute;
  Line2 += " ";
  Line2 += ABits;
  Line2 += " ";
  Line2 += ADy;
  Line2 += " ";
  Line2 += A12h;
  Line2 += " ";
  Line2 += Apm;
  printlcd(Line1, Line2);
}
