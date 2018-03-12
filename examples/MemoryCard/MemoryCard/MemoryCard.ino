#include "Arduboy2.h"
#include <MicroGamerMemoryCard.h>

MicroGamerMemoryCard mem(1);
Arduboy2 mg;

void setup() {
  mg.boot();
}

void loop() {
  unsigned int *ptr =(unsigned int *)mem.data();

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
  mg.println(" - A to load the data");
  mg.println(" - B to save the data");
  mg.display();

  if (mg.pressed(UP_BUTTON)) {
    (*ptr)++;
  } else if (mg.pressed(DOWN_BUTTON)){
    (*ptr)--;
  }

  if (mg.pressed(A_BUTTON)) {
    mem.load();
  } else if (mg.pressed(B_BUTTON)){
    mem.save();
  }
}
