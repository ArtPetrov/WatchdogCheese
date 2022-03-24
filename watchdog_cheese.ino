const unsigned short dPinLedOn = 4;
const unsigned short dPinLedWorking = 6;
const unsigned short dPinLeftRotation = 8;
const unsigned short dPinRightRotation = 10;
const unsigned short aPinTimerDelay = 1;

const unsigned short maxRotationSecond = 300;
const unsigned short pauseBeforeChangeRotation = 60;
const unsigned short minWorkDuration = 30;

unsigned long globalWatcher = 0;
unsigned long rotationWatcher = 0;
unsigned long pauseWatcher = 0;
char currentRotation = 'L';
String engineState = "OFF"; // OFF | ROTATION | PAUSE_BEFORE_CHANGE_ROTATION

void setup() {
  Serial.begin(9600);
  Serial.println("Controller for cheese machine: ON");
  pinMode(dPinLedOn, OUTPUT);
  pinMode(dPinLedWorking, OUTPUT);
  pinMode(dPinLeftRotation, OUTPUT);
  pinMode(dPinRightRotation, OUTPUT);
  digitalWrite(dPinLedOn, true);
}

long currentDelay() {
  return constrain(map(analogRead(aPinTimerDelay), 0, 1023, 0, maxRotationSecond), 0, maxRotationSecond);
}

bool isManualOn() {
  return currentDelay() > minWorkDuration;
}

void stopEngine() {
  Serial.println("[INFO] Stop engine.");
  engineState = "OFF";
  digitalWrite(dPinLeftRotation, false);
  digitalWrite(dPinRightRotation, false);
}

void runEngine() {
  Serial.println("[INFO] Run engine.");
  engineState = "ROTATION";
  digitalWrite(dPinLeftRotation, currentRotation == 'L');
  digitalWrite(dPinRightRotation, currentRotation == 'R');
  rotationWatcher = millis();
}

void changeRotationDirection() {
  stopEngine();
  currentRotation = currentRotation == 'L' ? 'R' : 'L';
  engineState = "PAUSE_BEFORE_CHANGE_ROTATION";
  pauseWatcher = millis();
}

void Logs() {
  Serial.print("Engine state: [");
  Serial.print(engineState);
  Serial.print("]; Rotation: ");
  Serial.print(currentRotation);
  Serial.print("; Manual value timer: ");
  Serial.print(currentDelay());
  Serial.print("s; isManualOn: ");
  Serial.print(isManualOn());
  Serial.println(';');
}

void loop() {
  if (!isManualOn()) {
    Serial.println("[INFO] Manual enabled engine");
    stopEngine();
    digitalWrite(dPinLedWorking, isManualOn());
    delay(10000);
  }

  if (millis() - globalWatcher >= 1000) {
    Logs();
    globalWatcher = millis();
    digitalWrite(dPinLedWorking, isManualOn());
    if (isManualOn() && engineState == "OFF") {
      Serial.println("[INFO] Manual run engine");
      runEngine();
    }
  }

  if (millis() - rotationWatcher >= currentDelay() * 1000 && engineState == "ROTATION" && isManualOn()) {
    Serial.println("[INFO] Change rotation.");
    changeRotationDirection();
  }

  if (millis() - pauseWatcher >= pauseBeforeChangeRotation * 1000 && engineState == "PAUSE_BEFORE_CHANGE_ROTATION" && isManualOn()) {
    Serial.println("[INFO] Finished waiting pause.");
    runEngine();
  }
}