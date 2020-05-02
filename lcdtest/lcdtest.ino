#include <LiquidCrystalRus.h>

LiquidCrystalRus lcd(8, 9, 4, 5, 6, 7);

String Line1, Line2, Linetemp; // строки дисплея

const byte ButtonPin = A0;
const byte notPressed = 0;
const byte rightButton = 1;
const byte upButton = 2;
const byte downButton = 3;
const byte leftButton = 4;
const byte selectButton = 5;
byte numButton;

byte pressButton(byte Pin)      {
  // right  0     0 - 120 1
  // up     143 130 - 210 2
  // down   329 280 - 380 3
  // left   504 420 - 580 4
  // select 741 700 - 880 5
  int Button = analogRead(Pin);
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
}
void setup() {
  lcd.begin(16,2);
 // Line1 = Line2 = Linetemp = String();
}
String Line () {
  Linetemp = rightButton;
  Linetemp += " ";
  Linetemp += leftButton;
  return Linetemp;
}
void loop() {
  numButton = pressButton(ButtonPin);
  Line2 = rightButton;
  Line2 += " ";
  Line2 += leftButton;
  Line1 = "Line 1  3";
//  Line2 = "Line 2";
  printlcd(Line1, Line());
  delay(2000);
}
