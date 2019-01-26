/**
 * @file MicroGamerCore.h
 * \brief
 * The MicroGamerCore class for MicroGamer hardware initilization and control.
 */

#ifndef MICROGAMER_CORE_H
#define MICROGAMER_CORE_H

#include <Arduino.h>
//#include <avr/power.h>
//#include <avr/sleep.h>
#include <limits.h>



// bit values for button states
// these are determined by the buttonsState() function
#define LEFT_BUTTON (1<<0)  /**< The Left button value for functions requiring a bitmask */
#define RIGHT_BUTTON (1<<1) /**< The Right button value for functions requiring a bitmask */
#define UP_BUTTON (1<<2)    /**< The Up button value for functions requiring a bitmask */
#define DOWN_BUTTON (1<<3)  /**< The Down button value for functions requiring a bitmask */
#define A_BUTTON (1<<4)     /**< The A button value for functions requiring a bitmask */
#define B_BUTTON (1<<5)     /**< The B button value for functions requiring a bitmask */
#define Y_BUTTON (1<<6)     /**< The Y button value for functions requiring a bitmask */
#define X_BUTTON (1<<7)     /**< The X button value for functions requiring a bitmask */

// --------------------

// OLED hardware (SSD1306)

#define OLED_PIXELS_INVERTED 0xA7 // All pixels inverted
#define OLED_PIXELS_NORMAL 0xA6 // All pixels normal

#define OLED_ALL_PIXELS_ON 0xA5 // all pixels on
#define OLED_PIXELS_FROM_RAM 0xA4 // pixels mapped to display RAM contents

#define OLED_VERTICAL_FLIPPED 0xC0 // reversed COM scan direction
#define OLED_VERTICAL_NORMAL 0xC8 // normal COM scan direction

#define OLED_HORIZ_FLIPPED 0xA0 // reversed segment re-map
#define OLED_HORIZ_NORMAL 0xA1 // normal segment re-map

#define SCREEN_RESET_PIN (8)

// -----

#define WIDTH (128) /**< The width of the display in pixels */
#define HEIGHT (64) /**< The height of the display in pixels */

#define COLUMN_ADDRESS_END (WIDTH - 1) & 127   // 128 pixels wide
#define PAGE_ADDRESS_END ((HEIGHT/8)-1) & 7    // 8 pages high

#define BUTTON_A_PIN (5)
#define BUTTON_B_PIN (11)
#define BUTTON_X_PIN (0)
#define BUTTON_Y_PIN (1)
#define BUTTON_UP_PIN (15)
#define BUTTON_DOWN_PIN (14)
#define BUTTON_LEFT_PIN (16)
#define BUTTON_RIGHT_PIN (13)

/** \brief
 * Lower level functions generally dealing directly with the hardware.
 *
 * \details
 * This class is inherited by MicroGamerBase and thus also MicroGamer, so wouldn't
 * normally be used directly by a sketch.
 *
 * \note
 * A friend class named _MicroGamerEx_ is declared by this class. The intention
 * is to allow a sketch to create an _MicroGamerEx_ class which would have access
 * to the private and protected members of the MicroGamerCore class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 */
class MicroGamerCore
{
  friend class MicroGamerEx;

  public:
    MicroGamerCore();

    /** \brief
     * Idle the CPU to save power.
     *
     * \details
     * This puts the CPU in _idle_ sleep mode. You should call this as often
     * as you can for the best power savings. The timer 0 overflow interrupt
     * will wake up the chip every 1ms, so even at 60 FPS a well written
     * app should be able to sleep maybe half the time in between rendering
     * it's own frames.
     */
    void static idle();

    /** \brief
     * Turn the display off.
     *
     * \details
     * The display will clear and be put into a low power mode. This can be
     * used to extend battery life when a game is paused or when a sketch
     * doesn't require anything to be displayed for a relatively long period
     * of time.
     *
     * \see displayOn()
     */
    void static displayOff();

    /** \brief
     * Turn the display on.
     *
     * \details
     * Used to power up and reinitialize the display after calling
     * `displayOff()`.
     *
     * \note
     * The previous call to `displayOff()` will have caused the display's
     * buffer contents to be lost. The display will have to be re-painted,
     * which is usually done by calling `display()`.
     *
     * \see displayOff()
     */
    void static displayOn();

    /** \brief
     * Get the width of the display in pixels.
     *
     * \return The width of the display in pixels.
     *
     * \note
     * In most cases, the defined value `WIDTH` would be better to use instead
     * of this function.
     */
    uint8_t static width();

    /** \brief
     * Get the height of the display in pixels.
     *
     * \return The height of the display in pixels.
     *
     * \note
     * In most cases, the defined value `HEIGHT` would be better to use instead
     * of this function.
     */
    uint8_t static height();

    /** \brief
     * Get the current state of all buttons as a bitmask.
     *
     * \return A bitmask of the state of all the buttons.
     *
     * \details
     * The returned mask contains a bit for each button. For any pressed button,
     * its bit will be 1. For released buttons their associated bits will be 0.
     *
     * The following defined mask values should be used for the buttons:
     *
     * LEFT_BUTTON, RIGHT_BUTTON, UP_BUTTON, DOWN_BUTTON, A_BUTTON, B_BUTTON,
     * Y_BUTTON, X_BUTTON
     */
    uint8_t static buttonsState();

    /** \brief
     * Asynchronously paints an entire image directly to the display from
     * program memory.
     *
     * \param image A byte array in program memory representing the entire
     * contents of the display.
     *
     * \details
     * The contents of the specified array in program memory is written to the
     * display. This is an asynchronous function, which means that the function
     * will return before the buffer is completely sent to the display. Each
     * byte in the array represents a vertical column of 8 pixels with the least
     * significant bit at the top. The bytes are written starting at the top
     * left, progressing horizontally and wrapping at the end of each row, to
     * the bottom right. The size of the array must exactly match the number of
     * pixels in the entire display.
     *
     * \see paintScreenInProgress() waitEndOfPaintScreen()
     */
    void static paintScreen(const uint8_t *image);

    /** \brief
     * Paint screen in progress.
     *
     * \return True if a screen transfer is in progress.
     *
     * \see paintScreen() waitEndOfPaintScreen()
     */
    bool static paintScreenInProgress();

    /** \brief
     * Wait end of paint screen.
     *
     * \see paintScreen()
     */
    void static waitEndOfPaintScreen();

    /** \brief
     * Invert the entire display or set it back to normal.
     *
     * \param inverse `true` will invert the display. `false` will set the
     * display to no-inverted.
     *
     * \details
     * Calling this function with a value of `true` will set the display to
     * inverted mode. A pixel with a value of 0 will be on and a pixel set to 1
     * will be off.
     *
     * Once in inverted mode, the display will remain this way
     * until it is set back to non-inverted mode by calling this function with
     * `false`.
     */
    void static invert(bool inverse);

    /** \brief
     * Turn all display pixels on or display the buffer contents.
     *
     * \param on `true` turns all pixels on. `false` displays the contents
     * of the hardware display buffer.
     *
     * \details
     * Calling this function with a value of `true` will override the contents
     * of the hardware display buffer and turn all pixels on. The contents of
     * the hardware buffer will remain unchanged.
     *
     * Calling this function with a value of `false` will set the normal state
     * of displaying the contents of the hardware display buffer.
     *
     * \note
     * All pixels will be lit even if the display is in inverted mode.
     *
     * \see invert()
     */
    void static allPixelsOn(bool on);

    /** \brief
     * Flip the display vertically or set it back to normal.
     *
     * \param flipped `true` will set vertical flip mode. `false` will set
     * normal vertical orientation.
     *
     * \details
     * Calling this function with a value of `true` will cause the Y coordinate
     * to start at the bottom edge of the display instead of the top,
     * effectively flipping the display vertically.
     *
     * Once in vertical flip mode, it will remain this way until normal
     * vertical mode is set by calling this function with a value of `false`.
     *
     * \see flipHorizontal()
     */
    void static flipVertical(bool flipped);

    /** \brief
     * Flip the display horizontally or set it back to normal.
     *
     * \param flipped `true` will set horizontal flip mode. `false` will set
     * normal horizontal orientation.
     *
     * \details
     * Calling this function with a value of `true` will cause the X coordinate
     * to start at the left edge of the display instead of the right,
     * effectively flipping the display horizontally.
     *
     * Once in horizontal flip mode, it will remain this way until normal
     * horizontal mode is set by calling this function with a value of `false`.
     *
     * \see flipVertical()
     */
    void static flipHorizontal(bool flipped);

    /** \brief
     * Send a single command byte to the display.
     *
     * \param command The command byte to send to the display.
     *
     * \details
     * The display will be set to command mode then the specified command
     * byte will be sent. The display will then be set to data mode.
     * Multi-byte commands can be sent by calling this function multiple times.
     *
     * \note
     * Sending improper commands to the display can place it into invalid or
     * unexpected states, possibly even causing physical damage.
     */
    void static sendLCDCommand(uint8_t command);

    void static sendLCDCommand(uint8_t command,
                               uint8_t command2);

    void static sendLCDCommand(uint8_t command,
                               uint8_t command2,
                               uint8_t command3);

    /** \brief
     * Initialize the MicroGamer's hardware.
     *
     * \details
     * This function initializes the display, buttons, etc.
     *
     * This function is called by begin() so isn't normally called within a
     * sketch. However, in order to free up some code space, by eliminating
     * some of the start up features, it can be called in place of begin().
     * The functions that begin() would call after boot() can then be called
     * to add back in some of the start up features, if desired.
     * See the README file or documentation on the main page for more details.
     *
     * \see MicroGamerBase::begin()
     */
    void static boot();

    /** \brief
     * Delay for the number of milliseconds, specified as a 16 bit value.
     *
     * \param ms The delay in milliseconds.
     *
     * \details
     * This function works the same as the Arduino `delay()` function except
     * the provided value is 16 bits long, so the maximum delay allowed is
     * 65535 milliseconds (about 65.5 seconds). Using this function instead
     * of Arduino `delay()` will save a few bytes of code.
     */
    void static delayShort(uint16_t ms) __attribute__ ((noinline));

  protected:
    // internals
    void static bootOLED();
    void static bootPins();
    void static bootPowerSaving();
    void static bootTWI();

    void static twiBeginTransmission(uint8_t address);
    uint8_t static twiTransmit(const uint8_t data[],
                               size_t quantity);

    void static twiTransmitAsync(const uint8_t data[],
                                    size_t quantity);
    uint8_t static twiTransmit(uint8_t data);
    uint8_t static twiEndTransmission();
};

#endif
