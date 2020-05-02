const int ButtonPin = A0;
      int Button = 0;
      boolean buttonRight, buttonUp, buttonDown, buttonLeft, buttonSelect;
      byte numButton, oldButton;
      
void setup() {
  Serial.begin(9600);

}

void loop() {
  // right  0; 0 - 120
  // up     143 130 - 210
  // down   329 280 - 380
  // left   504 420 - 580
  // select 741 700 - 880
  Button = analogRead(ButtonPin);
//  Serial.println(Button);
  if (Button < 120){
    numButton = 1;
    buttonRight = true;
    buttonUp = buttonDown = buttonLeft = buttonSelect = false;
  }
  if (Button > 130 && Button < 210) {
    numButton = 2;
    buttonUp = true;
    buttonRight = buttonDown = buttonLeft = buttonSelect = false;
  }
  if (Button > 280 && Button < 380) {
    numButton = 3;
    buttonDown = true;
    buttonRight = buttonUp = buttonLeft = buttonSelect = false;
  }
  if (Button > 420 && Button < 580) {
    numButton = 4;
    buttonLeft = true;
    buttonRight = buttonUp = buttonDown = buttonSelect = false;
  }
  if (Button > 700 && Button < 880) {
    numButton = 5;
    buttonSelect = true;
    buttonRight = buttonUp = buttonDown = buttonLeft = false;
  }
  if (Button > 900) {
    numButton = 0;
    buttonRight = buttonUp = buttonDown = buttonLeft = buttonSelect = false;
  }
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
