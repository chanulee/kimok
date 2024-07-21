#include <Wire.h>            // Used to establish serial communication on the I2C bus
#include "SparkFun_TMAG5273_Arduino_Library.h" // Used to send and recieve specific information from our sensor

// Create a new sensor object
TMAG5273 sensor; 

// I2C default address
uint8_t i2cAddress = TMAG5273_I2C_ADDRESS_INITIAL;

// Set constants for setting up device
uint8_t conversionAverage = TMAG5273_X32_CONVERSION;
uint8_t magneticChannel = TMAG5273_XYX_ENABLE;
uint8_t angleCalculation = TMAG5273_XY_ANGLE_CALCULATION;


// Define pin for button
const int buttonPin = 2; // Change to the pin you're using
int buttonState = 0;

// Variables for angle calculation and movement detection
float previousAngle = 0.0;
float criteriaAngle = 0.0;
int previousZone = 0;
const float movementThreshold = 15.0; // Adjust this threshold as needed


void setup() 
{
  Wire.begin();
  // Start serial communication at 115200 baud
  Serial.begin(115200);
  
  // Initialize button pin as input
  pinMode(buttonPin, INPUT);

  // Begin example of the magnetic sensor code (and add whitespace for easy reading)
  Serial.println("TMAG5273 Example 3: Angle Calculations");
  Serial.println("");
  // If begin is successful (0), then start example
  if(sensor.begin(i2cAddress, Wire) == true)
  {
    Serial.println("Begin");
  }
  else // Otherwise, infinite loop
  {
    Serial.println("Device failed to setup - Freezing code.");
    while(1); // Runs forever
  }

  // Set the device at 32x average mode 
  sensor.setConvAvg(conversionAverage);

  // Choose new angle to calculate from
  // Can calculate angles between XYX, YXY, YZY, and XZX
  sensor.setMagneticChannel(magneticChannel);

  // Enable the angle calculation register
  // Can choose between XY, YZ, or XZ priority
  sensor.setAngleEn(angleCalculation);

}


void loop() {

    // Read button state
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    // Send a signal when the button is pressed
    Serial.println("0");
  }

  // Checks if mag channels are on - turns on in setup
  if((sensor.getMagneticChannel() != 0) && (sensor.getAngleEn() != 0)) {
    float currentAngle = sensor.getAngleResult(); // Corrected variable name

    Serial.print("XYX: ");
    Serial.println(currentAngle, 3);

    // Calculate the change in angle
    float angleChange = currentAngle - previousAngle;

    if (angleChange != 0) {
      float delta = currentAngle - criteriaAngle;
      if (delta < 0) delta += 360; // Normalize delta to be within 0-360

      int currentZone = 0;
      if (delta >= 0 && delta <= 120) {
        currentZone = 1;
      } else if (delta > 120 && delta <= 240) {
        currentZone = 2;
      } else if (delta > 240 && delta <= 360) {
        currentZone = 3;
      }

      if (previousZone != 0) {
        if ((previousZone == 1 && currentZone == 2) || 
            (previousZone == 2 && currentZone == 3) || 
            (previousZone == 3 && currentZone == 1)) {
          Serial.println("1"); // Zone number increases
        } else if ((previousZone == 3 && currentZone == 2) || 
                   (previousZone == 2 && currentZone == 1) || 
                   (previousZone == 1 && currentZone == 3)) {
          Serial.println("2"); // Zone number decreases
        }
      }

      previousZone = currentZone;
    } else {
      criteriaAngle = currentAngle;
    }

    previousAngle = currentAngle;

  } else {
    Serial.println("Mag Channels disabled, stopping..");
    while(1);
  }

  delay(100);
}