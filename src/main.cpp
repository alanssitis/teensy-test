#include <Arduino.h>
#include <amp_msgs/SerialPacket.h>
#include <ros.h>

#define FS1 2
#define FWD 3
#define REV 4
#define BRK 13

#define STR 7
#define THR 10
#define FBR 11
#define BRK_0 14
#define BRK_1 15

#define STEER_MID 3000

bool is_updated;
ros::NodeHandle nh;

void packetHandler(const amp_msgs::SerialPacket& msg) {
  analogWrite(THR, msg.throttle);
  if (msg.direction == 1) {
    digitalWrite(FWD, HIGH);
    digitalWrite(REV, LOW);
  } else if (msg.direction == -1) {
    digitalWrite(FWD, LOW);
    digitalWrite(REV, HIGH);
  } else {
    digitalWrite(FWD, LOW);
    digitalWrite(REV, LOW);
  }

  // tone(STR, msg.steering);
  // use steering as brake
  if (msg.steering > STEER_MID) {
    digitalWrite(BRK_0, HIGH);
    digitalWrite(BRK_1, LOW);
  } else if (msg.steering < STEER_MID) {
    digitalWrite(BRK_0, LOW);
    digitalWrite(BRK_1, HIGH);

  } else {
    digitalWrite(BRK_0, LOW);
    digitalWrite(BRK_1, LOW);
  }

  is_updated = true;
}

ros::Subscriber<amp_msgs::SerialPacket> sub("/serial_packet", &packetHandler);

void setup() {
  is_updated = false;

  pinMode(THR, OUTPUT);
  // analogWriteResolution(12);   // Rely on default of 8-bits

  pinMode(STR, OUTPUT);

  pinMode(FS1, OUTPUT);
  pinMode(FWD, OUTPUT);
  pinMode(REV, OUTPUT);

  digitalWrite(FS1, HIGH);

  nh.initNode();
  nh.subscribe(sub);
}

void loop() {
  if (!is_updated) {
    analogWrite(THR, 0);
    digitalWrite(FWD, LOW);
    digitalWrite(REV, LOW);
  }
  is_updated = false;
  nh.spinOnce();
  delay(10);
}
