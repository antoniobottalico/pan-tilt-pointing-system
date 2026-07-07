/*
 * Two-Axis Pointing System (Pan-Tilt) — ESP32 + Two Servos
 * -----------------------------------------------------------
 * Controls two servos (pan and tilt axes) to aim at a target.
 *
 * Modes:
 *   - SERIAL CONTROL (default): send commands over Serial to set
 *     the pan/tilt angles precisely.
 *   - DEMO MODE: sweeps to random positions automatically, useful
 *     for testing wiring and range of motion.
 *
 * Serial command format (115200 baud, newline terminated):
 *   P<angle>        -> set pan servo angle,  e.g. "P90"
 *   T<angle>        -> set tilt servo angle, e.g. "T45"
 *   P<angle> T<angle> -> set both at once,   e.g. "P90 T45"
 *   D                -> toggle demo (random) mode on/off
 *   ?                -> print current status
 *
 * Hardware:
 *   Pan  servo signal -> GPIO25
 *   Tilt servo signal -> GPIO26
 *   Servo power (5V) from external supply, GND shared with ESP32
 *
 * Library: ESP32Servo (install via Library Manager)
 */

#include <ESP32Servo.h>

// ---------- Configuration ----------
const int PAN_PIN  = 25;
const int TILT_PIN = 26;

const int PAN_MIN_US  = 500;
const int PAN_MAX_US  = 2400;
const int TILT_MIN_US = 500;
const int TILT_MAX_US = 2400;

const int ANGLE_MIN = 0;
const int ANGLE_MAX = 180;

const unsigned long DEMO_INTERVAL_MS = 3000; // time between random moves in demo mode

// ---------- State ----------
Servo panServo;
Servo tiltServo;

int currentPan  = 90; // start centered
int currentTilt = 90;

bool demoMode = false;
unsigned long lastDemoMove = 0;

String inputBuffer = "";

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== Pointing System Initialized ===");
  Serial.println("Send commands: P<angle> T<angle> | D (toggle demo) | ? (status)");

  panServo.setPeriodHertz(50);
  tiltServo.setPeriodHertz(50);

  panServo.attach(PAN_PIN, PAN_MIN_US, PAN_MAX_US);
  tiltServo.attach(TILT_PIN, TILT_MIN_US, TILT_MAX_US);

  moveTo(currentPan, currentTilt);
  randomSeed(esp_random()); // hardware RNG, more reliable than floating analog pin

  printStatus();
}

// ---------- Main Loop ----------
void loop() {
  readSerialCommands();

  if (demoMode && millis() - lastDemoMove >= DEMO_INTERVAL_MS) {
    lastDemoMove = millis();
    int randomPan  = random(ANGLE_MIN, ANGLE_MAX + 1);
    int randomTilt = random(ANGLE_MIN, ANGLE_MAX + 1);
    moveTo(randomPan, randomTilt);
    printStatus();
  }
}

// ---------- Core Functions ----------
void moveTo(int panAngle, int tiltAngle) {
  panAngle  = constrain(panAngle, ANGLE_MIN, ANGLE_MAX);
  tiltAngle = constrain(tiltAngle, ANGLE_MIN, ANGLE_MAX);

  currentPan  = panAngle;
  currentTilt = tiltAngle;

  panServo.write(currentPan);
  tiltServo.write(currentTilt);
}

void printStatus() {
  Serial.print("Pan: ");
  Serial.print(currentPan);
  Serial.print(" deg | Tilt: ");
  Serial.print(currentTilt);
  Serial.print(" deg | Demo mode: ");
  Serial.println(demoMode ? "ON" : "OFF");
}

// ---------- Serial Command Parsing ----------
void readSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        parseCommand(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
}

void parseCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "?") {
    printStatus();
    return;
  }

  if (cmd == "D") {
    demoMode = !demoMode;
    Serial.print("Demo mode ");
    Serial.println(demoMode ? "enabled" : "disabled");
    return;
  }

  int newPan  = currentPan;
  int newTilt = currentTilt;
  bool validCommand = false;

  int pIndex = cmd.indexOf('P');
  int tIndex = cmd.indexOf('T');

  if (pIndex != -1) {
    int endIndex = (tIndex != -1 && tIndex > pIndex) ? tIndex : cmd.length();
    String valueStr = cmd.substring(pIndex + 1, endIndex);
    valueStr.trim();
    if (valueStr.length() > 0) {
      newPan = valueStr.toInt();
      validCommand = true;
    }
  }

  if (tIndex != -1) {
    int endIndex = (pIndex != -1 && pIndex > tIndex) ? pIndex : cmd.length();
    String valueStr = cmd.substring(tIndex + 1, endIndex);
    valueStr.trim();
    if (valueStr.length() > 0) {
      newTilt = valueStr.toInt();
      validCommand = true;
    }
  }

  if (validCommand) {
    demoMode = false; // manual command overrides demo mode
    moveTo(newPan, newTilt);
    printStatus();
  } else {
    Serial.println("Unrecognized command. Use: P<angle> T<angle> | D | ?");
  }
}
