/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

/*
 * Part of this file is based on Adafruit_SSD1306:
 */

/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include "Arduboy2Core.h"
#include <Wire.h>

#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D

#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

const uint8_t PROGMEM lcdBootProgram[] = {
  // boot defaults are commented out but left here in case they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf
  //
  // Display Off
  // 0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  0xA8, 0x3F,

  // Set Display Offset v = 0
  0xD3, 0x00,

  // Set Start Line (0)
  0x40,

  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, 0xCF,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  0xDB, 0x40,

  // Entire Display ON
  0xA4,

  // Set normal/inverse display
  0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,

  // set col address range
  // 0x21, 0x00, COLUMN_ADDRESS_END,

  // set page address range
  // 0x22, 0x00, PAGE_ADDRESS_END

  OLED_HORIZ_FLIPPED, OLED_VERTICAL_FLIPPED // Flip the screen
};


Arduboy2Core::Arduboy2Core() { }

void Arduboy2Core::boot()
{
  bootPins();
  bootOLED();
  // bootPowerSaving();
}

// Pins are set to the proper modes and levels for the specific hardware.
// This routine must be modified if any pins are moved to a different port
void Arduboy2Core::bootPins()
{
  pinMode(BUTTON_A_PIN, INPUT);
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_X_PIN, INPUT_PULLUP);
  pinMode(BUTTON_Y_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
}

void Arduboy2Core::bootOLED()
{
  
  // I2C Init
  Wire.begin();

  //Wire.setClock(250000);
  Wire.setClock(400000);

  // Setup reset pin direction (used by both SPI and I2C)
  pinMode(SCREEN_RESET_PIN, OUTPUT);
  digitalWrite(SCREEN_RESET_PIN, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
  delay(1);
  // bring reset low
  digitalWrite(SCREEN_RESET_PIN, LOW);
  // wait 10ms
  delay(10);
  // bring out of reset
  digitalWrite(SCREEN_RESET_PIN, HIGH);
  // turn on VCC (9V?)

  // run our customized boot-up command sequence against the
  // OLED to initialize it properly for Arduboy
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) {
    sendLCDCommand(pgm_read_byte(lcdBootProgram + i));
  }
}

/* Power Management */

void Arduboy2Core::idle()
{
  // set_sleep_mode(SLEEP_MODE_IDLE);
  // sleep_mode();
}

void Arduboy2Core::bootPowerSaving()
{
  // // disable Two Wire Interface (I2C) and the ADC
  // PRR0 = _BV(PRTWI) | _BV(PRADC);
  // // disable USART1
  // PRR1 = _BV(PRUSART1);
  // // All other bits will be written with 0 so will be enabled
}

// Shut down the display
void Arduboy2Core::displayOff()
{
  sendLCDCommand(0xAE,  // display off
                 0x8D,  // charge pump:
                 0x10); //   disable
  delayShort(250);
}

// Restart the display after a displayOff()
void Arduboy2Core::displayOn()
{
  bootOLED();
}

uint8_t Arduboy2Core::width() { return WIDTH; }

uint8_t Arduboy2Core::height() { return HEIGHT; }


/* Drawing */

// void Arduboy2Core::paint8Pixels(uint8_t pixels)
// {
//   SPItransfer(pixels);
// }

void Arduboy2Core::paintScreen(const uint8_t *image)
{
  sendLCDCommand(SSD1306_COLUMNADDR);
  sendLCDCommand(0);   // Column start address (0 = reset)
  sendLCDCommand(WIDTH-1); // Column end address (127 = reset)

  sendLCDCommand(SSD1306_PAGEADDR);
  sendLCDCommand(0); // Page start address (0 = reset)
  sendLCDCommand(7); // Page end address

  // I2C
  for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
    // send a bunch of data in one xmission
    Wire.beginTransmission(SSD1306_I2C_ADDRESS);
    Wire.write(0x40);
    for (uint8_t x=0; x<16; x++) {
      Wire.write(image[i]);
      i++;
    }
    i--;
    Wire.endTransmission();
  }
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
  sendLCDCommand(SSD1306_COLUMNADDR);
  sendLCDCommand(0);   // Column start address (0 = reset)
  sendLCDCommand(WIDTH-1); // Column end address (127 = reset)

  sendLCDCommand(SSD1306_PAGEADDR);
  sendLCDCommand(0); // Page start address (0 = reset)
  sendLCDCommand(7); // Page end address

  // I2C
  for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
    // send a bunch of data in one xmission
    Wire.beginTransmission(SSD1306_I2C_ADDRESS);
    Wire.write(0x40);
    for (uint8_t x=0; x<16; x++) {
      Wire.write(image[i]);
      if (clear) {
        image[i] = 0;
      }
      i++;
    }
    i--;
    Wire.endTransmission();
  }
}

void Arduboy2Core::blank()
{
  // for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  //   SPItransfer(0x00);
}

void Arduboy2Core::sendLCDCommand(uint8_t command)
{
  // I2C
  uint8_t control = 0x00;   // Co = 0, D/C = 0
  Wire.beginTransmission(SSD1306_I2C_ADDRESS);
  Wire.write(control);
  Wire.write(command);
  Wire.endTransmission();
}

void Arduboy2Core::sendLCDCommand(uint8_t command,
                                  uint8_t command2)
{
  // I2C
  uint8_t control = 0x00;   // Co = 0, D/C = 0
  Wire.beginTransmission(SSD1306_I2C_ADDRESS);
  Wire.write(control);
  Wire.write(command);
  Wire.write(command2);
  Wire.endTransmission();
}

void Arduboy2Core::sendLCDCommand(uint8_t command,
                                  uint8_t command2,
                                  uint8_t command3)
{
  // I2C
  uint8_t control = 0x00;   // Co = 0, D/C = 0
  Wire.beginTransmission(SSD1306_I2C_ADDRESS);
  Wire.write(control);
  Wire.write(command);
  Wire.write(command2);
  Wire.write(command3);
  Wire.endTransmission();
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse)
{
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on)
{
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_NORMAL: OLED_VERTICAL_FLIPPED);
}

// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_NORMAL: OLED_HORIZ_FLIPPED);
}

/* Buttons */

uint8_t Arduboy2Core::buttonsState()
{
  uint8_t buttons = 0;

  if (! digitalRead(BUTTON_LEFT_PIN)) {
      buttons |= LEFT_BUTTON;
  }
  if (! digitalRead(BUTTON_RIGHT_PIN)) {
      buttons |= RIGHT_BUTTON;
  }
  if (! digitalRead(BUTTON_UP_PIN)) {
      buttons |= UP_BUTTON;
  }
  if (! digitalRead(BUTTON_DOWN_PIN)) {
      buttons |= DOWN_BUTTON;
  }
  if (! digitalRead(BUTTON_Y_PIN)) {
      buttons |= A_BUTTON;
  }
  if (! digitalRead(BUTTON_X_PIN)) {
      buttons |= B_BUTTON;
  }

  return buttons;
}

// delay in ms with 16 bit duration
void Arduboy2Core::delayShort(uint16_t ms)
{
  delay((unsigned long) ms);
}

