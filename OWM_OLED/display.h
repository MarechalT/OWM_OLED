#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setupDisplay(){
  // initialize with the I2C addr 0x3C (for the 128x32)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Getting the Oled display ready
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
  display.clearDisplay();
}

void displayAllLines(String &line1, String &line2, String &line3, String &line4){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(line1);
    display.print(line2);
    display.print(line3);
    display.print(line4);
    display.display();
}
#endif
