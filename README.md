# Micro:Gamer Arduino Library

[![Gitter chat](https://badges.gitter.im/gitterHQ/gitter.png)](https://gitter.im/MicroGamerConsole/Lobby)

The Micro:Gamer Arduino library is maintained in a git repository hosted on [GitHub](https://github.com/) at:

https://github.com/MicroGamerConsole/MicroGamer-Arduino

The Micro:Gamer Arduino library is a fork of the [Arduboy2 library](), which provides a standard *application programming interface* (API) to the display, buttons and other hardware of the Micro:Gamer miniature game system.

## Library documentation

Comments in the library header files are formatted for the [Doxygen](http://www.doxygen.org) document generation system. The HTML files generated using the configuration file _extras/Doxyfile_ can be found at:

https://MLXXXp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/index.html

A generated PDF file can be found at:

https://MLXXXp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/pdf/Arduboy2.pdf

## Installation

The Arduboy2 library can be installed using the Arduino IDE Library Manager:

- In the Arduino IDE select from the menus: `Sketch > Include Library > Manage Libraries...`
- In the Library Manager *Filter your search...* field enter *arduboy2*.
- Click somewhere within the Arduboy2 entry.
- Click on the *Install* button.

For more library installation information see

[Installing Additional Arduino Libraries - Using the Library Manager](https://www.arduino.cc/en/Guide/Libraries#toc3)

## Start up features

The *begin()* function, used to initialize the library, includes features that are intended to be available to all sketches using the library (unless the sketch developer has chosen to disable one or more of them to free up some code space):

## Using the library in a sketch

As with most libraries, to use MicroGamer in your sketch you must include its header file at the start:

```cpp
#include <MicroGamer.h>
```

You must then create an MicroGamer class object:

```cpp
MicroGamer mg;
```

To initialize the library, you must call its *begin()* function. This is usually done at the start of the sketch's *setup()* function:

```cpp
void setup()
{
  mg.begin();
  // more setup code follows, if required
}
```

The rest of the MicroGamer functions will now be available for use.

If you wish to use the Sprites class functions you must create a Sprites object:

```cpp
Sprites sprites;
```

Sample sketches have been included with the library as examples of how to use it. To load an example, for examination and uploading to the Arduboy, using the Arduino IDE menus select:

`File > Examples > MicroGamer`

### Persistant data storage

There is no EEPROM on the micro:bit board so the library uses the flash memory inside the micro-controller for persistant storage.

The interface to use the flash as persistent storage is provided by the MicroGamerMemoryCard class. This class uses two different memory areas:

 - The first one is a 1k bytes page in the flash memory, this is where the data will be stored permanently. The reason for a fixed size of 1k bytes is because flash memory have to be erased/written by pages of 1k.

 - The second memory area is the temporary RAM buffer. This is where the program will read/write the data before saving it permanently in the flash page. Since there is not a lot of RAM available, the program can decide to have a temporary RAM buffer that is smaller than 1k.

Here is an example of how to use it in your sketch:

```cpp
#include <MicroGamerMemoryCard.h>

// Create a memory card of one 32bit word
MicroGamerMemoryCard mem(1);

// Load the content of the flash page to the temporary RAM buffer
mem.load();

// Read a value from the temporary RAM buffer
if (mem.read(0) != 42) {

  // Write a value to the temporary RAM buffer
  mem.write(0, 42);

  // Permanently save the RAM buffer into flash memory
  mem.save();
}
```

### Audio control functions

The library includes an MicroGamerAudio class. This class provides functions to enable and disable (mute) sound. It doesn't contain anything to actually produce sound.

The MicroGamerBase class, and thus the MircoGamer class, creates an MicroGamerAudio class object named *audio*, so a sketch doesn't need to create its own MicroGamerAudio object.

Example:

```cpp
#include <MicroGamer.h>

MicroGamer mg;

// MicroGamerAudio functions can be called as follows:
  mg.audio.on();
  mg.audio.off();
```

### Ways to make more code space available to sketches

#### Remove the text functions

If your sketch doesn't use any of the functions for displaying text, such as *setCursor()* and *print()*, you can remove them. You could do this if your sketch generates whatever text it requires by some other means. Removing the text functions frees up code by not including the font table and some code that is always pulled in by inheriting the [Arduino *Print* class](http://playground.arduino.cc/Code/Printclass).

To eliminate text capability in your sketch, when creating the library object simply use the *MicroGamerBase* class instead of *MicroGamer*:

For example, if the object will be named *mg*:

Replace

```cpp
MicroGamer mg;
```

with

```cpp
MicroGamerBase mg;
```
