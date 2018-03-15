/**
 * @file Arduboy2Audio.cpp
 * \brief
 * The Arduboy2Audio class for speaker and sound control.
 */

#include "Arduboy2.h"
#include "Arduboy2Audio.h"

bool Arduboy2Audio::audio_enabled = true;

void Arduboy2Audio::on()
{
  audio_enabled = true;
}

void Arduboy2Audio::off()
{
  audio_enabled = false;
}

void Arduboy2Audio::toggle()
{
  if (audio_enabled)
    off();
  else
    on();
}

void Arduboy2Audio::saveOnOff()
{
}

void Arduboy2Audio::begin()
{
    on();
}

bool Arduboy2Audio::enabled()
{
  return audio_enabled;
}
