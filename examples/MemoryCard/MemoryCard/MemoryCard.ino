#include "MicroGamer.h"
#include <MicroGamerMemoryCard.h>

MicroGamerMemoryCard mem(1);
MicroGamer mg;

void setup() {
  mg.boot();
  mg.setFrameRate(30);
}

void loop() {
//  unsigned int *ptr = (unsigned int *)mem.data();

  if (!mg.nextFrame()) {
    return; // go back to the start of the loop
  }

  mg.clear();
  mg.drawLine(0, 0, WIDTH - 1, HEIGHT - 1);
  mg.drawLine(WIDTH - 1, 0, 0, HEIGHT - 1);
  mg.setCursor(0, 0);
  mg.println("Data: 0x");
  //mg.print(*ptr, HEX);
  mg.print("\n");
  mg.print("RAM Addr: 0x");
  mg.drawLine(5, 0, WIDTH - 5, HEIGHT - 1);
  mg.drawLine(WIDTH - 5, 0, 5, HEIGHT - 1);
  //mg.print((unsigned int)ptr, HEX);
  mg.print("\n");
  mg.println("Press:");
  mg.println(" - UP to increment");
  mg.println(" - DOWN to decrement");
  mg.println(" - A to load the data");
  mg.println(" - B to save the data");
  mg.display();

//  if (mg.pressed(UP_BUTTON)) {
//    (*ptr)++;
//  } else if (mg.pressed(DOWN_BUTTON)){
//    (*ptr)--;
//  }
//
//  if (mg.pressed(A_BUTTON)) {
//    mem.load();
//  } else if (mg.pressed(B_BUTTON)){
//    mem.save();
//  }
}
