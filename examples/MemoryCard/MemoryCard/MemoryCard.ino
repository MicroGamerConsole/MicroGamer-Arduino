#include "MicroGamer.h"
#include <MicroGamerMemoryCard.h>

MicroGamerMemoryCard mem(1);
MicroGamer mg;

void setup() {
  mg.boot();
  mg.enableDoubleBuffer();
  mg.setFrameRate(30);
}

void loop() {
  unsigned int *ptr = (unsigned int *)mem.data();

  if (!mg.nextFrame()) {
    return; // go back to the start of the loop
  }

  mg.clear();
  mg.setCursor(0, 0);
  mg.print("Data: 0x");
  mg.print(*ptr, HEX);
  mg.print("\n");
  mg.print("RAM Addr: 0x");
  mg.print((unsigned int)ptr, HEX);
  mg.print("\n");
  mg.println("Press:");
  mg.println(" - UP to increment");
  mg.println(" - DOWN to decrement");
  mg.println(" - Y to load the data");
  mg.println(" - X to save the data");
  mg.display();

  if (mg.pressed(UP_BUTTON)) {
    (*ptr)++;
  } else if (mg.pressed(DOWN_BUTTON)){
    (*ptr)--;
  }

  if (mg.pressed(Y_BUTTON)) {
    mem.load();
  } else if (mg.pressed(X_BUTTON)){
    mem.save();
  }
}
