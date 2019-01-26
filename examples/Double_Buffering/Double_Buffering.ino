#include <MicroGamer.h>

MicroGamer mg;

void setup() {
  mg.boot();
  mg.clear();
}

void loop() {
  mg.setCursor(0, 0);
  if ( ! mg.doubleBuffer()) {
    mg.println("Some text that");
    mg.println("glitches");
    mg.println("because double buffer");
    mg.println("is not enabled");
    mg.println("==============="); 
  
  } else {
    mg.println("Some text that is now");
    mg.println("clean because");
    mg.println("double buffer is");
    mg.println("enabled");
    mg.println("==============="); 
  }

  mg.display();

  delay(7 + rand() % 5);

  // If double buffering is not enabled, the clear operation will modify the
  // frame buffer while it is sent to the screen. This will create glitches.
  // With double buffering enabled, drawing oprations will be preformed on
  // buffer A while buffer B is sent to the screen, which means there will be
  // no glitch.
  mg.clear();

  if (mg.pressed(Y_BUTTON)) {
    mg.enableDoubleBuffer();
  }
}
