/*
  GymPass - RFID Security System
  
  Reads RFID tags from a RS-232/Serial reader connected to Pin 0 (RX)
  and pipes the ID to a Rails based web-server IP as read from the SD
  card in config.txt. The server will must be running the GymPass site
  to authenticate the read RFID tag. If authenticated, a log entry is 
  written on the SD card's log(date).txt and a connected relay on pin
  2 will be charged.
*/

void setup() {
  
}

void loop() {
  
}
