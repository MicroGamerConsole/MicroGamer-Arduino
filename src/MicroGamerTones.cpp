/**
 * @file MicroGamerTones.cpp
 * \brief An Arduino library for playing tones and tone sequences, 
 * intended for the MicroGamer game system.
 */

/*****************************************************************************
  MicroGamerTones

An Arduino library to play tones and tone sequences.

Specifically written for use by the MicroGamer miniature game system
https://www.arduboy.com/
but could work with other Arduino AVR boards that have 16 bit timer 3
available, by changing the port and bit definintions for the pin(s)
if necessary.

Copyright (c) 2017 Scott Allen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*****************************************************************************/

#include "MicroGamerTones.h"

// pointer to a function that indicates if sound is enabled
static bool (*outputEnabled)();

static volatile long durationCount = 0;
static volatile bool tonesPlaying = false;
static volatile bool toneSilent;
#ifdef TONES_VOLUME_CONTROL
static volatile bool toneHighVol;
static volatile bool forceHighVol = false;
static volatile bool forceNormVol = false;
#endif

static volatile uint16_t *tonesStart;
static volatile uint16_t *tonesIndex;
static volatile uint16_t toneSequence[MAX_TONES * 2 + 1];
static volatile bool inProgmem;

#define AUDIO_TIMER_PRESCALER 5
#define AUDIO_PIN 2

MicroGamerTones::MicroGamerTones(boolean (*outEn)())
{
  outputEnabled = outEn;

  toneSequence[MAX_TONES * 2] = TONES_END;

  pinMode(AUDIO_PIN, OUTPUT);

  NRF_TIMER2->MODE = (NRF_TIMER2->MODE & ~TIMER_MODE_MODE_Msk) | ((TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);
  
  NRF_TIMER2->BITMODE = (NRF_TIMER2->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) | ((TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos) & TIMER_BITMODE_BITMODE_Msk);

  NRF_TIMER2->PRESCALER = (NRF_TIMER2->PRESCALER & ~TIMER_PRESCALER_PRESCALER_Msk) | ((AUDIO_TIMER_PRESCALER << TIMER_PRESCALER_PRESCALER_Pos) & TIMER_PRESCALER_PRESCALER_Msk);

  NRF_TIMER2->SHORTS = (NRF_TIMER2->SHORTS & ~TIMER_SHORTS_COMPARE0_CLEAR_Msk) | ((TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos) & TIMER_SHORTS_COMPARE0_CLEAR_Msk);

  // //  Connect Timer1 compare 0 event to GPIOTE Out 0 task
  // NRF_PPI->CH[0].EEP = (uint32_t)&(NRF_TIMER2->EVENTS_COMPARE[0]);
  // NRF_PPI->CH[0].TEP = (uint32_t)&(NRF_GPIOTE->TASKS_OUT[0]);
    
  // //  disable PPI channel 0 
  // NRF_PPI->CHENSET = 1;

  // NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos)
  //     | (AUDIO_PIN << GPIOTE_CONFIG_PSEL_Pos)
  //     | (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos)
  //     | (GPIOTE_CONFIG_OUTINIT_Low  << GPIOTE_CONFIG_OUTINIT_Pos);

  NVIC_ClearPendingIRQ(TIMER2_IRQn);
  NVIC_EnableIRQ(TIMER2_IRQn);

  NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
}

void MicroGamerTones::tone(uint16_t freq, uint16_t dur)
{
  stopTimer();

  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq;
  toneSequence[1] = dur;
  toneSequence[2] = TONES_END; // set end marker
  nextTone(); // start playing
}

void MicroGamerTones::tone(uint16_t freq1, uint16_t dur1,
                        uint16_t freq2, uint16_t dur2)
{
  stopTimer();

  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = TONES_END; // set end marker
  nextTone(); // start playing
}

void MicroGamerTones::tone(uint16_t freq1, uint16_t dur1,
                        uint16_t freq2, uint16_t dur2,
                        uint16_t freq3, uint16_t dur3)
{
  stopTimer();

  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = freq3;
  toneSequence[5] = dur3;
  // end marker was set in the constructor and will never change
  nextTone(); // start playing
}

void MicroGamerTones::tones(const uint16_t *tones)
{
  stopTimer();

  inProgmem = true;
  tonesStart = tonesIndex = (uint16_t *)tones; // set to start of sequence array
  nextTone(); // start playing
}

void MicroGamerTones::tonesInRAM(uint16_t *tones)
{
  stopTimer();

  inProgmem = false;
  tonesStart = tonesIndex = tones; // set to start of sequence array
  nextTone(); // start playing
}

void MicroGamerTones::noTone()
{
  stopTimer();
  tonesPlaying = false;
}

void MicroGamerTones::volumeMode(uint8_t mode)
{
  //  There's no volume mode on the Micro:Gamer
}

bool MicroGamerTones::playing()
{
  return tonesPlaying;
}

void MicroGamerTones::nextTone()
{
  uint16_t freq;
  uint16_t dur;
  long     Count;

  freq = getNext(); // get tone frequency

  if (freq == TONES_END) { // if freq is actually an "end of sequence" marker
    noTone(); // stop playing
    return;
  }

  tonesPlaying = true;

  if (freq == TONES_REPEAT) { // if frequency is actually a "repeat" marker
    tonesIndex = tonesStart; // reset to start of sequence
    freq = getNext();
  }

  freq &= ~TONE_HIGH_VOLUME; // strip volume indicator from frequency

  if (freq == 0) { // if tone is silent
    toneSilent = true;
  }
  else {
    toneSilent = false;
  }

  if (!outputEnabled()) { // if sound has been muted
    toneSilent = true;
  }


  if (toneSilent) {
      //  When there's no tone the timer still needs to run the timer to be able
      //  to count duration. We choose an arbitrary value for the frequency of
      //  the timer in that case.
      freq = 100;
  }

  dur = getNext(); // get tone duration

  if (dur != 0) {
    // A right shift is used to divide by 512 for efficiency.
    // For durations in milliseconds it should actually be a divide by 500,
    // so durations will by shorter by 2.34% of what is specified.
    Count = ((long)dur * freq) >> 9;
  }
  else {
    Count = -1; // indicate infinite duration
  }

  durationCount = Count;

  NRF_TIMER2->TASKS_CLEAR = 0x1UL;
  NRF_TIMER2->CC[0] = (16000000 / (1 << AUDIO_TIMER_PRESCALER)) / (freq * 2) ;
  startTimer();
}

uint16_t MicroGamerTones::getNext()
{
  if (inProgmem) {
    return pgm_read_word(tonesIndex++);
  }
  return *tonesIndex++;
}

void MicroGamerTones::stopTimer()
{
    NRF_TIMER2->TASKS_STOP = 0x1UL;    
}

void MicroGamerTones::startTimer()
{
    NRF_TIMER2->TASKS_START = 0x1UL;    
}

extern "C" {

void TIMER2_IRQHandler(void)
{
  NRF_TIMER2->EVENTS_COMPARE[0] = 0;

  NVIC_ClearPendingIRQ(TIMER2_IRQn);

  if (durationCount != 0) {
      if (!toneSilent) {
          digitalWrite(AUDIO_PIN, !digitalRead(AUDIO_PIN));
      } else {
          digitalWrite(AUDIO_PIN, LOW);
      }
  } else {
      digitalWrite(AUDIO_PIN, LOW);
  }

  if (durationCount > 0) {
    durationCount--;
  }
  else {
    MicroGamerTones::nextTone();
  }
}

}
