#include <FlexCAN.h>
#include <TeensyCANBase.h>
int toggle = 0;
void setup(void) {
  CAN_begin();
  pinMode(13, OUTPUT);
  delay(1000);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Teensy 3.X CAN Sender");
  digitalWrite(13, HIGH);
  toggle = 1;
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
  if (toggle == 1){
    digitalWrite(13, LOW);
    toggle = 0;
  } else {
    digitalWrite(13, HIGH);
    toggle = 1;
  }
}
void loop(void) {
  CAN_update();
  Serial.println("sent");
  writeLongs(0x607, 101, 202);
  Serial.println(101);
  delay(50);
}
