#include <Arduino.h>

const int rowCount = 8;
const int columnCount = 8;

const int buttonPin = 21; // The pin the button is connected to
int lastButtonState = HIGH; // Assuming pull-up resistor, so button press brings pin LOW
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers
int buttonState;                     // the current reading from the input pin


int currentPattern = 0;
unsigned long previousMillis = 0; // Stores the last time the pattern was updated
const long interval = 2000;       // Interval at which to switch patterns (milliseconds)

// Example pin mapping
// physical datasheet pins  = {13, 3, 4, 10, 6, 11, 15, 16}
int columnPins[columnCount] = {2,  3, 4,  5, 6,  7,  8, 9};   // X1 to X8 (Columns)

// physical datasheet pins  = { 9, 14, 8,  12, 1,  7,  2,  5}
int rowPins[rowCount]       = {10, 11, 12, 13, 14, 15, 16, 17}; // Y1 to Y8 (Rows with resistors)


void lightLED(int row, int col);
void clearMatrix();
void displayRow(int rowNum, byte pattern);
void displayPattern(byte pattern[], int size);

byte A[8] = {
  B00011000,
  B00100100,
  B01000010,
  B01111110,
  B01000010,
  B01000010,
  B01000010,
  B00000000};


// Define an 8x8 heart pattern
byte heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

// Define an 8x8 smiley face pattern
byte smiley[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// Define an 8x8 arrow pattern
byte arrow[8] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B00011000,
  B00011000,
  B00011000,
  B00000000
};

// Array of patterns
byte* patterns[] = {A, heart, smiley, arrow};
int numberOfPatterns = sizeof(patterns) / sizeof(patterns[0]);


void setup() {
  // Start the serial communication at 9600 baud rate.
  // This rate should match the one configured in your serial monitor.
  Serial.begin(9600);

  // Initialize all row and column pins
  for (int i = 0; i < rowCount; i++) {
    pinMode(rowPins[i], OUTPUT);
  }
  for (int i = 0; i < columnCount; i++) {
    pinMode(columnPins[i], OUTPUT);
  }
  
  clearMatrix(); // Turn off all LEDs initially

  pinMode(buttonPin, INPUT_PULLUP); // Initialize the push button pin as input with pull-up resistor


  // Print a message to the serial monitor.
  Serial.println("Hello from Arduino!");
}




void loop() {

  // Turn on every LED sequentially
  #if 0
  for (int row = 0; row < rowCount; row++) {
    for (int col = 0; col < columnCount; col++) {
      lightLED(row, col);
      delay(100); // Delay to visually see the LED light up
      clearMatrix(); // Turn off the LED before moving to the next
    }
  }
  #endif

  // Show Patterns by Time
  #if 0
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Save the last time you displayed a new pattern
    previousMillis = currentMillis;

    clearMatrix(); // Optional, depending on your displayPattern logic

    // Move to the next pattern
    currentPattern = (currentPattern + 1) % numberOfPatterns;
  }

  // Display the current pattern
  displayPattern(patterns[currentPattern], 8);
  #endif

  // Show Patterns by Push button with debounce mechanism 
  #if 1
  // Read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // Check if the button state has changed from high to low (button press).
  if (reading != lastButtonState) {
    // Reset the debouncing timer.
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed and it's been longer than the debounce delay, update the state.
    if (reading != buttonState) {
      buttonState = reading;

      // Only act if the button has been pressed (state is LOW due to INPUT_PULLUP mode).
      if (buttonState == LOW) {
        currentPattern = (currentPattern + 1) % numberOfPatterns;
        Serial.print("Changing to pattern: ");
        Serial.println(currentPattern);
      }
    }
  }

  // Save the reading for the next loop iteration.
  lastButtonState = reading;

  // Display the current pattern
  displayPattern(patterns[currentPattern], 8);
  #endif
}


void lightLED(int row, int col) {
  // Ensure all other LEDs are turned off before lighting a new one
  clearMatrix();

  // Set the target row HIGH and all others LOW (common cathode)
  for (int i = 0; i < rowCount; i++) {
    digitalWrite(rowPins[i], i == row ? HIGH : LOW);
  }
  
  // Set the target column LOW and all others HIGH
  for (int i = 0; i < columnCount; i++) {
    digitalWrite(columnPins[i], i == col ? LOW : HIGH);
  }

  // Print the coordinates and status
  Serial.print("LED at (");
  Serial.print(row);
  Serial.print(", ");
  Serial.print(col);
  Serial.println(") is ON");
}

void clearMatrix() {
  // Set all rows LOW and all columns HIGH to turn off all LEDs
  for (int i = 0; i < rowCount; i++) {
    digitalWrite(rowPins[i], LOW);
  }
  for (int i = 0; i < columnCount; i++) {
    digitalWrite(columnPins[i], HIGH);
  }
}

void displayRow(int rowNum, byte pattern) {
    // Turn off all rows first
    for (int i = 0; i < 1; i++) {
        digitalWrite(rowPins[i], LOW);
    }

    // Set the pattern for the columns
    for (int col = 0; col < columnCount; col++) {
        bool isLedOn = bitRead(pattern, col); // Read each bit of the pattern
        digitalWrite(columnPins[col], isLedOn ? LOW : HIGH); // Set columns based on pattern
    }

    // Activate the selected row
    digitalWrite(rowPins[rowNum], LOW); // Set the row LOW to turn on the LEDs for common cathode

    // Keep the row on for some time (e.g., 1000 us)
    delayMicroseconds(1000);

    // Deactivate the row
    digitalWrite(rowPins[rowNum], HIGH); // Set the row HIGH to turn off before moving to the next
    // Keep the row on for some time (e.g., 1000 us)
    delayMicroseconds(1000);

}


void displayPattern(byte pattern[], int size) {
    for (int i = 0; i < size; i++) {
        displayRow(i, pattern[i]);
        clearMatrix();
    }
}
