#include <EEPROM.h>

struct DBHeader{
  byte count;
  unsigned int start_address;
};

DBHeader db_header;

struct UserRecord{
  char id[16];
  byte finger_id;
};

void setup() {
  Serial.begin(9600);
  delay(10);

  // Serial.print("count: ");
  // Serial.println(db_header.count);
  // add_user_record("aaaaaaaaaaaaaaa", 1);
  // Serial.print("count: ");
  // Serial.println(db_header.count);
  // add_user_record("bbbbbbbbbbbbbbb", 2);
  // Serial.print("count: ");
  // Serial.println(db_header.count);
  // add_user_record();
  // Serial.print("count: ");
  // Serial.println(db_header.count);

  read_header();

  for(int i = 0; i<db_header.count; i++){
    read_user_record(i);
  }
  // reser_header();
  // read_header();
  // Serial.print("count: ");
  // Serial.println(db_header.count);
}

void read_header(){
  EEPROM.get(0, db_header);
}

void write_header(){
  EEPROM.put(0, db_header);
}

void add_user_record(const char* id, byte finger_id){
  UserRecord user;
  strncpy(user.id, id, 16);
  user.finger_id = finger_id;
  read_header();
  EEPROM.put(calculate_address(db_header.count), user);
  db_header.count += 1;
  write_header();
}

void reser_header(){
  db_header.count = 0;
  db_header.start_address = 0 + (unsigned int)sizeof(DBHeader);
  write_header();
}

void read_user_record(byte index){
  UserRecord user;
  read_header();
  EEPROM.get(calculate_address(index), user);
  Serial.print("user: ");
  Serial.print(index);
  Serial.print(" id: ");
  Serial.print(user.id);
  Serial.print(" finger_id: ");
  Serial.println(user.finger_id);
}

unsigned int calculate_address(byte index){
  return db_header.start_address + index * (unsigned int)sizeof(UserRecord);
}

void loop() {
}
