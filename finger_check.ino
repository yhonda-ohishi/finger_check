// #include "stdio.h"
#include <Adafruit_Fingerprint.h>
#include "Adafruit_TinyUSB.h"
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

void setup() {
  Serial.begin(115200);  // The baudrate of Serial monitor is set in 9600
  Serial1.setTX(0);
  Serial1.setRX(1);
  while (!Serial) {
    delay(1);
  };  // Waiting for Serial Monitor
  Serial.println("serial start");
  Serial.flush();
  finger.begin(57600);
  Serial.print("setBaud:");

  //Baud Rateを変えると、finger sensor のシリアルの数値変更が必要
  //Baud Rateは一度替えると、電源が切れても継続するみたいなので、変更したら、通信できなくなるおそれあり
  // uint8_t packet[] = { FINGERPRINT_WRITE_REG, FINGERPRINT_BAUD_REG_ADDR, FINGERPRINT_BAUDRATE_57600 };
  // // writePacket(theAddress, FINGERPRINT_COMMANDPACKET, sizeof(packet) + 2, packet);
  // Adafruit_Fingerprint_Packet pp = Adafruit_Fingerprint_Packet(FINGERPRINT_COMMANDPACKET, sizeof(packet), packet);
  // finger.writeStructuredPacket(pp);

  // finger print check
  if (!finger.verifyPassword()) {
    Serial.println("Did not find fingerprint sensor :(");
    Serial.flush();
    while (!finger.verifyPassword()) {
      delay(1);
    }
  } else {
    Serial.println("Found fingerprint sensor!");
    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Status: 0x"));
    Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x"));
    Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: "));
    Serial.println(finger.capacity);
    Serial.print(F("Security level: "));
    Serial.println(finger.security_level);
    Serial.print(F("Device address: "));
    Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: "));
    Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: "));
    Serial.println(finger.baud_rate);
    // SetBaut_Finger();
  }
  Serial.flush();
  uint8_t p;
  Serial.println();
  Serial.print("template Count:");
  finger.getTemplateCount();
  Serial.println(finger.templateCount);

  Serial.println("empty database");
  finger.emptyDatabase();

  Serial.print("template Count:");
  finger.getTemplateCount();
  Serial.println(finger.templateCount);

  p = uploadFingerpintTemplate(100);
  // delay(2000);
  while (finger.status_reg == 0xA) {
    delay(100);
    finger.getParameters();
  }
  downloadFingerprintTemplate(100);
  // delay(2000);
  // downloadFingerprintTemplate(100);
  Serial.print("template Count:");
  // delay(100);
  // delay(10000);
  while (finger.status_reg != 0x2) {
    delay(100);
    finger.getParameters();
  }
  finger.getTemplateCount();
  // Serial.println(finger.status_reg);
  finger.getTemplateCount();
  finger.getTemplateCount();
  while (finger.status_reg == 0xA) {
    delay(10);
    finger.getParameters();
  }
  // Serial.print("tmp_count:");
  Serial.println(finger.templateCount);
  Serial.flush();
  // finger.template
  Serial.println("test2");
}

String resp(uint8_t p) {
  switch (p) {
    case FINGERPRINT_OK:
      return "FINGERPRINT_OK";
    case FINGERPRINT_PACKETRECIEVEERR:
      return "FINGERPRINT_PACKETRECIEVEERR";
      break;
    case FINGERPRINT_BADPACKET:
      return "FINGERPRINT_BADPACKET";
    default:
      return "Unknown error ";
  }
}

String myString;
// char myString2;
int p;
void loop() {
  // while (true) {
  //   delay(1);
  // }

  if (Serial.available()) {
    myString = Serial.readString();
    Serial.print(myString);
  }
  if (myString.startsWith("enroll:")) {
    myString.replace("enroll:", "");
    Serial.println(myString);
    getFingerprintEnroll(myString.toInt());
    downloadFingerprintTemplate(myString.toInt());
    // enID=cngString.toInt();
  }
  if (myString.startsWith("count:")) {
    // myString.replace("enroll:", "");
    // Serial.println(myString);
    finger.getTemplateCount();
    finger.getParameters();
    while (finger.status_reg == 0xA) {
      delay(10);
      finger.getParameters();
    }
    Serial.println(finger.templateCount);
    myString = "";
    // downloadFingerprintTemplate(myString.toInt());
    // enID=cngString.toInt();
  }
  if (myString.startsWith("store:")) {
    myString.replace("store:", "");
    Serial.print(myString);
    p = finger.storeModel(myString.toInt());
    Serial.println(resp(p));
    Serial.flush();
    myString = "";
    // downloadFingerprintTemplate(myString.toInt());
    // enID=cngString.toInt();
  }
  if (myString.startsWith("enroll_data:")) {
    myString.replace("enroll_data:", "");
    // Serial.println(myString);
    p = enrolldata(myString);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("FINGERPRINT_OK");
        Serial.flush();
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        // return p;
        break;
      case FINGERPRINT_BADPACKET:
        Serial.println("Bad packet");
        // return p;
        break;
      default:
        Serial.println("Unknown error ");
        Serial.println(p);
        break;
        // return p;
    }
    // getFingerprintEnroll(myString.toInt());
    // downloadFingerprintTemplate();
    // enID=cngString.toInt();
  }
  if (myString.startsWith("download:")) {
    myString.replace("download:", "");
    Serial.println(myString);
    // getFingerprintEnroll(myString.toInt());
    downloadFingerprintTemplate(myString.toInt());
    finger.getTemplateCount();
    delay(1000);
    // Serial.print("tmp_count:");
    Serial.println(finger.templateCount);
    // enID=cngString.toInt();
  }
  if(myString==""){
    getFingerprintID();
  }
}