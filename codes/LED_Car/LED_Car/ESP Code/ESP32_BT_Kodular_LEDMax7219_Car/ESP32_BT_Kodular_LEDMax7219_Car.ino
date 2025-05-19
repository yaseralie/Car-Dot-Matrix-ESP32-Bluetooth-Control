/*  using ESP32 with bluetooth
    to select your board follow these steps.
    Tools → boards → scroll down to find ESP32 Dev Module → select the port
    Reference GPIO  https://www.botnroll.com/en/esp32/3639-wemos-d1-r32-w-esp32-uno-r3-pinout.html
    Wiring LED Max7219 in ESP32 Dev (Uno Board):
      - CLK : 18
      - CS  : 5
      - DIN : 23
*/
#include <BluetoothSerial.h>

// Pengaturan Bluetooth
BluetoothSerial SerialBT;

//Setup for LED dot Max7219==============================
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8   //I use 8 dot matrix
#define CS_PIN 5
// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//Setup for LED dot Max7219==============================

String default_msg = "Jaga Jarak Aman!";
String msg = default_msg;

//for input/output
int pin_STOP = 26;
int pin_REVERSE = 25;
bool status_stop = false;
bool status_reverse = false;

//This count for keep message until brake on with the number times
int max_count = 5;
int count = 4;

void setup() {
  Serial.begin(9600);
  P.begin();
  SerialBT.begin("ESP32"); // Nama bluetooth
  delay(500);

  //setup pin input
  pinMode(pin_STOP, INPUT_PULLUP);
  pinMode(pin_REVERSE, INPUT_PULLUP);

  msg = "Suzuki Ignis, Engine Start!";
  SerialBT.println(msg);
  Serial.println("Display LED Matrix from Kodular Using Bluetooth");
  delay(2000);
}

void loop() {
  // Read bluetooth
  processIncomingSerial();

  //check button
  int buttonStop = digitalRead(pin_STOP);
  //buttonstop-----------------------------
  if (buttonStop == LOW )
  {
    if (status_stop == false)
    {
      //Show Stop
      status_stop = true;
      if (count < max_count) {
        count = count + 1;
        delay(100);
        Serial.println(count);
      }
      else {
        msg = "BRAKE ON!";
        SerialBT.println(msg);
        Serial.println("STOP detected");
        delay(100);
      }
    }
  }
  else if (buttonStop == HIGH)
  {
    if (status_stop == true)
    {
      status_stop = false;
      status_reverse = false;
      if (count == max_count) {
        msg = default_msg;
        SerialBT.println(msg);
      }
    }
  }
  //buttonstop-----------------------------

  int buttonReverse = digitalRead(pin_REVERSE);
  //buttonreverse-----------------------------
  if (buttonReverse == LOW )
  {
    if (status_reverse == false)
    {
      //Show Reverse
      status_reverse = true;
      msg = "REVERSE!";
      SerialBT.println(msg);
      Serial.println("REVERSE detected");
      delay(100);
    }
  }
  else if (buttonReverse == HIGH)
  {
    if (status_reverse == true)
    {
      status_reverse = false;
      msg = default_msg;
      SerialBT.println(msg);
    }
  }
  //buttonreverse-----------------------------

  //Show Text in LED Matrix
  if (msg.length() < 10) {
    P.setTextAlignment(PA_CENTER);
    P.print(msg.c_str());
  }
  else {
    if (P.displayAnimate())
      P.displayText(msg.c_str(), PA_LEFT, 70, 70, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }
}

// INCOMING SERIAL DATA PROCESSING CODE----------------------------------------
void processIncomingSerial()
{
  if (SerialBT.available()) {
    // Read message from Bluetooth
    String str_serial = SerialBT.readStringUntil('\n');
    // Set default message with previous message
    if (str_serial == "Set_Default") {
      default_msg = msg;
    }
    else if (str_serial == "Reset") {
      default_msg = "Jaga Jarak Aman!";
    }
    else {
      msg = str_serial;
      //Start counting
      count = 0;
      Serial.println(msg);
    }

  }
}
