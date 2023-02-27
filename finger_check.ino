#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_MLX90614.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#include <misakiUTF16.h>

#include <Adafruit_Fingerprint.h>
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define SCREEN_WIDTH 128  // OLED 幅指定（メイン、サブ共通）
#define SCREEN_HEIGHT 64  // OLED 高さ指定（メイン）
#define OLED_RESET -1     // リセット端子（未使用-1）
#define SCREEN_HEIGHT 64  // OLED 高さ指定（メイン）

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);  //メイン
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

uint8_t font[8];  // フォント格納バッファ
// '無題', 128x64px

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
byte disp_st, disp1_st;

void setup() {
  Wire1.setSCL(3);
  Wire1.setSDA(2);     //I2C0はWireオブジェクトを使用します。
  Wire1.begin();       // Wire communication begin
  Serial.begin(115200);  // The baudrate of Serial monitor is set in 9600
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial2.setTX(4);
  Serial2.setRX(5);
  while (!Serial) {
    delay(1);
  };  // Waiting for Serial Monitor
  Serial.print("serial start");
  Serial.flush();
  // set the data rate for the sensor serial port
  // Serial1.begin(9600);
  finger.begin(57600);
  // Serial2.begin(9600);

  // finger print check
  if (!finger.verifyPassword()) {
    Serial.println("Did not find fingerprint sensor :(");
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
  
  p=uploadFingerpintTemplate(100);
  downloadFingerprintTemplate(100);
  Serial.print("template Count:");
  finger.getTemplateCount();
  Serial.println(finger.templateCount);
  Serial.flush();
  // finger.template
  Serial.println("test2");


}


void loop() {
  while(true){
    delay(1);
  }
}