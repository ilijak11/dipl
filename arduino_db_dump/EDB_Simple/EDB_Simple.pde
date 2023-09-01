/*
 EDB_Simple.pde
 Extended Database Library + Internal Arduino EEPROM Demo Sketch

 The Extended Database library project page is here:
 http://www.arduino.cc/playground/Code/ExtendedDatabaseLibrary

 */
#include "Arduino.h"
#include <EDB.h>

// Use the Internal Arduino EEPROM as storage
#include <EEPROM.h>

// Uncomment the line appropriate for your platform
#define TABLE_SIZE 512 // Arduino 168 or greater

// The number of demo records that should be created.  This should be less
// than (TABLE_SIZE - sizeof(EDB_Header)) / sizeof(LogEvent).  If it is higher,
// operations will return EDB_OUT_OF_RANGE for all records outside the usable range.
#define RECORDS_TO_CREATE 10

// Arbitrary record definition for this table.
// This should be modified to reflect your record needs.
struct UserRecord {
  char id[16];
  byte finger_id;
} userRec;

// The read and write handlers for using the EEPROM Library
void writer(unsigned long address, byte data)
{
  EEPROM.write(address, data);
}

byte reader(unsigned long address)
{
  return EEPROM.read(address);
}

// Create an EDB object with the appropriate write and read handlers
EDB db(&writer, &reader);

void setup()
{
  Serial.begin(9600);
  Serial.println("Extended Database Library + Arduino Internal EEPROM Demo");
  Serial.println();

  db.open(0);

  Serial.print("Record Count: "); Serial.println(db.count());

  for (int recno = 1; recno <= db.count(); recno++)
  {
    db.readRec(recno, EDB_REC userRec);
    Serial.print("ID: "); Serial.println(userRec.id);
    Serial.print("Temp: "); Serial.println(userRec.finger_id);
  }
}

void loop()
{
}
