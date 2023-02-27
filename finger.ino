

uint8_t uploadFingerpintTemplate(uint16_t id) {
  uint8_t p = uploadModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("upload ");
      // Serial.print(id2);
      // Serial.println(" loaded");
      // Serial.println(p);
      // Serial.read();
      Serial.flush();
      p = finger.storeModel(id);
      if (p == FINGERPRINT_OK) {

        Serial.print("id:");
        Serial.print(id);
        Serial.println("-saved");
        Serial.flush();
      }
      return p;
      // break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_BADPACKET:
      Serial.println("Bad packet");
      return p;
    default:
      Serial.println("Unknown error ");
      Serial.println(p);
      return p;
  }
  Serial.flush();
}



uint8_t getReply(uint8_t packet[], uint16_t timeout) {
  uint8_t reply[20], idx;
  uint16_t timer = 0;

  idx = 0;
#ifdef FINGERPRINT_DEBUG
  Serial.print("<--- ");
#endif
  while (true) {
    // while (!mySerial->available()) {
    while (!Serial1.available()) {
      delay(1);
      timer++;
      if (timer >= timeout) return FINGERPRINT_TIMEOUT;
    }
    // something to read!
    // reply[idx] = mySerial->read();
    reply[idx] = Serial1.read();
#ifdef FINGERPRINT_DEBUG
    Serial.print(" 0x");
    Serial.print(reply[idx], HEX);
#endif
    if ((idx == 0) && (reply[0] != (FINGERPRINT_STARTCODE >> 8)))
      continue;
    idx++;

    // check packet!
    if (idx >= 9) {
      if ((reply[0] != (FINGERPRINT_STARTCODE >> 8)) || (reply[1] != (FINGERPRINT_STARTCODE & 0xFF)))
        return FINGERPRINT_BADPACKET;
      uint8_t packettype = reply[6];
      //Serial.print("Packet type"); Serial.println(packettype);
      uint16_t len = reply[7];
      len <<= 8;
      len |= reply[8];
      len -= 2;
      //Serial.print("Packet len"); Serial.println(len);
      if (idx <= (len + 10)) continue;
      packet[0] = packettype;
      for (uint8_t i = 0; i < len; i++) {
        packet[1 + i] = reply[9 + i];
      }
#ifdef FINGERPRINT_DEBUG
      Serial.println();
#endif
      return len;
    }
  }
}




#define FINGERPRINT_DOWNLOAD 0x09
uint8_t uploadModel(void) {
  uint8_t packet[] = { FINGERPRINT_DOWNLOAD, 0x02 };
  uint8_t packet1[] = { FINGERPRINT_DOWNLOAD, 0x01 };
  uint8_t packet2[] = { 0x3, 0x1, 0x59, 0x1D, 0x0, 0x0, 0xFF, 0x00, 0xFF, 0xFE, 0xF0, 0xFE, 0xE0, 0x6, 0xE0, 0x6, 0x0, 0x6, 0x0, 0x6, 0x0, 0x2, 0x0, 0x2, 0x0, 0x2, 0x0, 0x6, 0x0, 0x6, 0x0, 0x6, 0x8, 0x76 };
  uint8_t packet3[] = { 0x0, 0x6, 0x0, 0xE, 0x0, 0xE, 0x0, 0x1E, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x8E, 0xE2, 0x96, 0x54, 0x19, 0xA4, 0xFE, 0x4, 0xA9 };
  uint8_t packet4[] = { 0x1F, 0xA1, 0xDF, 0x3E, 0x5F, 0x22, 0xA6, 0x7E, 0x68, 0x25, 0xA7, 0xFE, 0xF, 0xB4, 0xDC, 0x9E, 0x2C, 0xBD, 0x19, 0xFE, 0x4C, 0x42, 0x57, 0x1E, 0x26, 0xC2, 0xC2, 0x7E, 0x38, 0x93, 0xE3, 0x7F, 0xF, 0x4D };
  uint8_t packet5[] = { 0x12, 0x9E, 0xDF, 0x1F, 0x15, 0x25, 0x99, 0x7F, 0x44, 0xA8, 0xAC, 0xBF, 0x34, 0x2F, 0x5B, 0x7F, 0x5C, 0xB2, 0x2A, 0x7F, 0x39, 0x40, 0x80, 0xFF, 0x44, 0x19, 0x9F, 0xBC, 0x4E, 0x95, 0xE3, 0x1D, 0xD, 0x6F };
  uint8_t packet6[] = { 0x62, 0x18, 0xc, 0xBD, 0x2D, 0x26, 0x7f, 0x5D, 0x26, 0xA7, 0x45, 0x3D, 0x46, 0x13, 0xF, 0x7A, 0x59, 0x13, 0xCD, 0xFA, 0x57, 0xB8, 0x81, 0xB2, 0x5A, 0x41, 0x16, 0x92, 0x42, 0x16, 0x24, 0x93, 0xB, 0x44 };
  uint8_t packet7[] = { 0x5C, 0x39, 0x17, 0x13, 0x5C, 0x96, 0x23, 0x19, 0x54, 0xC1, 0x2B, 0x53, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xA4 };
  uint8_t packet8[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };
  uint8_t packet9[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };
  uint8_t packet10[] = { 0x3, 0x1, 0x53, 0x17, 0x0, 0x0, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0x1E, 0xFC, 0x2, 0xF8, 0x2, 0xF8, 0x0, 0xF0, 0x0, 0xE0, 0x0, 0xE0, 0x0, 0xE0, 0x0, 0xE0, 0x0, 0xC0, 0x0, 0xC0, 0x2, 0xE, 0x8A };
  uint8_t packet11[] = { 0xC0, 0x2, 0xC0, 0x2, 0xE0, 0x68, 0xF0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x43, 0x8F, 0x63, 0x96, 0x48, 0x13, 0xA5, 0x3E, 0x6, 0x8D };
  uint8_t packet12[] = { 0x6F, 0x24, 0x27, 0x1E, 0x6D, 0x32, 0x2A, 0x9E, 0x1F, 0xB5, 0x1C, 0x9E, 0x58, 0x40, 0x56, 0x5E, 0x2F, 0x22, 0x9F, 0x1F, 0x25, 0x24, 0xC6, 0x9F, 0x56, 0xA9, 0x0, 0x1F, 0x42, 0xAE, 0x9B, 0xFF, 0xB, 0xA2 };
  uint8_t packet13[] = { 0x3F, 0xBC, 0x59, 0x7F, 0x48, 0xC0, 0xC0, 0x7F, 0x32, 0xC1, 0x2, 0x9F, 0x56, 0x9A, 0x66, 0x7D, 0x3D, 0x25, 0xDD, 0x9D, 0x37, 0x27, 0x5, 0x7D, 0x54, 0x17, 0x26, 0x13, 0x6B, 0x16, 0xF, 0xB8, 0xC, 0x4D };
  uint8_t packet14[] = { 0x62, 0x98, 0x23, 0x18, 0x6E, 0x18, 0xA3, 0x99, 0x64, 0x1A, 0x65, 0x99, 0x58, 0x14, 0x4F, 0xB4, 0x36, 0x9B, 0xA0, 0x93, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xA };
  uint8_t packet15[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };
  uint8_t packet16[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };
  uint8_t packet17[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2A };
  Adafruit_Fingerprint_Packet p0 = Adafruit_Fingerprint_Packet(FINGERPRINT_COMMANDPACKET, sizeof(packet), packet);
  Adafruit_Fingerprint_Packet p1 = Adafruit_Fingerprint_Packet(FINGERPRINT_COMMANDPACKET, sizeof(packet1), packet1);
  Adafruit_Fingerprint_Packet p2 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet2), packet2);
  Adafruit_Fingerprint_Packet p3 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet3), packet3);
  Adafruit_Fingerprint_Packet p4 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet4), packet4);
  Adafruit_Fingerprint_Packet p5 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet5), packet5);
  Adafruit_Fingerprint_Packet p6 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet6), packet6);
  Adafruit_Fingerprint_Packet p7 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet7), packet7);
  Adafruit_Fingerprint_Packet p8 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet8), packet8);
  Adafruit_Fingerprint_Packet p9 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet9), packet9);
  Adafruit_Fingerprint_Packet p10 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet10), packet10);
  Adafruit_Fingerprint_Packet p11 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet11), packet11);
  Adafruit_Fingerprint_Packet p12 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet12), packet12);
  Adafruit_Fingerprint_Packet p13 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet13), packet13);
  Adafruit_Fingerprint_Packet p14 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet14), packet14);
  Adafruit_Fingerprint_Packet p15 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet15), packet15);
  Adafruit_Fingerprint_Packet p16 = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet16), packet16);
  Adafruit_Fingerprint_Packet p17 = Adafruit_Fingerprint_Packet(FINGERPRINT_ENDDATAPACKET, sizeof(packet17), packet17);

  finger.writeStructuredPacket(p1);
  delay(10);
  finger.writeStructuredPacket(p2);
  delay(10);
  finger.writeStructuredPacket(p3);
  delay(10);
  finger.writeStructuredPacket(p4);
  delay(10);
  finger.writeStructuredPacket(p5);
  delay(10);
  finger.writeStructuredPacket(p6);
  delay(10);
  finger.writeStructuredPacket(p7);
  delay(10);
  finger.writeStructuredPacket(p8);
  delay(10);
  finger.writeStructuredPacket(p9);
  delay(10);
  finger.writeStructuredPacket(p10);
  delay(10);
  finger.writeStructuredPacket(p11);
  delay(10);
  finger.writeStructuredPacket(p12);
  delay(10);
  finger.writeStructuredPacket(p13);
  delay(10);
  finger.writeStructuredPacket(p14);
  delay(10);
  finger.writeStructuredPacket(p15);
  delay(10);
  finger.writeStructuredPacket(p16);
  delay(10);
  finger.writeStructuredPacket(p17);
  delay(10);

  finger.writeStructuredPacket(p0);
  delay(10);
  finger.writeStructuredPacket(p2);
  delay(10);
  finger.writeStructuredPacket(p3);
  delay(10);
  finger.writeStructuredPacket(p4);
  delay(10);
  finger.writeStructuredPacket(p5);
  delay(10);
  finger.writeStructuredPacket(p6);
  delay(10);
  finger.writeStructuredPacket(p7);
  delay(10);
  finger.writeStructuredPacket(p8);
  delay(10);
  finger.writeStructuredPacket(p9);
  delay(10);
  finger.writeStructuredPacket(p10);
  delay(10);
  finger.writeStructuredPacket(p11);
  delay(10);
  finger.writeStructuredPacket(p12);
  delay(10);
  finger.writeStructuredPacket(p13);
  delay(10);
  finger.writeStructuredPacket(p14);
  delay(10);
  finger.writeStructuredPacket(p15);
  delay(10);
  finger.writeStructuredPacket(p16);
  delay(10);
  finger.writeStructuredPacket(p17);
  delay(10);

  uint8_t len = getReply(packet17, DEFAULTTIMEOUT);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet17[1];
  // return len;
}


uint8_t downloadFingerprintTemplate(uint16_t id) {
  delay(1000);
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #");
  Serial.println(id);
  Serial.flush();

  uint8_t p = finger.loadModel(id);
  Serial.println(id);
  Serial.flush();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return p;
  }
  Serial.flush();

  // OK success!
  delay(100);
  Serial.print("Attempting to get #");
  Serial.println(id);
  Serial.flush();
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return p;
  }
  Serial.flush();
  // one data packet is 267 bytes. in one data packet, 11 bytes are 'usesless' :D
  uint8_t bytesReceived[534];  // 2 data packets
  memset(bytesReceived, 0xff, 534);

  uint32_t starttime = millis();
  int i = 0;
  while (i < 534 && (millis() - starttime) < 20000) {
    if (Serial1.available()) {
      bytesReceived[i++] = Serial1.read();
    }
  }
  Serial.print(i);
  Serial.println(" bytes read.");
  Serial.println("Decoding packet...");

  uint8_t fingerTemplate[512];  // the real template
  memset(fingerTemplate, 0xff, 512);

  // filtering only the data packets
  int uindx = 9, index = 0;
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);  // first 256 bytes
  uindx += 256;                                                // skip data
  uindx += 2;                                                  // skip checksum
  uindx += 9;                                                  // skip next header
  index += 256;                                                // advance pointer
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);  // second 256 bytes

  for (int i = 0; i < 512; ++i) {
    //Serial.print("0x");
    printHex(fingerTemplate[i], 2);
    //Serial.print(", ");
  }
  Serial.println("\ndone.");
  Serial.flush();

  return p;

  /*
    uint8_t templateBuffer[256];
    memset(templateBuffer, 0xff, 256);  //zero out template buffer
    int index=0;
    uint32_t starttime = millis();
    while ((index < 256) && ((millis() - starttime) < 1000))
    {
    if (mySerial.available())
    {
      templateBuffer[index] = mySerial.read();
      index++;
    }
    }
    Serial.print(index); Serial.println(" bytes read");
    //dump entire templateBuffer.  This prints out 16 lines of 16 bytes
    for (int count= 0; count < 16; count++)
    {
    for (int i = 0; i < 16; i++)
    {
      Serial.print("0x");
      Serial.print(templateBuffer[count*16+i], HEX);
      Serial.print(", ");
    }
    Serial.println();
    }*/
}



void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}
