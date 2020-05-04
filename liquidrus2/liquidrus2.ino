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
String Line1, Line2, Line3, Line4; // строки дисплея
const byte ButtonPin = A0;
byte numButton, oldButton;
byte iDay, lastDay;
boolean lswitch = true;
const byte notPressed = 0;
const byte rightButton = 1;
const byte upButton = 2;
const byte downButton = 3;
const byte leftButton = 4;
const byte selectButton = 5;
//int timedelay = 2000; // задержка строки на дисплее
byte area; // поле для правки
byte displ; // номер текущего экрана
const unsigned long interval = 1500;
unsigned long curtimer, lastimer;
boolean LineNo; // строки для дисплея не сформированы

byte pressButton(byte Pin)      {
  // right  0     0 - 120 1 rightButton
  // up     143 130 - 210 2 upButton
  // down   329 280 - 380 3 downButton
  // left   504 420 - 580 4 leftButton
  // select 741 700 - 880 5 selectButton
  //                      0 notPressed

  int Button = analogRead(Pin);
  delay(15);
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

byte lcdprint (byte displ, String linepr1, String linepr2) {
  if (LineNo) {
    printlcd(linepr1, linepr2);
    LineNo = false;
  }
  if (millis() - lastimer > interval) {
    displ++; // через время задержки переходим ко второму экрану
    lastimer = millis();
    LineNo = true;
  }
  return displ;
}


void editDoW() {
//lastimer = millis();
lastDay = 8; // чтобы выполнить printlcd первый раз

 while (numButton == selectButton || numButton == leftButton) {
  numButton = pressButton(ButtonPin);

 delay(150); // ждать отпускания кнопки
 }
//  numButton = notPressed;
  iDay = Clock.getDoW() - 1; // номер дня недели в массиве
  while (numButton != selectButton && numButton != leftButton) {
    Line1 = week[iDay];
    Line2 = " ";
    if (lastDay != iDay) printlcd(Line1, Line2);


    numButton = pressButton(ButtonPin);
        delay(150);
        Serial.println(numButton, DEC);
    if (numButton == upButton) {     // up
      lastDay = iDay;
      if (iDay > 0) iDay--;
    }
    if (numButton == downButton) { // down
      lastDay = iDay;
      if (iDay < 6) iDay++;
    }

  }
  if (iDay != Clock.getDoW() - 1 && numButton == selectButton)
    Clock.setDoW(iDay + 1);
  //  numButton = leftButton;
}

void editDay() {
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

void editMonth() {
  lastDay = 100;  // выполнить printlcd первый раз
  Serial.print("MnumButton ");
  Serial.print(numButton, DEC);
  Serial.print(" Marea ");
  Serial.println(area, DEC);
  iDay = Clock.getMonth(Century) - 1;
   while (numButton == selectButton || numButton == leftButton) {
  numButton = pressButton(ButtonPin);
  delay(150); //ждать отпускания кнопки
   }
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
      if (lastDay != iDay) printlcd(Line1, Line2);
      numButton = pressButton(ButtonPin);
      delay(150);
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
      if (lastDay != iDay) printlcd(Line1, Line2);
      numButton = pressButton(ButtonPin);
      delay(150);
    }
    if (numButton == downButton) {     // up
  lastDay = iDay;
      if (iDay > 1) iDay--;
    }
    if (numButton == upButton) { // down
      lastDay = iDay;
      if (iDay < 12) iDay++;
    }
    if (iDay != Clock.getMonth(Century) - 1)
      Clock.setMonth(iDay);
  }
}

void editYear() {
  lastDay = 100; // выполнить printlcd первый раз
  iDay = Clock.getYear();
  while (numButton == selectButton || numButton == leftButton) {
  numButton = pressButton(ButtonPin);
Serial.print("editYear ");
Serial.println(numButton, DEC);
 delay(150); // ждать отпускания кнопки
 }
  
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      lswitch = false;
      Line1 = Clock.getDate();
      Line1 += " ";
      Line1 += month[Clock.getMonth(Century) - 1];
      Line1 += " 20";
      if (iDay < 10)
        Line1 += "0";
      Line1 += iDay;
      Line2 = " ";
      if (lastDay != iDay) printlcd(Line1, Line2);

    }
    else {
      lswitch = true;
      Line1 = Clock.getDate();
      Line1 += " ";
      Line1 += month[Clock.getMonth(Century) - 1];
      Line2 = " ";
      if (lastDay != iDay) printlcd(Line1, Line2);
 
    }
         numButton = pressButton(ButtonPin);
      delay(150);
    if (numButton == downButton) {     // up
      lastDay = iDay;
      if (iDay > 0) iDay--;
    }
    if (numButton == upButton) { // down
      lastDay = iDay;
      if (iDay < 99) iDay++;
    }
    if (iDay != Clock.getYear())
      Clock.setYear(iDay);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  Line1 = String();
  Line2 = String();
  numButton = pressButton(ButtonPin);
  //  timedelay = 2000;
  displ = 1; // начинаем с первого экрана
  area = 0;     // править начинаю с дня недели
  //  area = 1 день месяца
  // 2 месяц
  // 3 - год
  // 4 - часы
  // 5 - минуты
  // 6 - секунды
  Clock.setClockMode(false); // 24 часа индикация
  curtimer = lastimer = millis();
  LineNo = true; // строчки не сформированы
}

void loop() {
  Line1 = week[Clock.getDoW() - 1];

  Line2 = Clock.getDate();
  Line2 += " ";
  Line2 += month[Clock.getMonth(Century) - 1];
  Line2 += " 20";
  if (Clock.getYear() < 10)
    Line2 += "0";
  Line2 += Clock.getYear();
  Line3 = "";
  if (Clock.getHour(h12, PM) < 10)
    Line3 += "0";
  Line3 += Clock.getHour(h12, PM);
  Line3 += ":";
  if (Clock.getMinute() < 10)
    Line3 += "0";
  Line3 += Clock.getMinute();
  Line3 += ":";
  if (Clock.getSecond() < 10)
    Line3 += "0";
  Line3 += Clock.getSecond();
  Line4 = "Температура ";
  Line4 += Clock.getTemperature();
  if (displ == 1) // высвечиваем первый экран
    displ =  lcdprint (displ, Line1, Line2);
  if (displ == 2) // высвечиваем второй экран
    displ =  lcdprint (displ, Line2, Line3);
  if (displ == 3)
    displ =  lcdprint (displ, Line3, Line4);
  if (displ == 4) displ = 1;
  numButton = pressButton(ButtonPin);

  if (numButton == leftButton)  // left начинаем править со дня недели
    ++area;
 // else area = 0;// Следующее поле
  //    numButton = notPressed;
    Serial.print("numButton ");
     Serial.print(numButton, DEC);
      Serial.print(" area ");
     Serial.println(area, DEC);
  if (area == 1) editDoW();
  if (area == 2) editYear();
  if (area == 3) editMonth();
  if (area == 4) editDay();
  if (area == 5) area = 0;



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
