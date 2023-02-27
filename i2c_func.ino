
void i2c_detect(TwoWire Wir) {


  // // Serial.println("Scanning...");
  byte error, address;  //variable for error and I2C address
  int nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    Wir.beginTransmission(address);
    error = Wir.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
    // // start continuous ranging
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
/**
 * @return uint8_t  return 0 if no_error
*  @brief start with serial print I2C_check ,if no, doubt wire 
 *
 */
uint8_t i2c_check(TwoWire Wir, byte Ad) {
  byte error;
  Serial.print("I2C check");
  Wir.beginTransmission(Ad);
  error = Wir.endTransmission();
  if (error == 0) {
    Serial.print("I2C device found at address 0x");
    if (Ad < 16)
      Serial.print("0");
    Serial.print(Ad, HEX);
    Serial.println("  !");

  } else if (error == 4) {
    Serial.print("Unknown error at address 0x");
    if (Ad < 16)
      Serial.print("0");
    Serial.println(Ad, HEX);
  }
  return error;
  // // start continuous ranging
}