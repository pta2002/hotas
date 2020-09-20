#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int topLevel = 0;
int secondLevel = -1;
int thirdLevel = -1;

char *menus[] = {
  "XPDR",
  "COM",
  "NAV",
  "AP"
};
int menuLength = sizeof(menus) / sizeof(int);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  pinMode(15, INPUT);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(19, INPUT_PULLDOWN);
}

int determineMove(int x) {
  float i = ((float) x + (455.0 / 2.0)) / 455 - 5;
  return round(i);
}

void showMenu() {
  display.setTextColor(SSD1306_WHITE);
  if (secondLevel == -1) {
    topLevel = analogRead(15) / 1024;

    char *menu = menus[topLevel % menuLength];
    char *prev = menus[(topLevel + menuLength - 1) % menuLength];
    char *next = menus[(topLevel + 1) % menuLength];

    int mlen = strlen(menu);
    int plen = strlen(prev);
    int nlen = strlen(next);

    display.setTextSize(2); // 12x16
    display.setCursor(64 - mlen * 12 / 2, 24);
    display.println(menu);

    display.setTextSize(1);
    display.setCursor(16 - plen * 6 / 2, 28);
    display.println(prev);
    display.setCursor(112 - nlen * 6 / 2, 28);
    display.println(next);
  } else {
    char *menu = menus[topLevel % menuLength];

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(menu);
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  }
}

bool backPressed = false, enterPressed = false;
bool backTriggered = false, enterTriggered = false;

void loop() {
  display.clearDisplay();
  int m = determineMove(analogRead(15));

  bool back = digitalRead(18);
  bool enter = digitalRead(19);

  backPressed = false;
  enterPressed = false;

  if (back && !backPressed && !backTriggered) {
    backPressed = true;
    backTriggered = true;
  } else if (!back && backTriggered) {
    backTriggered = false;
  }
  if (enter && !enterPressed && !enterTriggered) {
    enterPressed = true;
    enterTriggered = true;
  } else if (!enter && enterTriggered) {
    enterTriggered = false;
  }

  if (enterPressed) {
    if (secondLevel == -1) {
      secondLevel = 0;
    } else if (thirdLevel == -1) {
      thirdLevel = 0;
    }
  }

  if (backPressed) {
    if (thirdLevel >= 0) {
      thirdLevel = -1;
    } else if (secondLevel >= 0) {
      secondLevel = -1;
    }
  }
  
  showMenu();

  display.display();
}

/*
 * Menus:
 * Right button: enter
 * Left button: leave
 * Knob: speed of change
 */