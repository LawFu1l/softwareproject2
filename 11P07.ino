// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO
#define PIN_SERVO 10   // 서보 모터 핀 추가

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25        // sampling interval (unit: msec)
#define PULSE_DURATION 10  // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 180.0    // 최소 거리 (단위: mm)
#define _DIST_MAX 360.0    // 최대 거리 (단위: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficient to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  pinMode(PIN_SERVO, OUTPUT);  // 서보 핀 설정
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;

  // wait until next sampling time. 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // 거리 기반 서보 각도 제어
  int servoAngle = calculateServoAngle(distance); // 서보 각도 계산
  analogWrite(PIN_SERVO, servoAngle); // 서보에 각도 적용

  // LED 상태 제어
  if ((distance == 0.0) || (distance > _DIST_MAX)) {
      distance = _DIST_MAX + 10.0;    // Set Higher Value
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else if (distance < _DIST_MIN) {
      distance = _DIST_MIN - 10.0;    // Set Lower Value
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else {    // In desired Range
      digitalWrite(PIN_LED, 0);       // LED ON      
  }

  // output the distance to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.print(", Servo Angle: "); Serial.print(servoAngle);
  Serial.println("");
  
  last_sampling_time += INTERVAL;
}

// 거리 측정 함수
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

// 거리로부터 서보 각도를 계산하는 함수
int calculateServoAngle(float distance) {
  if (distance <= 180) {
      return 0; // 거리 18cm 이하: 각도 0도
  } else if (distance > 180 && distance <= 360) {
      // 거리 18~36cm 사이의 경우 0~180도 비례 계산
      return map(distance, 180, 360, 0, 180);
  } else {
      return 180; // 거리 36cm 이상: 각도 180도
  }
}
