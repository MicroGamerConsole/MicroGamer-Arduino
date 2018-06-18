/**
 * @file MicroGamerAudio.h
 * \brief
 * The MicroGamerAudio class for speaker and sound control.
 */

#ifndef MICROGAMER_AUDIO_H
#define MICROGAMER_AUDIO_H

#include <Arduino.h>
//#include <EEPROM.h>

/** \brief
 * Provide speaker and sound control.
 *
 * \details
 * This class provides functions to initialize the speaker and control the
 * enabling and disabling (muting) of sound. It doesn't provide any functions
 * to actually produce sound.
 *
 * The state of sound muting is stored in system EEPROM and so is retained
 * over power cycles.
 *
 * An MicroGamerAudio class object named `audio` will be created by the
 * MicroGamerBase class, so there is no need for a sketch itself to create an
 * MicroGamerAudio object. MicroGamerAudio functions can be called using the
 * MicroGamer or MicroGamerBase `audio` object.
 *
 * Example:
 *
 * \code
 * #include <MicroGamer.h>
 *
 * MicroGamer arduboy;
 *
 * // MicroGamerAudio functions can be called as follows:
 *   arduboy.audio.on();
 *   arduboy.audio.off();
 * \endcode
 *
 * \note
 * \parblock
 * In order for this class to be fully functional, the external library or
 * functions used by a sketch to actually to produce sounds should be compliant
 * with this class. This means they should only produce sound if it is enabled,
 * or mute the sound if it's disabled. The `enabled()` function can be used
 * to determine if sound is enabled or muted. Generally a compliant library
 * would accept the `enabled()` function as an initialization parameter and
 * then call it as necessary to determine the current state.
 *
 * For example, the ArduboyTones and ArduboyPlaytune libraries require an
 * `enabled()` type function to be passed as a parameter in the constructor,
 * like so:
 *
 * \code
 * #include <MicroGamer.h>
 * #include <ArduboyTones.h>
 *
 * MicroGamer arduboy;
 * ArduboyTones sound(arduboy.audio.enabled);
 * \endcode
 * \endparblock
 *
 * \note
 * \parblock
 * A friend class named _MicroGamerEx_ is declared by this class. The intention
 * is to allow a sketch to create an _MicroGamerEx_ class which would have access
 * to the private and protected members of the MicroGamerAudio class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 * \endparblock
 */
class MicroGamerAudio
{
 friend class MicroGamerEx;

 public:
  /** \brief
   * Initialize the speaker based on the current mute setting.
   *
   * \details
   * The speaker is initialized based on the current mute setting saved in
   * system EEPROM. This function is called by `MicroGamerBase::begin()` so it
   * isn't normally required to call it within a sketch. However, if
   * `MicroGamerCore::boot()` is used instead of `MicroGamerBase::begin()` and the
   * sketch includes sound, then this function should be called after `boot()`.
   */
  void static begin();

  /** \brief
   * Turn sound on.
   *
   * \details
   * The system is configured to generate sound. This function sets the sound
   * mode only until the unit is powered off. To save the current mode use
   * `saveOnOff()`.
   *
   * \see off() toggle() saveOnOff()
   */
  void static on();

  /** \brief
   * Turn sound off (mute).
   *
   * \details
   * The system is configured to not produce sound (mute). This function sets
   * the sound mode only until the unit is powered off. To save the current
   * mode use `saveOnOff()`.
   *
   * \see on() toggle() saveOnOff()
   */
  void static off();

  /** \brief
   * Toggle the sound on/off state.
   *
   * \details
   * If the system is configured for sound on, it will be changed to sound off
   * (mute). If sound is off, it will be changed to on. This function sets
   * the sound mode only until the unit is powered off. To save the current
   * mode use `saveOnOff()`.
   *
   * \see on() off() saveOnOff()
   */
  void static toggle();

  /** \brief
   * Save the current sound state in EEPROM.
   *
   * \details
   * The current sound state, set by `on()` or `off()`, is saved to the
   * reserved system area in EEPROM. This allows the state to carry over between
   * power cycles and after uploading a different sketch.
   *
   * \note
   * EEPROM is limited in the number of times it can be written to. Sketches
   * should not continuously change and then save the state rapidly.
   *
   * \see on() off() toggle()
   */
  void static saveOnOff();

  /** \brief
   * Get the current sound state.
   *
   * \return `true` if sound is currently enabled (not muted).
   *
   * \details
   * This function should be used by code that actually generates sound.
   * If `true` is returned, sound can be produced. If `false` is returned,
   * sound should be muted.
   *
   * \see on() off() toggle()
   */
  bool static enabled();

 protected:
  bool static audio_enabled;
};

#endif
