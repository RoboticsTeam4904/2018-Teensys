#include <FlexCAN.h>
#include <TeensyCANBase.h>
int pin = 25;
void setup(void) {
  CAN_begin();
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Teensy 3.X CAN Sender");
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
  writeLongs(0x672, 101, 202);
  delay(10);
}
