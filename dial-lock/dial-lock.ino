#include <PciManager.h>
#include <Debouncer.h>
#include <Rotary.h>

#define ROT_PIN_A A4
#define ROT_PIN_B A5
#define ROT_PUSH A3

#define GREEN 5
#define RED 6

#define OPEN 0
#define WAIT 1
#define ERROR 2

// -- Define method signatures.
void onRotate(short direction, Rotary* rotary);
void onRotPushPress();
void onRotPushRelease(unsigned long pressTime);

Rotary r(ROT_PIN_A, ROT_PIN_B, onRotate, true);
Debouncer rotPushDebouncer(ROT_PUSH, MODE_CLOSE_ON_PUSH, onRotPushPress, onRotPushRelease, true);

//short secret[] = { -4, 7 };
//short lastidx = 1;

short secret[] = { 2, -2, 1, -1 };
short lastidx = 3;

short state = WAIT;

short idx = 0;
short pos = 0;


void setup() {
  Serial.begin(9600);
  PciManager.registerListener(ROT_PUSH, &rotPushDebouncer);

  pinMode(GREEN, OUTPUT);
  pinMode(RED,   OUTPUT);

  digitalWrite(GREEN, LOW);
  digitalWrite(RED,   LOW);

  Serial.println("Ready.");
}

void onRotate(short direction, Rotary* rotary) {
  if(direction == DIRECTION_CW) {
    Serial.println("cw");
  }
  if(direction == DIRECTION_CCW) {
    Serial.println("ccw");
  }

  blinkRG();
  
  if (state == WAIT) {
    if (pos == secret[idx]) {
      if (pos * direction > 0) {
        state = ERROR;
      } else {
        idx = idx + 1;
        if (idx <= lastidx) {
          pos = direction;
        } else {
          state = ERROR;
        }
      }
    } else {
      if (secret[idx] * direction < 0) {
        state = ERROR;
      } else {
        pos = pos + direction;
      }
    }
  }
  Serial.print("Stav:");
  Serial.println(state);
  Serial.print("Disk:");
  Serial.println(idx);
  Serial.print("Poz: ");
  Serial.println(pos);
}

void onRotPushPress() {
  Serial.println("pushed");
  if (state == WAIT && idx == lastidx && pos == secret[idx]) {
    state = OPEN;
    Serial.println("Otevreno");
    digitalWrite(GREEN, HIGH);
  } else {
    if (state == OPEN) {
      digitalWrite(GREEN, LOW);
      Serial.println("Prave zavreno");
    } else {
      blinkError();
      Serial.println("Chyba");
    }
    pos = 0;
    idx = 0;
    state = WAIT;
  }

}
void onRotPushRelease(unsigned long pressTime) {
  Serial.println("released");
}

void blinkRG() {
  digitalWrite(GREEN, HIGH);
  digitalWrite(RED,   HIGH);

  delay(10);
  
  digitalWrite(GREEN, LOW);
  digitalWrite(RED,   LOW);
}

void blinkError() {
  for (int i=0;i<5;i++) {
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
    delay(500);
  }
}
