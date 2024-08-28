#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define IR_SENSOR_IN 2       // IR sensor for entry (Pin 2)
#define IR_SENSOR_OUT 3      // IR sensor for exit (Pin 3)
#define LED_PIN 13           // LED indicator (optional)
#define RX_PIN 10            // Bluetooth RX pin (Pin 10)
#define TX_PIN 11            // Bluetooth TX pin (Pin 11)

// Initialize the LCD. Change the pin numbers to match your setup.
LiquidCrystal lcd(7, 8, 9, 4, 5, 6);
SoftwareSerial bluetooth(RX_PIN, TX_PIN);

int totalSeats = 0;          // Total seats in the room
int availableSeats = 0;      // Available seats

void setup() {
  pinMode(IR_SENSOR_IN, INPUT);
  pinMode(IR_SENSOR_OUT, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  lcd.begin(16, 2);          // Initialize a 16x2 LCD
  lcd.print("Seats Left: ");
  
  bluetooth.begin(9600);     // Initialize Bluetooth communication
  Serial.begin(9600);        // Initialize Serial for debugging
  
  lcd.setCursor(0, 1);
  lcd.print("Set Seats via BT");
}

void loop() {
  // Check for Bluetooth input to set total seats
  if (bluetooth.available()) {
    totalSeats = bluetooth.parseInt();  // Read the total number of seats from Bluetooth
    availableSeats = totalSeats;
    updateDisplay();
  }

  // Check for entry
  if (digitalRead(IR_SENSOR_IN) == HIGH) {
    delay(100);  // Debounce delay
    if (digitalRead(IR_SENSOR_OUT) == LOW) {
      personEntered();
    }
  }

  // Check for exit
  if (digitalRead(IR_SENSOR_OUT) == HIGH) {
    delay(100);  // Debounce delay
    if (digitalRead(IR_SENSOR_IN) == LOW) {
      personExited();
    }
  }
}

void personEntered() {
  if (availableSeats > 0) {
    availableSeats--;
    updateDisplay();
    digitalWrite(LED_PIN, HIGH);  // Optional LED indication
    delay(500);
    digitalWrite(LED_PIN, LOW);
  }
}

void personExited() {
  if (availableSeats < totalSeats) {
    availableSeats++;
    updateDisplay();
    digitalWrite(LED_PIN, HIGH);  // Optional LED indication
    delay(500);
    digitalWrite(LED_PIN, LOW);
  }
}

void updateDisplay() {
  lcd.clear();
  lcd.print("Seats Left: ");
  lcd.print(availableSeats);
}
