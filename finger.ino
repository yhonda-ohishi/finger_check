
uint8_t uploadFingerpintTemplate(uint16_t id) {
  uint8_t p = uploadModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("upload ");
      // Serial.print(id2);
      // Serial.println(" loaded");
      Serial.println(p);
      // Serial.read();
      Serial.flush();
      p = finger.storeModel(id);
      if (p == FINGERPRINT_OK) {

        Serial.print("id:");
        Serial.print(id);
        Serial.println("-saved");
        Serial.flush();
        finger.getTemplateCount();

        Serial.print("tmp:count:");
        Serial.println(finger.templateCount);
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


int HexAtoi(String st) {
  int ret = 0;
  if (st.length() == 2) {
    if (st[0] >= 'A' && st[0] <= 'F') {
      ret += (st[0] - 'A' + 10) * 16;
    } else if (st[0] >= '0' && st[0] <= '9') {
      ret += (st[0] - '0') * 16;
    }
    if (st[1] >= 'A' && st[1] <= 'F') {
      ret += (st[1] - 'A' + 10);
    } else if (st[1] >= '0' && st[1] <= '9') {
      ret += (st[1] - '0');
    }
    //
  }
  return ret;
}


String getmodelstring() {
  String ret = "";
  uint8_t delete_st;
  while (Serial1.available()) {
    delete_st = Serial1.read();
  }
  // delete=
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.print("Template ");
      // // Serial.print(id);
      // Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return "";
  }
  Serial.flush();
  // one data packet is 267 bytes. in one data packet, 11 bytes are 'usesless' :D


  uint8_t templateBuffer[688];
  memset(templateBuffer, 0xffff, 688);  //initialize template buffer
  Serial1.flush();
  int index = 0;
  uint32_t starttime = millis();
  while ((index < 688) && ((millis() - starttime) < 1000)) {
    if (Serial1.available() > 0) {
      templateBuffer[index] = Serial1.read();
      index++;
    }
  }
  Serial1.flush();

  // Serial.print(index);
  // Serial.println(" bytes read");

  //dump entire templateBuffer.
  index = 0;
  // Serial.print("DData:");
  for (int ii = 0; ii < 4; ii++) {

    index += 9;
    for (int i = 0; i < 128; i++) {
      ret += StrHex(templateBuffer[index++], 2);
    }
    index += 2;
  }
  ret + '\n';
  // Serial.flush();
  return ret;
  // Serial.p("\ndone.");
}




#define FINGERPRINT_DOWNLOAD 0x09
uint8_t enrolldata(String st) {
  uint8_t packet1[] = { FINGERPRINT_DOWNLOAD, 0x01 };
  Adafruit_Fingerprint_Packet p1 = Adafruit_Fingerprint_Packet(FINGERPRINT_COMMANDPACKET, sizeof(packet1), packet1);
  finger.writeStructuredPacket(p1);
  delay(10);
  uint8_t packet[32];
  uint8_t packet17[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2A };
  Adafruit_Fingerprint_Packet p17 = Adafruit_Fingerprint_Packet(FINGERPRINT_ENDDATAPACKET, sizeof(packet17), packet17);
  for (int ii = 0; ii < 16; ii++) {
    for (int i = 0; i < 32; i++) {
      packet[i] = 0;
      packet[i] = HexAtoi(st.substring(i * 2 + ii * 2 * 32, i * 2 + 2 + ii * 2 * 32));
      printHex(packet[i], 2);
      // Serial.println();
    }
    Adafruit_Fingerprint_Packet pp = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet), packet);
    finger.writeStructuredPacket(pp);
    delay(10);
  }
  finger.writeStructuredPacket(p17);
  delay(10);
  Serial.println();
  packet1[1] = 0x02;
  Serial.println(packet1[0], packet1[1]);
  p1 = Adafruit_Fingerprint_Packet(FINGERPRINT_COMMANDPACKET, sizeof(packet1), packet1);
  finger.writeStructuredPacket(p1);
  delay(10);
  for (int ii = 0; ii < 16; ii++) {
    for (int i = 0; i < 32; i++) {
      packet[i] = 0;
      packet[i] = HexAtoi(st.substring(i * 2 + ii * 2 * 32, i * 2 + 2 + ii * 2 * 32));
      printHex(packet[i], 2);
    }
    Adafruit_Fingerprint_Packet pp = Adafruit_Fingerprint_Packet(FINGERPRINT_DATAPACKET, sizeof(packet), packet);
    finger.writeStructuredPacket(pp);
    delay(10);
  }

  finger.writeStructuredPacket(p17);
  delay(10);

  uint8_t len = getReply(packet17, DEFAULTTIMEOUT);
  Serial.print("len:");
  Serial.println(len);

  Serial.print("strcmp:");
  st.trim();
  switch (strcmp(st.c_str(), getmodelstring().c_str())) {
    case 0:
      Serial.println("Data loaded");
      break;
    default:
      Serial.println("Data load error");
      break;
  }

  finger.getParameters();
  while (finger.status_reg == 0xA) {
    delay(10);
    finger.getParameters();
  }
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet17[1];
}

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
  uint8_t packet15[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };  //len =34
  uint8_t packet16[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24 };
  uint8_t packet17[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2A };
  // uint8_t packet17[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20 };
  // uint8_t packet17[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
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
  for (int i = 0; i < 34; i++) printHex(packet2[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet3[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet4[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet5[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet6[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet7[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet8[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet9[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet10[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet11[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet12[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet13[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet14[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet15[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet16[i], 2);
  for (int i = 0; i < 34; i++) printHex(packet17[i], 2);
  Serial.println();
  Serial.println('f');
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
  Serial.println();
  uint8_t len = getReply(packet17, DEFAULTTIMEOUT);
  Serial.print("len:");
  Serial.println(len);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet17[1];
  // return len;
}


uint8_t downloadFingerprintTemplate(uint16_t id) {
  delay(100);
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #");
  Serial.println(id);
  Serial.flush();

  finger.getParameters();
  while (finger.status_reg == 0xA) {
    delay(10);
    finger.getParameters();
  }
  Serial.print("status:");
  Serial.println(finger.status_reg, HEX);
  delay(10);

  uint8_t p = finger.loadModel(id);
  delay(10);
  Serial.print("id:");
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
  uint8_t delete_flush;
  while (Serial1.available() > 0) delete_flush = Serial1.read();

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


  uint8_t templateBuffer[688];
  memset(templateBuffer, 0xffff, 688);  //initialize template buffer
  Serial1.flush();
  int index = 0;
  uint32_t starttime = millis();
  while ((index < 688) && ((millis() - starttime) < 1000)) {
    if (Serial1.available() > 0) {
      templateBuffer[index] = Serial1.read();
      index++;
    }
  }
  Serial1.flush();

  Serial.print(index);
  Serial.println(" bytes read");

  //dump entire templateBuffer.
  index = 0;
  Serial.print("DData:");
  for (int ii = 0; ii < 4; ii++) {

    index += 9;
    for (int i = 0; i < 128; i++) {
      printHex(templateBuffer[index++], 2);
    }
    index += 2;
  }
  Serial.println("\ndone.");
  Serial.flush();

  return p;
}


uint16_t getFingerprintEnroll(uint16_t id) {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  Serial.print("SOUND:putfinger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      Serial.println("SOUND:finger_invalid");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      Serial.println("SOUND:finger_invalid");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  Serial.println("SOUND:1");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  Serial.println("SOUND:SameFinger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        Serial.println("SOUND:OK");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("SOUND:finger_invalid");
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("SOUND:finger_invalid");
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    Serial.println("SOUND:2");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("SOUND:finger_notMatch");
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      // Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}



void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}
String StrHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  return tmp;
}
