#include <Arduino.h>
#include <OneButton.h>
#include <singleLEDLibrary.h>
#include <SimpleCLI.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

#include "AddressBook.hpp"

SoftwareSerial gsmSerial(8, 9);
#include "gsmConfig.h"

#define PIN_HEARTBEAT_LED 13
#define PIN_USER_LED 3
#define PIN_CMD_RELAY 4
#define PIN_BTN_USER 2

sllib led(PIN_USER_LED);
sllib heartbeatLed(PIN_HEARTBEAT_LED);

GSM gsmAccess;
GSMVoiceCall vcs;

SimpleCLI cli;
AddressBook addressBook;

enum {
  SYSTEM_IDLE,
  SYSTEM_LEARNING_PHONE_NUMBERS
} systemState;

OneButton btn = OneButton(
    PIN_BTN_USER,    // Input pin for the button
    true, // Button is active LOW
    true  // Enable internal pull-up resistor
);

static void handleClick()
{
  Serial.println("Clicked!");

  led.setOffSingle();
  systemState = SYSTEM_IDLE;
}

static void handleDoubleClick()
{
  Serial.println("double!");
}

// this function will be called when the button started long pressed.
void LongPressStart(void *oneButton)
{
  Serial.print(((OneButton *)oneButton)->getPressedMs());
  Serial.println("\t - LongPressStart()");
}

// this function will be called when the button is released.
void LongPressStop(void *oneButton)
{
  Serial.print(((OneButton *)oneButton)->getPressedMs());
  Serial.println("\t - LongPressStop()\n");
}

// this function will be called when the button is held down.
void DuringLongPress(void *oneButton)
{
  Serial.print(((OneButton *)oneButton)->getPressedMs());
  Serial.println("\t - DuringLongPress()");

  unsigned long pressedMs = ((OneButton *)oneButton)->getPressedMs();
  if (pressedMs > 5000)
  {
    led.setOnSingle();
    systemState = SYSTEM_LEARNING_PHONE_NUMBERS;
  }
}

void cliCbRelayToggle(cmd *c)
{
  // relay.ge
  Serial.println("TBI");
}

void cliCbNrList(cmd *c)
{
  Serial.print("Total: ");
  Serial.println(addressBook.count());

  for (size_t i = 0; i < addressBook.MaxNumbers; i++)
  {
    String number = addressBook.getNumber(i);
    Serial.print("  ");
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println(number);
  }
}

void cliCbEraseList(cmd *c)
{
  Serial.print("Erasing...");
  addressBook.erase();
  Serial.println("done");
}

void cliCbAddList(cmd *c)
{
  Command cmd(c);
  int index = cmd.getArgument("index").getValue().toInt();
  String number = cmd.getArgument("number").getValue();

  if (addressBook.positionIsEmpty(index) == false)
  {
    Serial.println("Position is full");
    return;
  }

  addressBook.addNumber(index, number);
  Serial.println("OK");
}

void cliCbAddNextList(cmd *c)
{
  Command cmd(c);
  String number = cmd.getArgument("number").getValue();

  if (addressBook.addNumber(number))
  {
    Serial.println("OK");
  }
  else
  {
    Serial.println("ERROR");
  }
}

void cliCbRemoveList(cmd *c)
{
  Command cmd(c);
  int index = cmd.getArgument("index").getValue().toInt();

  if (addressBook.positionIsEmpty(index))
  {
    Serial.println("Position is empty");
    return;
  }

  addressBook.deleteNumber(index);
  Serial.println("OK");
}

void cliCbCheckList(cmd *c)
{
  Command cmd(c);
  String number = cmd.getArgument("number").getValue();

  if (addressBook.isNumberAllowed(number))
  {
    Serial.println("YES");
  }
  else
  {
    Serial.println("NO");
  }
}

void helpCallback(cmd *c)
{
  Command cmd(c); // Create wrapper object

  Serial.println(cli.toString());
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Booting...");

  
  Serial.print("Building cli...");
  cli.addCmd("help", helpCallback);
  cli.addCmd("relayToggle", cliCbRelayToggle).setDescription("bau");
  cli.addCmd("nrList", cliCbNrList).setDescription("List phone numbers");
  cli.addCmd("nrErase", cliCbEraseList).setDescription("Erase all phone numbers");
  {
    Command cmd = cli.addCmd("nrAdd", cliCbAddList);
    cmd.addArg("index");
    cmd.addArg("number");
    cmd.setDescription("Add a phone number");
  }
  {
    Command cmd = cli.addCmd("nrAddNext", cliCbAddNextList);
    cmd.addArg("number");
    cmd.setDescription("Add a phone number");
  }
  {
    Command cmd = cli.addCmd("nrRemove", cliCbRemoveList);
    cmd.addArg("index");
    cmd.setDescription("Add a phone number");
  }
  {
    Command cmd = cli.addCmd("nrCheck", cliCbCheckList);
    cmd.addArg("number");
    cmd.setDescription("Check a phone number");
  }
  Serial.println("Done");
  Serial.print("Init buttons...");

  btn.attachClick(handleClick);
  btn.attachDoubleClick(handleDoubleClick);

  btn.attachLongPressStart(LongPressStart, &btn);
  btn.attachDuringLongPress(DuringLongPress, &btn);
  btn.attachLongPressStop(LongPressStop, &btn);

  btn.setLongPressIntervalMs(1000);

  Serial.println("Done");
  Serial.print("Init leds...");

  heartbeatLed.setBlinkSingle(500);

  Serial.println("Done");
  Serial.print("Init commands...");
  pinMode(PIN_CMD_RELAY, OUTPUT);
  Serial.println("Done");
  Serial.print("Init GSM...");
#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  // connection state
  bool connected = false;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected)
  {
    if (gsmAccess.begin(9600, NULL, false) == GSM_READY)
    {
      connected = true;
    }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  // This makes sure the modem correctly reports incoming events
  vcs.hangCall();
  Serial.println("Done");

  // gsmSerial.begin(9600);
  wdt_enable(WDTO_2S); 
  systemState = SYSTEM_IDLE;
  Serial.println("Boot completed");
}

void commandGate()
{
  digitalWrite(PIN_CMD_RELAY, HIGH);
  delay(500);
  digitalWrite(PIN_CMD_RELAY, LOW);
}

void onCallRequestAccess(String &callingNumber)
{
  if(addressBook.isNumberAllowed(callingNumber))
  {
    Serial.print("Allow acces for ");
    Serial.println(callingNumber);
    
    commandGate();
  }
  else
  {
    Serial.print("Access denied for ");
    Serial.println(callingNumber);
  }
}

void onCallRequestRegister(String &callingNumber)
{
  if (addressBook.addNumber(callingNumber))
  {
    Serial.print("New phone number added to address book ");
    Serial.println(callingNumber);
  }
  else
  {
    Serial.print("Failed to add phone number to address book ");
    Serial.println(callingNumber);
  }
}

void onCall(String &callingNumber)
{
  if(systemState == SYSTEM_LEARNING_PHONE_NUMBERS)
  {
    onCallRequestRegister(callingNumber);
  }
  else
  {
    onCallRequestAccess(callingNumber);
  }
}

void loop()
{
  btn.tick();

  led.update();
  heartbeatLed.update();

  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    Serial.print("# ");
    Serial.println(input);
    cli.parse(input);
  }
  // if (Serial.available())
  //   gsmSerial.write(Serial.read());
  // if (gsmSerial.available())
  //   Serial.write(gsmSerial.read());



  switch (vcs.getvoiceCallStatus())
  {
    case IDLE_CALL: // Nothing is happening

      break;

    case RECEIVINGCALL: // Yes! Someone is calling us
    {
      Serial.println("RECEIVING CALL");
      
      char numtel[20];
      vcs.retrieveCallingNumber(numtel, 20);
      String numberCalling(numtel);
      Serial.print("Number from modem:");
      Serial.println(numberCalling);

      vcs.hangCall();
      if(strnlen(numtel, sizeof(numtel)) == 0)
        break;
        
      if(numberCalling.startsWith("+") == false)
      {
        numberCalling = String("+4") + numberCalling;
      }
      numberCalling.trim();
      onCall(numberCalling);
    }
      break;

    default:
      break;
  }
  wdt_reset(); 
}
