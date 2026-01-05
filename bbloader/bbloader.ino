#include <EEPROM.h>

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38

const int ADDR_SBB = 0, ADDR_SPEED = 10, ADDR_DIR = 20, ADDR_RAMP = 30;

long stepsPerBB;
int stepDelay, rampSteps;
bool reverseDir;

void loadSettings() {
  EEPROM.get(ADDR_SBB, stepsPerBB);
  EEPROM.get(ADDR_SPEED, stepDelay);
  EEPROM.get(ADDR_DIR, reverseDir);
  EEPROM.get(ADDR_RAMP, rampSteps);
  if (stepsPerBB <= 0) stepsPerBB = 100;
  if (stepDelay <= 0) stepDelay = 800;
}

void setup() {
  Serial.begin(115200);
  loadSettings();
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, HIGH);
}

void runStepper(int bbAmount) {
  long totalSteps = (long)bbAmount * stepsPerBB;
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(X_DIR_PIN, reverseDir ? LOW : HIGH);

  for (long i = 0; i < totalSteps; i++) {
    int currentDelay = stepDelay;
    if (i < rampSteps) currentDelay = stepDelay + (rampSteps - i);
    else if (i > (totalSteps - rampSteps)) currentDelay = stepDelay + (i - (totalSteps - rampSteps));

    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(currentDelay);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(currentDelay);
    
    if (i % stepsPerBB == 0) {
      Serial.print("PROGRESS:");
      Serial.println(bbAmount - (i / stepsPerBB));
    }

    if (Serial.available() > 0 && Serial.read() == 'S') {
      Serial.println("MSG:STOPPED");
      break;
    }
  }
  digitalWrite(X_ENABLE_PIN, HIGH);
  Serial.println("MSG:FINISHED");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil(':');
    String val = Serial.readStringUntil('\n');
    if (cmd == "START") runStepper(val.toInt());
    else if (cmd == "SET_SBB") { stepsPerBB = val.toInt(); EEPROM.put(ADDR_SBB, stepsPerBB); }
    else if (cmd == "SET_SPEED") { stepDelay = val.toInt(); EEPROM.put(ADDR_SPEED, stepDelay); }
    else if (cmd == "SET_RAMP") { rampSteps = val.toInt(); EEPROM.put(ADDR_RAMP, rampSteps); }
    else if (cmd == "SET_DIR") { reverseDir = (val.toInt() == 1); EEPROM.put(ADDR_DIR, reverseDir); }
  }
}