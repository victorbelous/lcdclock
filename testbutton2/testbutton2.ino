const byte ButtonPin = A0;
      int Button = 0;
      byte numButton, oldButton;

byte pressButton(byte Pin)      {
  // right  0;    0 - 120 1 righButton
  // up     143 130 - 210 2 upButton
  // down   329 280 - 380 3 downButton
  // left   504 420 - 580 4 leftButton
  // select 741 700 - 880 5 selectButton
  // > 900                0 notButton
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

void setup() {
  Serial.begin(9600);

}

void loop() {
  numButton = pressButton(ButtonPin);
  if (oldButton != numButton) {
    Serial.print(oldButton);
    Serial.print(" ");
    Serial.print(numButton);
    Serial.print(" ");
    Serial.println(Button);
  }
  oldButton = numButton;
  delay(5);
}
