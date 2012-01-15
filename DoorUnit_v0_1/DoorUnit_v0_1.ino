/*

  GymPass - RFID Security System
  by Nathan Manzi <nathan@nmanzi.com>
  
  version 0.0.1:
  - initial version.
  - checks db on SD card for recv. RFID tag, charges RELAY_PIN if found for a while
  
  Tested with Arduino Uno
  Requires Ethernet Shield
  
  Reads RFID tags from a RS-232/Serial reader connected to Pin 0 (RX)
  and searches the TAG_DATABASE on attached SD card for a matching entry.
  If found, it pushes RELAY_PIN high for UNLOCK_TIME milliseconds.
  
  DB entry data structure:
  [ 1 byte bool (enabled/disabled) ][ 10 byte ascii (card ID) ]
  
  If first byte is 0x00 (false) then ID is skipped.
  
*/

#include <SD.h>

// Here's our constants.
//
// I LOVE YOU PENNY
#define TAG_ENABLED 0x01
#define LOG_FILENAME "LOG.TXT"
#define TAG_DATABASE "TAGS.DB"
#define UNLOCK_TIME 3000

#define STX 0x02
#define ETX 0x03
#define CR 0x0D
#define LF 0x0A

#define RELAY_PIN 2

// These are our functions.
void setup();
void loop();
boolean seekRFID(char *id);
void unlockDoor();
void crashAndBurn();

// This is our data.
File openFile;
char rfidTagID[11] = { 0 };
byte currentByte = 0;
int bytesRead = 0;
char currentID[11] = { 0 };

void setup() {
  Serial.begin(9600);
  
  if (!SD.begin(4)) {
    Serial.println("SD FAIL (-_-)");
    crashAndBurn();
  }
  
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    currentByte = Serial.read();
    if (currentByte == STX) {
      bytesRead = 0;
      while (bytesRead < 10) {
        if (Serial.available() > 0) {
          rfidTagID[bytesRead] = Serial.read();
          bytesRead++;
        }
      }
      seekRFID(rfidTagID);
    }
    // Flush the rest, it's crap.
    Serial.flush();
  }
}

boolean seekRFID(char *id) {
  openFile = SD.open(TAG_DATABASE, FILE_READ);
  if (openFile) {
    Serial.print("Scanning for tag ");
    Serial.println(id);
    boolean foundID = false;
    while (!foundID) {
      if (openFile.available()) {
        currentByte = openFile.read();
        if (currentByte == 0x01) {
          for (int i = 0; i < 10; i++) {
            currentID[i] = openFile.read();
          }
          // Serial.println(thisID); // DEBUG
          if (strcmp(currentID, id) == 0) foundID = true;
        } else {
          int filePos = openFile.position() + 10;
          if (!openFile.seek(filePos)) crashAndBurn();
        }
      } else {
        break;
      }
    }
    Serial.print("ID ");
    if (foundID) {
      Serial.println("Found");
      unlockDoor(UNLOCK_TIME);
      foundID = false;
    }
    else Serial.println("Not Found");
  }
  openFile.close();
}

void unlockDoor(int delaySecs) {
  digitalWrite(RELAY_PIN, HIGH);
  delay(delaySecs);
  digitalWrite(RELAY_PIN, LOW);
}

void crashAndBurn() {
  // Why are you here?
  // Because you made a mistake.
  for(;;)
    ;
}
