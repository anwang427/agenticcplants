#include <Arduino.h>

static unsigned int
 photoresistorPins[] = {PC0, PC1, PC2, PC3},
 trigPins[] = {PB1, PB2, PB3, PB4},
 echoPins[] = {PD5, PD6, PD7, PB0};

static float photoresistorMaximums[] = {0, 0, 0, 0};

static unsigned char actions[] = {0, 0};

static const int
 motorPinOne = PD2,
 motorPinTwo = PD3,
 pumpPin = PD4,
 soilPin = PC4;

char print_buffer[255];

void updatePhotoresistorMaximums() {
 for (int i = 0; i < 4; i++) {
  const auto read = analogRead(photoresistorPins[i]);
  const auto max = photoresistorMaximums[i];
  if (read > max) {
   photoresistorMaximums[i] = read;
  }
  auto
   maxStr = String(max),
   readStr = String(read),
   percentStr = String((read / max) * 100.0);
  sprintf(
   print_buffer,
   "Maximum for sensor %d: %s",
   i,
   maxStr.c_str()
  );
  Serial.println(print_buffer);
  sprintf(
   print_buffer,
   "Current for sensor %d: %s (%s%%)",
   i,
   maxStr.c_str(),
   percentStr.c_str()
  );
  Serial.println(print_buffer);
 }
}

void setup() {
 Serial.begin(9600);
 while (!Serial); //wait for serial to become available

 //Setup photoresistor analog input pins
 for (int i = 0; i < 4; i++) {
  pinMode(photoresistorPins[i], INPUT);
 }

 //Setup ultrasonic trigger outputs
 for (int i = 0; i < 4; i++) {
  pinMode(trigPins[i], OUTPUT);
 }

 //Setup ultrasonic echo pins
 for (int i = 0; i < 4; i++) {
  pinMode(echoPins[i], INPUT);
 }

 //Setup motor outputs
 pinMode(motorPinOne, OUTPUT);
 pinMode(motorPinTwo, OUTPUT);
 pinMode(pumpPin, OUTPUT);

 //Setup moisture input pin
 pinMode(soilPin, INPUT);

 //Initialize maximums
 updatePhotoresistorMaximums();
}

int wrap(int i) {
 return i % 4;
}

//Resets all actions
void clearActions() {
 for (int i = 0; i < 2; i++) {
  actions[i] = 0;
 }
}

void ultrasonicSensorsPlease() {
 clearActions();

 int distances[] = {0, 0, 0, 0};

 float
  distance = 0,
  duration = 0;

 for (int i = 0; i < 4; i++) {
  digitalWrite(trigPins[i], LOW);
  delayMicroseconds(2);

  digitalWrite(trigPins[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPins[i], LOW);

  duration = pulseIn(echoPins[i], HIGH, 90000);
  if (duration >= 90000) {
   distance = 3;
  } else {
   //calculate the distance in metres. Run the same i for trigPins and echoPins; push accordingly
   distance = duration * 34.0 / 2.0;
  }

  auto distance_string = String(distance);
  sprintf(print_buffer, "Sensor %d depth: %s meters", i, distance_string.c_str());
  Serial.println(print_buffer);

  distances[i] = distance;
 }

 for (int x = 0; x < 1; ++x) {
  if (distances[x] > 0.2) {
   actions[(x < 2 ? 0 : 1)] = 1;
  }
 }
}

void photoresistorsPlease() {
 clearActions();
 updatePhotoresistorMaximums();

 for (int i = 0; i < 5; i++) {
  auto
   light1 = analogRead(photoresistorPins[wrap(i)]) / 1024.0,
   light2 = analogRead(photoresistorPins[wrap(i + 1)]) / 1024.0;

  //debugging only
  if (i < 4) {
   auto light1_string = String(light1 * 100.0);
   sprintf(print_buffer, "Light sensor %d: %s%%", i, light1_string.c_str());
   Serial.println(print_buffer);
  }
  //1 > 2, then we turn right wheel; 2 > 3 then still turn right. 1 = right
  if (light1 * 1.1 >= light2 || light1 * 0.9 >= light2) {
   actions[(i < 2 ? 0 : 1)] = 1;
  }
 }
}

unsigned long lastAction = 0;

void moveMotorsPlease() {
 const int
  left = actions[0],
  right = actions[1];

 //Set timestamp
// sprintf(print_buffer, "%d :: %d", left, right);
// Serial.println(print_buffer);
 if (!(left || right)) {
  lastAction = micros();
 }

 if (!digitalRead(motorPinOne) && !digitalRead(motorPinTwo)) {
  if (left && right) {
   digitalWrite(motorPinOne, HIGH);
   digitalWrite(motorPinTwo, HIGH);
  } else if (left) {
   digitalWrite(motorPinOne, HIGH);
  } else if (right) {
   digitalWrite(motorPinTwo, HIGH);
  }
 } else {
//  sprintf(print_buffer, "%ld :: %ld", lastAction, micros());
  Serial.println(print_buffer);
  if (micros() - lastAction > 180000) {
   digitalWrite(motorPinOne, LOW);
   digitalWrite(motorPinTwo, LOW);
  }
 }
}

void doSomethingWithMoisture() {
 const auto soilValue = analogRead(soilPin) / 1024.0;

 auto string_soil_value = String(soilValue * 100.0);
 sprintf(print_buffer, "Dryness percentage: %s%%", string_soil_value.c_str());
 Serial.println(print_buffer);

 if (soilValue > .5) {
  const auto value_to_write = (int)(255.0 * soilValue);
  sprintf(print_buffer, "Writing pump value: %d", value_to_write);
  Serial.println(print_buffer);
  analogWrite(pumpPin, value_to_write);
 } else {
  analogWrite(pumpPin, LOW);
 }
}

void clear() {
 Serial.write(27);
 Serial.print("[2J");
 Serial.write(27);
 Serial.println("[H");
}

void loop() {
 clear();
 photoresistorsPlease();
 moveMotorsPlease();
// ultrasonicSensorsPlease();
 doSomethingWithMoisture();
 delay(500); //delay 1/4 second
}