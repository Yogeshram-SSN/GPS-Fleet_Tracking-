int relayPin = 18;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(relayPin, HIGH); // Relay OFF
}

void loop() {
  digitalWrite(relayPin, LOW);  // Turn ON motor (active LOW)
  Serial.println("Motor ON");
  delay(7000); // Motor runs for 7 sec

  digitalWrite(relayPin, HIGH); // Turn OFF motor
  Serial.println("Motor OFF");
  delay(5000); // Motor is OFF for 5 sec
}

