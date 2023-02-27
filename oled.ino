



void drawBitmapEx(Adafruit_SSD1306 disp,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color,uint16_t bgcolor, uint16_t ex) {
  int16_t i, j,b=(w+7)/8;
  for( j = 0; j < h; j++) {
    for(i = 0; i < w; i++ ) { 
      if(*(bitmap + j*b + i / 8) & (128 >> (i & 7))) {
        // ドットあり
        if (ex == 1)  disp.drawPixel(x+i, y+j, color); //1倍
        else          disp.fillRect(x + i * ex, y + j * ex, ex, ex, color); // ex倍
      } else {
        // ドットなし
        if (ex == 1)  disp.drawPixel(x+i, y+j, bgcolor);
        else          disp.fillRect(x + i * ex, y + j * ex, ex, ex, bgcolor);
       }
     }
   }
}

void jp_disp(Adafruit_SSD1306 disp, char *str) {


  disp.clearDisplay();

  // "あ"の描画
  int16_t ex = 2;  // 文字倍率
  // getFontData(font, "あ");              // "あ"のフォントを取得
  // drawBitmapEx(0, 0, font,8,8,1,0,ex);  // フレームバッファにフォント書き込み
  // disp.display();                       // 画面更新

  // 文字列の表示
  // char *str="Abcあいうえお、埼玉 "      // 文字列
  //           "日本語表示できるかな？ ";
  char *ptr = str;             // 文字列参照位置
  uint16_t x = 0, y = 0 * ex;  // 表示位置

  while (*ptr) {                         // 文字列分ループ
    ptr = getFontData(font, ptr, true);  // 1文字分のフォント取得
    if (!ptr)
      break;  // エラーの場合は終

    drawBitmapEx(disp,x, y, font, 8, 8, 1, 0, ex);  // フレームバッファにフォント書き込み

    if (128 - 8 * ex > x) {  // 表示位置更新
      x += 8 * ex;
    } else {
      x = 0;
      y += 8 * ex;
    }
  }
  disp.display();
}