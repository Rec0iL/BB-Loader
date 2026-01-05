#include <EEPROM.h>

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38

const int ADDR_SBB = 0, ADDR_SPEED = 10, ADDR_DIR = 20, ADDR_RAMP = 30, ADDR_HOLD = 40;

long stepsPerBB;
int stepDelay, rampSteps, holdTime;
bool reverseDir;

void loadSettings() {
  EEPROM.get(ADDR_SBB, stepsPerBB);
  EEPROM.get(ADDR_SPEED, stepDelay);
  EEPROM.get(ADDR_DIR, reverseDir);
  EEPROM.get(ADDR_RAMP, rampSteps);
  EEPROM.get(ADDR_HOLD, holdTime);
  if (stepsPerBB <= 0) stepsPerBB = 100;
  if (stepDelay <= 0) stepDelay = 800;
  if (rampSteps < 0) rampSteps = 500;
  if (holdTime < 0) holdTime = 5;
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
  bool stopped = false;

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
      stopped = true;
      break;
    }
  }

  if (!stopped) {
    Serial.println("MSG:FINISHED");
    if (holdTime > 0) {
      unsigned long start = millis();
      while(millis() - start < (unsigned long)holdTime * 1000) {
        if (Serial.available() > 0) {
          break;
        }
        delay(1);
      }
    }
  }

  digitalWrite(X_ENABLE_PIN, HIGH);
}


void processCommand(char* cmd) {
  char* separator = strchr(cmd, ':');
  if (separator == NULL) {
    if (strcmp(cmd, "GET_ALL") == 0) {
      Serial.print("SBB:");
      Serial.print(stepsPerBB);
      Serial.print(",SPEED:");
      Serial.print(stepDelay);
      Serial.print(",RAMP:");
      Serial.print(rampSteps);
      Serial.print(",DIR:");
      Serial.print(reverseDir);
      Serial.print(",HOLD:");
      Serial.println(holdTime);
    }
    return;
  }

  *separator = '\0';
  char* val_str = separator + 1;
  long val = atol(val_str);

  if (strcmp(cmd, "START") == 0) {
    runStepper(val);
  } else if (strcmp(cmd, "SET_SBB") == 0) {
    stepsPerBB = val;
    EEPROM.put(ADDR_SBB, stepsPerBB);
    Serial.print("ACK:SET_SBB:");
    Serial.println(stepsPerBB);
  } else if (strcmp(cmd, "SET_SPEED") == 0) {
    stepDelay = val;
    EEPROM.put(ADDR_SPEED, stepDelay);
    Serial.print("ACK:SET_SPEED:");
    Serial.println(stepDelay);
  } else if (strcmp(cmd, "SET_RAMP") == 0) {
    rampSteps = val;
    EEPROM.put(ADDR_RAMP, rampSteps);
    Serial.print("ACK:SET_RAMP:");
    Serial.println(rampSteps);
  } else if (strcmp(cmd, "SET_DIR") == 0) {
    reverseDir = (val == 1);
    EEPROM.put(ADDR_DIR, reverseDir);
    Serial.print("ACK:SET_DIR:");
    Serial.println(reverseDir);
  } else if (strcmp(cmd, "SET_HOLD") == 0) {
    holdTime = val;
    EEPROM.put(ADDR_HOLD, holdTime);
    Serial.print("ACK:SET_HOLD:");
    Serial.println(holdTime);
  }
}

void loop() {
  static char cmdBuffer[64];
  static int cmdIndex = 0;

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      cmdBuffer[cmdIndex] = '\0';
      processCommand(cmdBuffer);
      cmdIndex = 0;
    } else if (cmdIndex < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdIndex++] = c;
    }
  }
}