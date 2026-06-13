#include "display.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool displayInit() {
    Wire.begin(OLED_SDA, OLED_SCL);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("Error: pantalla OLED no encontrada");
        return false;
    }

    display.clearDisplay();
    display.display();
    return true;
}

void displayText(const char* text, int x, int y, uint8_t size) {
    display.clearDisplay();
    display.setTextSize(size);
    display.setTextColor(WHITE);
    display.setCursor(x, y);
    display.println(text);
    display.display();
}

void displayShowLines(const String& line1, const String& line2, const String& line3, const String& line4) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    if (line1.length() > 0) {
        display.setCursor(0, 0);
        display.println(line1);
    }
    if (line2.length() > 0) {
        display.setCursor(0, 12);
        display.println(line2);
    }
    if (line3.length() > 0) {
        display.setCursor(0, 24);
        display.println(line3);
    }
    if (line4.length() > 0) {
        display.setCursor(0, 36);
        display.println(line4);
    }

    display.display();
}

void displayClear() {
    display.clearDisplay();
    display.display();
}
