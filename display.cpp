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

void displayClear() {
    display.clearDisplay();
    display.display();
}
