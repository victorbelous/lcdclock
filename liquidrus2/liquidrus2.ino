#include <LiquidCrystalRus.h>
#include <DS3231.h>
#include <Wire.h>

LiquidCrystalRus lcd(8, 9, 4, 5, 6, 7);

DS3231 Clock;
bool Century = false;
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
  "Января  ",
  "Февраля ",
  "Марта   ",
  "Апреля  ",
  "Мая     ",
  "Июня    ",
  "Июля    ",
  "Августа ",
  "Сентября",
  "Октября ",
  "Ноября  ",
  "Декабря ",
};
String Line1, Line2; // строки дисплея
const byte ButtonPin = A0;
byte numButton, oldButton;
byte iDay;
boolean lswitch = true;
const byte notPressed = 0;
const byte rightButton = 1;
const byte upButton = 2;
const byte downButton = 3;
const byte leftButton = 4;
const byte selectButton = 5;
int timedelay = 2000; // задержка строки на дисплее
byte area; // поле для правки

byte pressButton(byte Pin)      {
  // right  0     0 - 120 1
  // up     143 130 - 210 2
  // down   329 280 - 380 3
  // left   504 420 - 580 4
  // select 741 700 - 880 5
  
  int Button = analogRead(Pin);
  delay(5);
  // Вправо Right
  if (Button < 120) {
    return 1;
  }
  // Вверх Up
  if (Button > 130 && Button < 210) {
    return 2;
  }
  // Вниз Down
  if (Button > 280 && Button < 380) {
    return 3;
  }
  // Влево Left
  if (Button > 420 && Button < 580) {
    return 4;
  }
  // Выбор Select
  if (Button > 700 && Button < 880) {
    return 5;
  }
  // Не нажата Not press
  if (Button > 900) {
    return 0;
  }
}

void printlcd(String lcdLine1, String lcdLine2)
{
  lcd.clear();
  lcd.print(lcdLine1);
  lcd.setCursor(0, 1);    // столбец, строка
  lcd.print(lcdLine2);
  // delay(2000);
}

void lcdprint1 () {

  Line1 = week[Clock.getDoW() - 1];

    Line2 = Clock.getDate();
    Line2 += " ";
    Line2 += month[Clock.getMonth(Century) - 1];
    Line2 += " 20";
    if (Clock.getYear() < 10)
      Line2 += "0";
    Line2 += Clock.getYear();


    printlcd(Line1, Line2);
    delay(timedelay);
}

void lcdprint2 () {
      Line1 = Line2;
    Line2 = "";
      if (Clock.getHour(h12, PM) < 10)
      Line2 += "0";
    Line2 += Clock.getHour(h12, PM);
    Line2 += ":";
    if (Clock.getMinute() < 10)
      Line2 += "0";
    Line2 += Clock.getMinute();
    Line2 += ":";
    if (Clock.getSecond() < 10)
      Line2 += "0";
    Line2 += Clock.getSecond();
    printlcd(Line1, Line2);
    delay(timedelay);
}

void lcdprint3 () {
      Line1 = Line2;
    Line2 = "Температура ";
    Line2 += Clock.getTemperature();
    printlcd(Line1, Line2);
    delay(timedelay);
}

void editDoW() {
     iDay = Clock.getDoW() - 1; // номер дня недели в массиве
    while (numButton != selectButton && numButton != leftButton) {
      Line1 = week[iDay];
      Line2 = " ";
      printlcd(Line1, Line2);
      delay(500);
      numButton = pressButton(ButtonPin);
      if (numButton == upButton) {     // up
        if (iDay > 0) iDay--;
      }
      if (numButton == downButton) { // down
        if (iDay < 6) iDay++;
      }
    }
    if (iDay != Clock.getDoW() - 1 && numButton == selectButton)
      Clock.setDoW(iDay + 1);
      numButton = leftButton;
}

void editDay(){
     iDay = Clock.getDate();
    numButton = notPressed;
    while (numButton != selectButton && numButton != rightButton) {
      Line1 = "";
      if (lswitch) {
        lswitch = false;
        if (iDay < 10)
          Line1 = " ";
        Line1 += iDay;
      }
      else {
        Line1 = "  ";
        lswitch = true;
      }
      Line1 += " ";
      Line1 += month[Clock.getMonth(Century) - 1];
      Line1 += " 20";
      if (Clock.getYear() < 10)
        Line1 += "0";
      Line1 += Clock.getYear();
      Line2 = " ";
      printlcd(Line1, Line2);

      numButton = pressButton(ButtonPin);
      delay(500);
      if (numButton == downButton) {     // up
        if (iDay > 1) iDay--;
      }
      if (numButton == upButton) { // down
        if (iDay < 31) iDay++;
      }
         if (iDay != Clock.getDate() && numButton == selectButton)
      Clock.setDate(iDay);
}
}
void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  Line1 = String();
  Line2 = String();
  numButton = pressButton(ButtonPin);
  timedelay = 2000;
  area = 0;     // править начинаю с дня недели
  //  area = 1 день месяца
  // 2 месяц
  // 3 - год
  // 4 - часы
  // 5 - минуты
  // 6 - секунды
    Clock.setClockMode(false); // 24 часа индикация 
}

void loop() {




    lcdprint1 ();


    numButton = pressButton(ButtonPin);

if (numButton != notPressed) timedelay = 1;

      lcdprint2 ();

    numButton = pressButton(ButtonPin);

      lcdprint3();

    numButton = pressButton(ButtonPin);

  if (numButton != notPressed) timedelay = 1;




  timedelay = 2000;

  if (numButton == upButton) { // up править день недели
    editDoW();
 
  }
  if (numButton == leftButton || numButton == selectButton) { // left  править день месяца
    editDay();  
  

    if (numButton == rightButton) {
      iDay = Clock.getMonth(Century) - 1;
      while (numButton != selectButton && numButton != leftButton) {
        if (lswitch) {
          lswitch = false;
          Line1 = Clock.getDate();
          Line1 += " ";
          Line1 += month[iDay];
          Line1 += " 20";
          if (Clock.getYear() < 10)
            Line1 += "0";
          Line1 += Clock.getYear();
          Line2 = " ";
          printlcd(Line1, Line2);
          numButton = pressButton(ButtonPin);
          delay(500);
        }
        else {
          lswitch = true;
          Line1 = Clock.getDate();
          Line1 += " ";
          Line1 += "        ";
          Line1 += " 20";
          if (Clock.getYear() < 10)
            Line1 += "0";
          Line1 += Clock.getYear();
          Line2 = " ";
          printlcd(Line1, Line2);
          numButton = pressButton(ButtonPin);
          delay(500);
        }
        if (numButton == downButton) {     // up
          if (iDay > 1) iDay--;
        }
        if (numButton == upButton) { // down
          if (iDay < 12) iDay++;
        }
        if (iDay != Clock.getMonth(Century) - 1)
          Clock.setMonth(iDay);
      }
    }
  }

 // if (numButton == selectButton) { // select править/смотреть будильники
    // select
if (numButton == 7) { //выключить смотреть/править будильники временно
    Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
    Line1 = "Будильник 1 ";
    Line1 += Clock.checkIfAlarm(1);
    Line2 = "Будильник 2 ";
    Line2 += Clock.checkIfAlarm(2);
    printlcd(Line1, Line2);
    delay(2000);
    //  Line1 = Line2;

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
    delay(2000);
    Line1 = Line2;

    Line2 = "Будильник 2:";
    Line2 += Clock.checkIfAlarm(2);
    printlcd(Line1, Line2);
    delay(2000);
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
    delay(2000);
  }
}