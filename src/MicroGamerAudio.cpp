/**
 * @file MicroGamerAudio.cpp
 * \brief
 * The MicroGamerAudio class for speaker and sound control.
 */

#include "MicroGamer.h"
#include "MicroGamerAudio.h"

bool MicroGamerAudio::audio_enabled = true;

void MicroGamerAudio::on()
{
  audio_enabled = true;
}

void MicroGamerAudio::off()
{
  audio_enabled = false;
}

void MicroGamerAudio::toggle()
{
  if (audio_enabled)
    off();
  else
    on();
}

void MicroGamerAudio::saveOnOff()
{
}

void MicroGamerAudio::begin()
{
    on();
}

bool MicroGamerAudio::enabled()
{
  return audio_enabled;
}
