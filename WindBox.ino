/* The anemometer I was using did not produce enough signal to trigger an interrupt at less than hurricane-force winds. 
    This code is set up to use an NPN transistor with the base on the 'windPut' pin to drop the interrupt pin enough to
    trigger.  If your instrument will trigger the interrupt, you can just make the interrupt pin the same as the input pin. 
    I left some debug prints in the code commented out if you need to make changes to run on a different MCU. Written for
    an ATMega4809 on an Arduino Nano Every. 
    Nate Hubbard
    Wayland Automation, 2022  
 
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <avr/sleep.h>
#include <LibPrintf.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define windPut 15

const int leds[6] = {2,3,4,5,6,7};
//int velocityMap=0;
const int chaseTune = 75;

void fastWords(String message) {      //Prints a string to the display, does not clear display until the next time
  display.clearDisplay();                 //it's called. No Delay.
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(10, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(message);
  display.display();
  }

void Words(String message) {          //Prints a string to the display, holds it there for 1.5 sec and clears it.
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(10, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(message);
  display.display();

  delay(1500);
  display.clearDisplay();
  display.display();  
  }

void Interrupt() {        //ISR to wake up the MCU when the anemometer turns
  sleep_disable();

  //goto main;  
}

void Blink(int loops) {     //Blink onboard LED 'loops' times in a lopsided blink. Debug without prints
  for (int i=0; i<loops; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
      }
    }

void setup() {
pinMode(windPut, INPUT);
pinMode(LED_BUILTIN, OUTPUT);   //setup chaser lights
  for (int i=0; i<6; i++) {
      pinMode(leds[i], OUTPUT);
  }
pinMode(16, INPUT_PULLUP);  //setup interrupt pin

Serial.begin(115200);
while (!Serial){
    printf("Waiting on Serial Connection\n");
  }
printf("Serial Connected\n");

sleep_enable();           //Setup sleepmode from library, could also use 'IDLE'
set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  printf("Sleep Mode Eabled\n");

display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);  //initialize display
display.clearDisplay();
display.display();

attachInterrupt(digitalPinToInterrupt(16), Interrupt, FALLING);   //init interrupt to wake MCU

Blink(5);
 printf("Setup Complete\n");

}

//main:
void loop() {
cli();    //No Interrupts or the chasers might be unpredictable
static int timeout = millis();

  int velocity = analogRead(windPut); 
  String velocityMap = String(map(velocity, 0, 1023, 0, 136));
  fastWords("Windspeed    " + velocityMap +" Knots");
  //delay(200); printf(velocity);

  for (int j=0; j<6; j++) {
      digitalWrite(leds[j], HIGH);
      delay((1/velocity)*chaseTune);   //delay inverse to input: higher volts => smaller delay/faster chaser lights
      digitalWrite(leds[j], LOW);
  }
sei();    //Set interrupts or MCU will not wake up from Sleep Mode
if (millis()-timeout >9999 && velocity <= 10) {
    timeout = millis();
   // delay(200); printf("Nighty night\n");
    Words("Night, Night");
    Blink(2);
    sleep_enable();
    sleep_cpu();
  }
}
