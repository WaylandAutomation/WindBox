# WindBox
Uses a 0-5v output anemometer to create a children's learning toy. As the anemometer spins it fires an interrupt to wake the processor. From there it flashes 6 LEDs in a circular chaser at a speed corresponding to the windspeed. It also displays the windspeed on an SSD1306. If the anemometer is idle for 10 sec, the CPU enters sleep mode until the interrupt is fired again.
This code is written for an Arduino Nano Every with an AVR chip. Other MCUs will need small adjustments in how to implement sleep and interrupts.
