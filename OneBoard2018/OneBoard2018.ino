#include <FlexCAN.h>
#include <TeensyCANBase.h>
#include <Encoder.h>

int toggle = 0;

struct encoderData {
  long lastRead;
  long pos;
  long rate;
};

Encoder armEncoder(7, 8);
encoderData armData;
Encoder rightEncoder(9, 10);
encoderData rightData;
Encoder leftEncoder(11, 12);
encoderData leftData;

const int limitReadLeft = 16;
const int limitReadRight = 20;
const int limitWriteLeft = 17;
const int limitWriteRight = 21;

int limitStateLeft = 0;
int limitStateRight = 0;

int limitOutputLeft = 0000;
int limitOutputRight = 0000;

void resetArmEncoder(byte * msg) {
  if (msg[0] == 0x72 && msg[1] == 0x65 && msg[2] == 0x73 && msg[3] == 0x65 && msg[4] == 0x74 && msg[5] == 0x65 && msg[6] == 0x6e && msg[7] == 0x63) {
    armEncoder.write(0);
    armData.pos = 0;
    armData.rate = 0;
    Serial.println("reset arm");
  }
}

void resetLeftEncoder(byte * msg) {
  if (msg[0] == 0x72 && msg[1] == 0x65 && msg[2] == 0x73 && msg[3] == 0x65 && msg[4] == 0x74 && msg[5] == 0x65 && msg[6] == 0x6e && msg[7] == 0x63) {
    leftEncoder.write(0);
    leftData.pos = 0;
    leftData.rate = 0;
    Serial.println("reset left");
  }
}

void resetRightEncoder(byte * msg) {
  if (msg[0] == 0x72 && msg[1] == 0x65 && msg[2] == 0x73 && msg[3] == 0x65 && msg[4] == 0x74 && msg[5] == 0x65 && msg[6] == 0x6e && msg[7] == 0x63) {
    rightEncoder.write(0);
    rightData.pos = 0;
    rightData.rate = 0;
    Serial.println("reset right");
  }
}

void setup(void) {
  CAN_begin();
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  delay(1000);
  Serial.println("Teensy 3.X CAN Sender");
  digitalWrite(13, HIGH);
  toggle = 1;

  leftData.lastRead = 0;
  leftData.pos = -999;
  leftData.rate = 0;
  CAN_add_id(0x610, &resetLeftEncoder);
  
  rightData.lastRead = 0;
  rightData.pos = -999;
  rightData.rate = 0;
  CAN_add_id(0x611, &resetRightEncoder);
  
  armData.lastRead = 0;
  armData.pos = -999;
  armData.rate = 9;
  CAN_add_id(0x612, &resetArmEncoder);

  pinMode(limitReadLeft, INPUT);
  pinMode(limitReadRight, INPUT);
  pinMode(limitWriteLeft, OUTPUT);
  pinMode(limitWriteRight, OUTPUT);  
}


void writeLongs(uint32_t id, long value1, long value2){
  byte * msg = new byte[8];

  for (int i = 0; i < 4; i++) {
    msg[i] = (value1 >> i * 8) & 0xFF;
  }
  for (int i = 0; i < 4; i++) {
    msg[i + 4] = (value2 >> i * 8) & 0xFF;
  }
  
  CAN_write(id, msg);

  delete msg;
}

void loop(void) {
  CAN_update();
  long newPos = leftEncoder.read();
  if (newPos != leftData.pos) {
    leftData.rate = ((double) 1000000.0 * (newPos - leftData.pos)) / ((double) (micros() - leftData.lastRead));
    leftData.pos = newPos;
    leftData.lastRead = micros();
  }
  else {
    if ((micros() - leftData.lastRead) > 1000) {
      leftData.rate = 0;
    }
  }
  writeLongs(0x610, leftData.pos, leftData.rate);

  
  newPos = rightEncoder.read();
  if (newPos != rightData.pos) {
    rightData.rate = ((double) 1000000.0 * (newPos - rightData.pos)) / ((double) (micros() - rightData.lastRead));
    rightData.pos = newPos;
    rightData.lastRead = micros();
  }
  else {
    if ((micros() - rightData.lastRead) > 1000) {
      rightData.rate = 0;
    }
  }
  writeLongs(0x611, rightData.pos, rightData.rate);

  
  newPos = armEncoder.read();
  if (newPos != armData.pos) {
    armData.rate = ((double) 1000000.0 * (newPos - armData.pos)) / ((double) (micros() - armData.lastRead));
    armData.pos = newPos;
    armData.lastRead = micros();
  }
  else {
    if ((micros() - armData.lastRead) > 1000) {
      armData.rate = 0;
    }
  }
  writeLongs(0x612, armData.pos, armData.rate);
  
  digitalWrite(limitWriteLeft, HIGH);
  
  limitStateLeft = digitalRead(limitReadLeft);
  if (limitStateLeft == HIGH) {
    limitOutputLeft = 0001;
  } else {
    limitOutputLeft = 0000;
  }
  
  digitalWrite(limitWriteRight, HIGH);
  limitStateRight = digitalRead(limitReadRight);
  if (limitStateLeft == HIGH) {
    limitOutputRight = 0001;
  } else {
    limitOutputRight = 0000;
  }
  
  writeLongs(0x613, limitOutputLeft, limitOutputRight);
  
  delay(50);
}
