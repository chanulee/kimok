const int buttonPin = 7;  // The pin number where the button is connected
int buttonState = 0;      // Variable to store the button state
int lastButtonState = HIGH;  // Last state of the button (initialized to HIGH due to pull-up)

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin as input with internal pull-up resistor
  Serial.begin(9600);                // Initialize serial communication
}

void loop() {
  buttonState = digitalRead(buttonPin);  // Read the state of the button

  // Check if the button state has changed from not pressed to pressed
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.write('0');  // Send '0' as a byte when the button is pressed
  }

  lastButtonState = buttonState;  // Update the last button state

  delay(100);  // Small delay for debouncing
}