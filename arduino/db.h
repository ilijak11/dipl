#define DB_HEADER_ADDRESS 0
#define DB_MAX_USER_RECORDS 128
#define DB_FAIL 1
#define DB_SUCCESS 0

#define DB_FIELD_USED 1
#define DB_FIELD_UNUSED 0


struct DBHeader{
  byte count;
  unsigned int start_address;
};

struct UserRecord{
  byte id;
  char tag[16];
  char token[16];
  byte finger_id;
  byte used;
};