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
  // set the data rate for the sensor serial port
  // Serial1.begin(9600);
  // finger.begin(57600);
  finger.begin(9600);
  // Serial2.begin(9600);

  // finger print check
  if (!finger.verifyPassword()) {
    Serial.println("Did not find fingerprint sensor :(");
    Serial.flush();
    while(!finger.verifyPassword()){
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
  delay(2000);
  downloadFingerprintTemplate(100);
  delay(2000);
  downloadFingerprintTemplate(100);
  Serial.print("template Count:");
  finger.getTemplateCount();
  delay(100);
  // Serial.print("tmp_count:");
  Serial.println(finger.templateCount);
  Serial.flush();
  // finger.template
  Serial.println("test2");
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
    Serial.println(myString);
  }
  if (myString.startsWith("enroll:")) {
    myString.replace("enroll:", "");
    Serial.println(myString);
    getFingerprintEnroll(myString.toInt());
    downloadFingerprintTemplate(myString.toInt());
    // enID=cngString.toInt();
  }
  if (myString.startsWith("enroll_data:")) {
    myString.replace("enroll_data:", "");
    // Serial.println(myString);
    p = enrolldata(myString);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("upload ");
        // Serial.print(id2);
        // Serial.println(" loaded");
        Serial.println(p);
        // Serial.read();
        Serial.flush();
        delay(100);
        p = finger.storeModel(10);
        // Serial.flush();
        Serial.println(p);
        if (p == FINGERPRINT_OK) {

          Serial.print("id:");
          Serial.print(101);
          Serial.println("-saved");
          Serial.flush();
          // downloadFingerprintTemplate(100);
        }
        // return p;
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
}