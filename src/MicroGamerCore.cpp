/**
 * @file MicroGamerCore.cpp
 * \brief
 * The MicroGamerCore class for MicroGamer hardware initilization and control.
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

#include "MicroGamerCore.h"
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

// Two Wire Interface

#define TWI_DEVICE  (NRF_TWI1)
#define TWI_PIN_SDA (20)
#define TWI_PIN_SCL (19)
#define TWI_IRQn (SPI1_TWI1_IRQn)

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

volatile bool twiInProgress = false;
const uint8_t *twiTxData = NULL;
size_t twiByteToSend = 0;

MicroGamerCore::MicroGamerCore()
{
  twiInProgress = false;
}

void MicroGamerCore::boot()
{
  bootPins();
  bootTWI();
  bootOLED();
  bootPowerSaving();
}

// Pins are set to the proper modes and levels for the specific hardware.
// This routine must be modified if any pins are moved to a different port
void MicroGamerCore::bootPins()
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

void MicroGamerCore::bootOLED()
{
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
  // OLED to initialize it properly for MicroGamer
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) {
    sendLCDCommand(pgm_read_byte(lcdBootProgram + i));
  }
}

void MicroGamerCore::bootTWI()
{
  TWI_DEVICE->EVENTS_TXDSENT = 0;
  TWI_DEVICE->EVENTS_STOPPED = 0;
  TWI_DEVICE->EVENTS_RXDREADY = 0;
  TWI_DEVICE->EVENTS_ERROR = 0;

  TWI_DEVICE->INTENSET =
        TWI_INTENSET_TXDSENT_Set  << TWI_INTENSET_TXDSENT_Pos  |
        TWI_INTENSET_STOPPED_Set  << TWI_INTENSET_STOPPED_Pos  |
        TWI_INTENSET_ERROR_Set    << TWI_INTENSET_ERROR_Pos    |
        TWI_INTENSET_RXDREADY_Set << TWI_INTENSET_RXDREADY_Pos;

  Wire.begin();
  Wire.setClock(400000);
}

/* TWI  */

void MicroGamerCore::twiBeginTransmission(uint8_t address)
{
  TWI_DEVICE->ADDRESS = address;
  TWI_DEVICE->SHORTS = 0x0UL;
  TWI_DEVICE->TASKS_RESUME = 0x1UL;
  TWI_DEVICE->TASKS_STARTTX = 0x1UL;
}

uint8_t MicroGamerCore::twiTransmit(const uint8_t data[],
                                  size_t quantity)
{
  for(size_t i = 0; i < quantity; ++i)
  {
    TWI_DEVICE->TXD = data[i];

    while(!TWI_DEVICE->EVENTS_TXDSENT && !TWI_DEVICE->EVENTS_ERROR);

    if (TWI_DEVICE->EVENTS_ERROR)
    {
      break;
    }

    TWI_DEVICE->EVENTS_TXDSENT = 0x0UL;
  }

  if (TWI_DEVICE->EVENTS_ERROR)
  {
    TWI_DEVICE->EVENTS_ERROR = 0x0UL;

    uint32_t error = TWI_DEVICE->ERRORSRC;

    TWI_DEVICE->ERRORSRC = error;

    if (error == TWI_ERRORSRC_ANACK_Msk)
    {
      return 2;
    }
    else if (error == TWI_ERRORSRC_DNACK_Msk)
    {
      return 3;
    }
    else
    {
      return 4;
    }
  }

  return 0;
}

void MicroGamerCore::twiTransmitAsync(const uint8_t data[],
                                       size_t quantity)
{

  if (quantity == 0) {
    return;
  }

  if (quantity > 1 ) {
    twiInProgress = true;
    twiTxData = data + 1;
    twiByteToSend = quantity - 1;

    NVIC_ClearPendingIRQ(TWI_IRQn);
    NVIC_EnableIRQ(TWI_IRQn);
  }

  TWI_DEVICE->TXD = data[0];
}

uint8_t MicroGamerCore::twiTransmit(uint8_t data)
{
    twiTransmit(&data, 1);
}

uint8_t MicroGamerCore::twiEndTransmission()
{
  TWI_DEVICE->TASKS_STOP = 0x1UL;
  while(!TWI_DEVICE->EVENTS_STOPPED);
  TWI_DEVICE->EVENTS_STOPPED = 0x0UL;

  if (TWI_DEVICE->EVENTS_ERROR)
  {
    TWI_DEVICE->EVENTS_ERROR = 0x0UL;

    uint32_t error = TWI_DEVICE->ERRORSRC;

    TWI_DEVICE->ERRORSRC = error;

    if (error == TWI_ERRORSRC_ANACK_Msk)
    {
      return 2;
    }
    else if (error == TWI_ERRORSRC_DNACK_Msk)
    {
      return 3;
    }
    else
    {
      return 4;
    }
  }

  return 0;
}

extern "C" {

void SPI1_TWI1_IRQHandler(void)
{

  if(TWI_DEVICE->EVENTS_TXDSENT)
  {
    TWI_DEVICE->EVENTS_TXDSENT = 0;
    if(twiByteToSend != 0)
    {
      TWI_DEVICE->TXD = *twiTxData++;
      twiByteToSend--;
    }
    else
    {
      TWI_DEVICE->TASKS_STOP = 1;
    }
  }

  if(TWI_DEVICE->EVENTS_STOPPED)
  {
    TWI_DEVICE->EVENTS_STOPPED = 0;
    twiInProgress = false;
    NVIC_DisableIRQ(TWI_IRQn);
  }

  if(TWI_DEVICE->EVENTS_RXDREADY)
  {
    TWI_DEVICE->EVENTS_RXDREADY = 0;
  }

  if(TWI_DEVICE->EVENTS_ERROR)
  {
    TWI_DEVICE->EVENTS_ERROR = 0;
    twiInProgress = false;
    NVIC_DisableIRQ(TWI_IRQn);
  }
}

}

/* Power Management */

void MicroGamerCore::idle()
{
  // set_sleep_mode(SLEEP_MODE_IDLE);
  // sleep_mode();
}

void MicroGamerCore::bootPowerSaving()
{
  // // disable Two Wire Interface (I2C) and the ADC
  // PRR0 = _BV(PRTWI) | _BV(PRADC);
  // // disable USART1
  // PRR1 = _BV(PRUSART1);
  // // All other bits will be written with 0 so will be enabled
}

// Shut down the display
void MicroGamerCore::displayOff()
{
  sendLCDCommand(0xAE,  // display off
                 0x8D,  // charge pump:
                 0x10); //   disable
  delayShort(250);
}

// Restart the display after a displayOff()
void MicroGamerCore::displayOn()
{
  bootOLED();
}

uint8_t MicroGamerCore::width() { return WIDTH; }

uint8_t MicroGamerCore::height() { return HEIGHT; }


/* Drawing */

void MicroGamerCore::paintScreen(const uint8_t *image)
{
  waitEndOfPaintScreen();

  sendLCDCommand(SSD1306_COLUMNADDR);
  sendLCDCommand(0);   // Column start address (0 = reset)
  sendLCDCommand(WIDTH-1); // Column end address (127 = reset)

  sendLCDCommand(SSD1306_PAGEADDR);
  sendLCDCommand(0); // Page start address (0 = reset)
  sendLCDCommand(7); // Page end address

  twiBeginTransmission(SSD1306_I2C_ADDRESS);
  twiTransmit(0x40);
  twiTransmitAsync(image, SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8);
}

bool MicroGamerCore::paintScreenInProgress()
{
    return twiInProgress;
}

void MicroGamerCore::waitEndOfPaintScreen()
{
  while (twiInProgress) {
    idle();
  }
}

void MicroGamerCore::sendLCDCommand(uint8_t command)
{
  uint8_t data[2] = {0x00,  // Co = 0, D/C = 0
                     command};
  twiBeginTransmission(SSD1306_I2C_ADDRESS);
  twiTransmit(data, 2);
  twiEndTransmission();
}

void MicroGamerCore::sendLCDCommand(uint8_t command,
                                  uint8_t command2)
{
  uint8_t data[3] = {0x00,  // Co = 0, D/C = 0
                     command,
                     command2};
  twiBeginTransmission(SSD1306_I2C_ADDRESS);
  twiTransmit(data, 3);
  twiEndTransmission();
}

void MicroGamerCore::sendLCDCommand(uint8_t command,
                                  uint8_t command2,
                                  uint8_t command3)
{
  uint8_t data[4] = {0x00,  // Co = 0, D/C = 0
                     command,
                     command2,
                     command3};
  twiBeginTransmission(SSD1306_I2C_ADDRESS);
  twiTransmit(data, 4);
  twiEndTransmission();
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void MicroGamerCore::invert(bool inverse)
{
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void MicroGamerCore::allPixelsOn(bool on)
{
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void MicroGamerCore::flipVertical(bool flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_NORMAL: OLED_VERTICAL_FLIPPED);
}

// flip the display horizontally or set to normal
void MicroGamerCore::flipHorizontal(bool flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_NORMAL: OLED_HORIZ_FLIPPED);
}

/* Buttons */

uint8_t MicroGamerCore::buttonsState()
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
  if (! digitalRead(BUTTON_A_PIN)) {
      buttons |= A_BUTTON;
  }
  if (! digitalRead(BUTTON_B_PIN)) {
      buttons |= B_BUTTON;
  }
  if (! digitalRead(BUTTON_Y_PIN)) {
      buttons |= Y_BUTTON;
  }
  if (! digitalRead(BUTTON_X_PIN)) {
      buttons |= X_BUTTON;
  }

  return buttons;
}

// delay in ms with 16 bit duration
void MicroGamerCore::delayShort(uint16_t ms)
{
  delay((unsigned long) ms);
}
