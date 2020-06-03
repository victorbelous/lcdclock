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
byte dayMax [] =   // число дней по месяцам обычный
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
byte dayVis [] =   // число дней по месяцам високосный
{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
byte *dayCurr; // указатель на текущий массив дней
String Line1, Line2, LineWeek, LineDate, LineTime, LineTem, Line3; // строки дисплея
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
byte area; // поле для правки
byte displ; // номер текущего экрана
const unsigned long interval = 1500;
unsigned long lastimer;
boolean LineNo = true; // строки для дисплея сформированы
boolean alarm = true, oldalarm = true; // Будильник 1/2

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

void printlcd(String lcdLine1, String lcdLine2) {

  lcd.clear();
  lcd.print(lcdLine1);
  lcd.setCursor(0, 1);    // столбец, строка
  lcd.print(lcdLine2);
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
  lastDay = 8; // чтобы выполнить printlcd первый раз
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  iDay = Clock.getDoW() - 1; // номер дня недели в массиве
  while (numButton != selectButton && numButton != leftButton) {
    Line1 = week[iDay];
    Line2 = " ";
    if (lastDay != iDay) printlcd(Line1, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == upButton && iDay > 0) {     // up
      lastDay = iDay--;
    }
    if (numButton == downButton && iDay < 6) { // down
      lastDay = iDay++;
    }
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getDoW() - 1 && numButton == selectButton)
    Clock.setDoW(iDay + 1);
  ++area;

}

void editDay() {
  lastDay = 100; // выполнить printlcd  в первый раз
  if (Clock.getYear() % 4 == 0) //  високосный год
    dayCurr = dayVis;       // если високосный, то текущий массив дней - високосный
  else dayCurr = dayMax;  // иначе - обычный 28 дней в феврале
  if (Clock.getDate() > dayCurr[Clock.getMonth(Century)])
    Clock.setDate(dayCurr[Clock.getMonth(Century)]);
  iDay = Clock.getDate();
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    Line1 = "";
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
      }
      if (iDay < 10)
        Line1 = " ";
      Line1 += iDay;
    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      Line1 = "  ";
    }
    Line1 += " ";
    Line1 += month[Clock.getMonth(Century)];
    Line1 += " 20";
    if (Clock.getYear() < 10)
      Line1 += "0";
    Line1 += Clock.getYear();
    Line2 = " ";
    if (lastDay != iDay) printlcd(Line1, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == downButton && iDay > 1) {     // up
      lastDay = iDay--;
    }
    if (numButton == upButton && iDay < dayCurr[Clock.getMonth(Century)]) {
      lastDay = iDay++;
    }
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getDate() && numButton == selectButton)
    Clock.setDate(iDay);
  ++area;
}

void editMonth() {
  lastDay = 100;  // выполнить printlcd первый раз
  iDay = Clock.getMonth(Century);
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); //ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки

        lastimer = millis();
        lswitch = false;
      }
      if (Clock.getDate() < 10) Line1 = " ";
      else Line1 = "";
      Line1 += Clock.getDate();
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
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      if (Clock.getDate() < 10) Line1 = " ";
      else Line1 = "";
      Line1 += Clock.getDate();
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
    if (numButton == downButton && iDay > 0)      // up
      lastDay = iDay--;


    if (numButton == upButton && iDay < 11)  // down
      lastDay = iDay++;

    if (numButton == rightButton) break;
  }

  if (iDay != Clock.getMonth(Century))
    Clock.setMonth(iDay);

  ++area;
}

void editYear() {
  lastDay = 100; // выполнить printlcd первый раз
  lastimer = millis();
  lswitch = true; // переключатель моргания
  iDay = Clock.getYear();
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
      }
      if (Clock.getDate() < 10) Line1 = " ";
      else Line1 = "";
      Line1 += Clock.getDate();
      Line1 += " ";
      Line1 += month[Clock.getMonth(Century)];
      Line1 += " 20";
      if (iDay < 10)
        Line1 += "0";
      Line1 += iDay;
      Line2 = " ";
    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      if (Clock.getDate() < 10) Line1 = " ";
      else Line1 = "";
      Line1 += Clock.getDate();
      Line1 += " ";
      Line1 += month[Clock.getMonth(Century)];
      Line2 = " ";
    }
    printlcd(Line1, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == downButton && iDay > 0)      // up
      lastDay = iDay--;
    if (numButton == upButton && iDay < 99 )  // down
      lastDay = iDay++;
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getYear() && numButton == selectButton)
    Clock.setYear(iDay);
  ++area;
}

void editMin() {
  String Min;
  lastDay = 100;
  Line2 = "";
  iDay = Clock.getMinute();
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
      }
      if (iDay < 10)
        Min = "0"; else Min = "";
      Min += iDay;
    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      else Min = "  ";
    }
    Line3 = "";
    if (Clock.getHour(h12, PM) < 10)
      Line3 += "0";
    Line3 += Clock.getHour(h12, PM);
    Line3 += ":";
    Line3 += Min;
    Line3 += ":";
    if (Clock.getSecond() < 10)
      Line3 += "0";
    Line3 += Clock.getSecond();
    if (lastDay != iDay ) printlcd(Line3, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == downButton && iDay > 0)      // up
      lastDay = iDay--;
    if (numButton == upButton && iDay < 59)  // down
      lastDay = iDay++;
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getMinute() && numButton == selectButton)
    Clock.setMinute(iDay);
  ++area;
}

void editHour() {
  String Min;
  lastDay = 100;
  Line2 = "";
  iDay = Clock.getHour(h12, PM);
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
      }
      if (iDay < 10)
        Min = "0";
      else Min = "";
      Min += iDay;
    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      else Min = "  ";
    }
    Line3 = "";
    Line3 += Min;
    Line3 += ":";
    if (Clock.getMinute() < 10)
      Line3 += "0";
    Line3 += Clock.getMinute();
    // Line3 += Min;
    Line3 += ":";
    if (Clock.getSecond() < 10)
      Line3 += "0";
    Line3 += Clock.getSecond();
    if (lastDay != iDay) printlcd(Line3, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == downButton && iDay > 0)     // up
      lastDay = iDay--;
    if (numButton == upButton && iDay < 23)  // down
      lastDay = iDay++;
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getHour(h12, PM) && numButton == selectButton)
    Clock.setHour(iDay);
  ++area;
}

void resetSec() {
  String Sec;
  lastDay = 100;
  Line2 = "";
  iDay = Clock.getSecond();
  while (numButton == selectButton || numButton == leftButton) {
    numButton = pressButton(ButtonPin);
    delay(150); // ждать отпускания кнопки
  }
  while (numButton != selectButton && numButton != leftButton) {
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
      }
      if (iDay < 10)
        Sec = "0"; else Sec = "";
      Sec += iDay;
    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
      else Sec = "  ";
    }
    Line3 = "";
    if (Clock.getHour(h12, PM) < 10)
      Line3 += "0";
    Line3 += Clock.getHour(h12, PM);
    Line3 += ":";
    if (Clock.getMinute() < 10)
      Line3 += "0";
    Line3 += Clock.getMinute();
    Line3 += ":";
    Line3 += Sec;
    if (lastDay != iDay ) printlcd(Line3, Line2);
    numButton = pressButton(ButtonPin);
    delay(150);
    if (numButton == downButton) {     // down
      lastDay = iDay;
      iDay = 0;
    }
    if (numButton == upButton) { // up
      lastDay = iDay;
      iDay = 59;
    }
    if (numButton == rightButton) break;
  }
  if (iDay != Clock.getSecond() && numButton == selectButton)
    Clock.setSecond(iDay);
  ++area;

}

void displayCheckAlarm () {
    Line1 = "Будильник 1 ";
    if (Clock.checkIfAlarm(1))
    Line1 += "вкл";
    else Line1 += "выкл";
    Line2 = "Будильник 2 ";
    if (Clock.checkIfAlarm(2))
    Line2 += "вкл";
    else Line2 += "выкл";
    printlcd(Line1, Line2);
	delay(2000);
}
void editAlarm () {
	
  lastimer = millis();
  lswitch = true; // выключатель строк
  alarm = true; // переключатель строк
  Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
  while (numButton != selectButton && numButton != rightButton) {
    displayCheckAlarm();
    if (lswitch) {
      if (millis() - lastimer > interval / 3) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = false;
        if (alarm && numButton == upButton) Line1 = "";
        if (alarm && numButton == downButton) Line2 = "";
        //     else Line2 = "";
        oldalarm = alarm;
        if (alarm) alarm = false; else alarm = true;
      }

    }
    else {
      if (millis() - lastimer > interval / 4) { // по истечении переключить вывод строки
        lastimer = millis();
        lswitch = true;
      }
    }
    if (oldalarm != alarm)  {
      printlcd(Line1, Line2);
      oldalarm = alarm;
    }
    Serial.print("alarm = ");
    Serial.print(alarm, BIN);
    Serial.print(" oldalarm = ");
    Serial.print(oldalarm, BIN);
    Serial.print(" lswitch = ");
    Serial.print(lswitch, BIN);
    Serial.print(" numButton ");
    Serial.print(numButton, DEC);
    Serial.print(" lastimer = ");
    Serial.println(lastimer, DEC);
    Line1 = "Будильник 1 ";
    Line1 += Clock.checkIfAlarm(1);
    Line2 = "Будильник 2 ";
    Line2 += Clock.checkIfAlarm(2);
    numButton = pressButton(ButtonPin);
    delay (150);


  }

  delay(2000);
  //  Line1 = Line2;
  // Alarmbits 1
  // 31 - раз в секунду
  // 15 - раз в секунду
  // 14, 30 - сигнал, когда секунды совпадают
  // 12, 28 - сигнал, когда совпадают минуты и секунды
  // 8, 24 - сигнал, когда совпадают часы минуты и секунды
  // 0 - сигнал, когда совпадают день месяца, часы, минуты и секунды
  // 16 - сигнал, когда совпадают день недели, часы, минуты и секунды
  // alarmbits2
  // 7, 15 - сигнал раз в минуту, когда секунды 0
  // 6, 14 - сигнал, когда совпадают минуты
  // 8, 12 - сигнал, когда совпадают часы и минуты
  // 0 - сигнал, когда совпадают день, час и минута
  // 8 - сигнал, когда совпадают день недели, час и минута
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
  Serial.println(ABits, DEC);
  switch (ABits) {
    case 31:
      Line2 = " Раз в секунду";
    case 30:
      Line2 = " совпадение секунд";
    case 28:
      Line2 = "совпадение минуты, секунды";
    case 24:
      Line2 = "совпадение часы, минуты, секунды";
    case 16:
      Line2 = "совпадение неделя, часы, минуты, секунды";
    case 15:
      Line2 = "раз в секунду";
    case 14:
      Line2 = "совпадение секунд";
    case 12:
      Line2 = "совпадение минуты, секунды";
    case 8:
      Line2 = "совпадение часы, минуты, секунды";
    case 0:
      Line2 = "ме,де,ча,ми,се";
      break;
    default: ;
      break;
  }
  Serial.println(Line2);
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
void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  numButton = pressButton(ButtonPin);
  displ = 1; // начинаем с первого экрана
  area = 0;     // править начинаю с дня недели
  //  area = 1 день месяца
  // 2 месяц
  // 3 - год
  // 4 - часы
  // 5 - минуты
  // 6 - секунды
  Clock.setClockMode(false); // 24 часа индикация
  lastimer = millis();
}

void loop() {
  LineWeek = week[Clock.getDoW() - 1];
  if (Clock.getDate() < 10) LineDate = " ";
  else LineDate = "";
  LineDate += Clock.getDate();
  LineDate += " ";
  LineDate += month[Clock.getMonth(Century)];
  LineDate += " 20";
  if (Clock.getYear() < 10)
    LineDate += "0";
  LineDate += Clock.getYear();
  LineTime = "";
  if (Clock.getHour(h12, PM) < 10)
    LineTime += "0";
  LineTime += Clock.getHour(h12, PM);
  LineTime += "час ";
  if (Clock.getMinute() < 10)
    LineTime += "0";
  LineTime += Clock.getMinute();
  LineTime += "мин ";
  if (Clock.getSecond() < 10)
    LineTime += "0";
  LineTime += Clock.getSecond();
  LineTime += "с.";
  LineTem = "Температура ";
  LineTem += Clock.getTemperature();
  switch (displ) {              // печатаю строки на дисплее попарно и по очереди
    case 1:
      displ =  lcdprint (displ, LineWeek, LineDate);
      break;
    case 2:
      displ =  lcdprint (displ, LineDate, LineTime);
      break;
    case 3:
      displ =  lcdprint (displ, LineTime, LineTem);
      break;
    default:
      displ = 1;
      break;
  }
  numButton = pressButton(ButtonPin);
  if (numButton == leftButton)  // left начинаем править со дня недели
    ++area;
  else area = 0;// Следующее поле
  if (area == 1 && numButton != rightButton) editDoW();   // День  недели
  if (area == 2 && numButton != rightButton) editYear();  // год
  if (area == 3 && numButton != rightButton) editMonth(); // месяц
  if (area == 4 && numButton != rightButton) editDay();   // день
  if (area == 5 && numButton != rightButton) editMin();   // минуты
  if (area == 6 && numButton != rightButton) editHour();  // часы
  if (area == 7 && numButton != rightButton) resetSec();  // сброс секунд до 0 или 59
  if (area > 7) area = 0;     // выход из правки

//  if (numButton == rightButton) editAlarm();
  if (numButton == rightButton) {
	  displayCheckAlarm();
  lastimer = millis();
  while (numButton == rightButton) {
	  numButton = pressButton(ButtonPin);
	  delay(150); // ждать отпускания кнопки
  }
  if  (millis() - lastimer > interval) editAlarm();
  }
}
