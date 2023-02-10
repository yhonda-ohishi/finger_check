#include <Adafruit_MLX90614.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define SCREEN_WIDTH 128  // OLED 幅指定（メイン、サブ共通）
#define SCREEN_HEIGHT 64  // OLED 高さ指定（メイン）
#define OLED_RESET -1     // リセット端子（未使用-1）
#define SCREEN_HEIGHT 64  // OLED 高さ指定（メイン）

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //メイン
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define str "test";

void setup() {
  Wire.setSDA(0);  //I2C0はWireオブジェクトを使用します。
  Wire.setSCL(1);
  Wire.begin();        // Wire communication begin
  Serial.begin(9600);  // The baudrate of Serial monitor is set in 9600
    ;  // Waiting for Serial Monitor
  Serial.println("\nI2C Scanner");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306:0 allocation failed"));
    // for (;;)
    ;  //エラーなら無限ループ
  } else {
    Serial.println(F("SSD1306:0 OK"));
  }
  // OLED1初期表示
  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("SSD1306:1 allocation failed"));
    // for (;;)
    ;  //エラーなら無限ループ
  } else {
    Serial.println(F("SSD1306:1 OK"));
  }
  if (!lox.begin(41, &Wire)) {
    Serial.println(F("Failed to boot VL53L0X"));
      ;
  } else {
    Serial.println("VL53L0X OK");
  }
  if (!mlx.begin(90, &Wire)) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
      ;
  } else {
    Serial.println("MLX Sensor OK");
  }
  display.clearDisplay();               //表示クリア
  display.setTextSize(2);               //文字サイズ
  display.setTextColor(SSD1306_WHITE);  //文字色
  display.setCursor(0, 0);              //表示開始位置左上角（X,Y）
  display.println("test");              //表示内容
  display.display();                    //表示実行

  // OLED表示（サブ画面）
  display1.clearDisplay();               //表示クリア
  display1.setTextSize(2);               //文字サイズ
  display1.setTextColor(SSD1306_WHITE);  //文字色
  display1.setCursor(0, 0);              //表示開始位置左上角（X,Y）
  display1.println("test");              //表示内容
  display1.display();                    //表示実行


  Serial.println("Scanning...");
  byte error, address;  //variable for error and I2C address

  int nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

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
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  // start continuous ranging
  lox.startRangeContinuous();
}

void loop() {
  double obj, amb;

  amb = mlx.readAmbientTempC();
  obj = mlx.readObjectTempC();

  display.clearDisplay();  //表示クリア
  display.setTextSize(2);  //文字サイズ
  // display.setTextColor(SSD1306_WHITE);  //文字色
  display.setCursor(0, 0);   //表示開始位置左上角（X,Y）
  display.print("amb ");     //表示内容
  display.println(amb);      //表示内容
  display.setCursor(0, 15);  //表示開始位置左上角（X,Y）
  display.print("obj ");     //表示内容
  display.println(obj);      //表示内容
  display.display();

  if (lox.isRangeComplete()) {
    display1.clearDisplay();  //表示クリア
    display1.setTextSize(2);  //文字サイズ
    // display1.setTextColor(SSD1306_WHITE);  //文字色
    display1.setCursor(0, 0);   //表示開始位置左上角（X,Y）
    display1.print("dis ");     //表示内容
    display1.println(lox.readRange());      //表示内容
    display1.display();

    // Serial.print("Distance in mm: ");
    // Serial.println(lox.readRange());
  }
}