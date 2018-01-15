#include <FlexCAN.h>
#include <TeensyCANBase.h>
int pin = 25;




void changeLEDs(byte* msg) {
  int mode = msg[6] + (msg[7] << 8);
  int value = msg[4] + (msg[5] << 8);
  int R = msg[2];
  int G = msg[1];
  int B = msg[0];
  Serial.println(mode);
}

void setup(void) {
  CAN_add_id(0x672, &changeLEDs);
  CAN_begin();
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Teensy 3.X CAN Receiver");
  digitalWrite(pin, HIGH);
}

void writeLongs(uint32_t id, long value1, long value2){
  byte * msg = new byte[8];

  for(int i = 0; i < 4; i++){
    msg[i] = (value1 >> i*8) & 0xFF;
  }
  for(int i = 0; i < 4; i++){
    msg[i + 4] = (value2 >> i*8) & 0xFF;
  }
  
  CAN_write(id, msg);

  delete msg;
}

void loop(void) {
  CAN_update();
  delay(10);
}

