// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)
#define _DIST_MID 200.0   // distance where LED brightness is maximum (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;

  // wait until next sampling time
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // Adjust the LED brightness based on distance
  if (distance > _DIST_MAX || distance < _DIST_MIN) {
      analogWrite(PIN_LED, 0); // Turn off LED if out of range
  } else {
      // Map the distance to a brightness value
      int brightness = calculateBrightness(distance);
      analogWrite(PIN_LED, brightness);
  }

  // output the distance to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

// Calculate brightness based on distance
int calculateBrightness(float distance) {
    if (distance <= _DIST_MIN || distance >= _DIST_MAX) {
        return 0; // Minimum brightness for out-of-range values
    }
    
    if (distance == _DIST_MID) {
        return 255; // Maximum brightness at the mid distance (200mm)
    }

    // Calculate brightness based on linear interpolation between _DIST_MIN, _DIST_MID, and _DIST_MAX
    if (distance < _DIST_MID) {
        // Distance between _DIST_MIN and _DIST_MID
        return map(distance, _DIST_MIN, _DIST_MID, 0, 255);
    } else {
        // Distance between _DIST_MID and _DIST_MAX
        return map(distance, _DIST_MID, _DIST_MAX, 255, 0);
    }
}
