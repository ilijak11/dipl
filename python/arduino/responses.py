class ArduinoResponses:
    FAIL = 0
    SUCC = 1
    ENTERED_REGISTER_MODE = 2
    CARD_PRESENT = 3
    CARD_AUTH_FAIL = 4
    CARD_AUTH_SUCCESS = 9
    CARD_WRITE_FAIL = 5
    CARD_WRITE_SUCCESS = 6
    DB_WRITE_FAIL = 7
    DB_WRITE_SUCCESS = 8
    ENROLLING_FINGER = 10
    WAITING_FOR_FINGER = 11
    IMAGE_TAKEN = 12
    NO_FINGER = 13
    COMMUNICATION_ERROR = 14
    IMAGE_ERROR = 15
    UNKNOWN_ERROR = 16
    IMAGE_CONVERTED = 17
    IMAGE_MESSY = 18
    NO_FEATURES = 19
    REMOVE_FINGER = 20
    PLACE_SAME_FINGER = 21
    MATCH = 22
    NO_MATCH = 23
    CREATING_MODEL = 24
    STORING_MODEL = 25
    STORED = 26
    BAD_LOCATION = 27
    FLASH_ERROR = 28
    WAITING_FOR_CARD = 29
    CARD_READ_FAIL = 30
    CARD_READ_SUCCESS = 31
    DB_READ_FAIL = 32
    DB_READ_SUCCESS = 33
    MATCH_TOKEN = 34
    NO_MATCH_TOKEN = 35
    MATCH_FINGER = 36
    NO_MATCH_FINGER = 37
    USER_REGISTERED = 38
    USER_ACCESS = 39
    DB_CLEAR = 40
    WAITING_FOR_USER_ID = 41
    USER_DELETED = 42


response_message_map = {
    ArduinoResponses.SUCC: {
        'code': ArduinoResponses.SUCC,
        'status': 1,
        'message': 'success'
    },
    ArduinoResponses.FAIL: {
        'code': ArduinoResponses.FAIL,
        'status': 0,
        'message': 'fail'
    },
    ArduinoResponses.ENTERED_REGISTER_MODE: {
        'code': ArduinoResponses.ENTERED_REGISTER_MODE,
        'status': 1,
        'message': 'entered register mode'
    },
    ArduinoResponses.CARD_PRESENT: {
        'code': ArduinoResponses.CARD_PRESENT,
        'status': 1,
        'message': 'card present'
    },
    ArduinoResponses.CARD_AUTH_FAIL: {
        'code': ArduinoResponses.CARD_AUTH_FAIL,
        'status': 0,
        'message': 'card auth failed'
    },
    ArduinoResponses.CARD_AUTH_SUCCESS: {
        'code': ArduinoResponses.CARD_AUTH_SUCCESS,
        'status': 1,
        'message': 'card auth success'
    },
    ArduinoResponses.CARD_WRITE_FAIL: {
        'code': ArduinoResponses.CARD_WRITE_FAIL,
        'status': 0,
        'message': 'card write failed'
    },
    ArduinoResponses.CARD_WRITE_SUCCESS: {
        'code': ArduinoResponses.CARD_WRITE_SUCCESS,
        'status': 1,
        'message': 'card write success'
    },
    ArduinoResponses.DB_WRITE_FAIL: {
        'code': ArduinoResponses.DB_WRITE_FAIL,
        'status': 0,
        'message': 'arduino db write fail'
    },
    ArduinoResponses.DB_WRITE_SUCCESS: {
        'code': ArduinoResponses.DB_WRITE_SUCCESS,
        'status': 1,
        'message': 'arduino db write success'
    },
    ArduinoResponses.ENROLLING_FINGER: {
        'code': ArduinoResponses.ENROLLING_FINGER,
        'status': 1,
        'message': 'enroling fingerprint procedure'
    },
    ArduinoResponses.WAITING_FOR_FINGER: {
        'code': ArduinoResponses.WAITING_FOR_FINGER,
        'status': 1,
        'message': 'put finger on sensor'
    },
    ArduinoResponses.IMAGE_TAKEN: {
        'code': ArduinoResponses.IMAGE_TAKEN,
        'status': 1,
        'message': 'image taken'
    },
    ArduinoResponses.NO_FINGER: {
        'code': ArduinoResponses.NO_FINGER,
        'status': 1,
        'message': '...'
    },
    ArduinoResponses.COMMUNICATION_ERROR: {
        'code': ArduinoResponses.COMMUNICATION_ERROR,
        'status': 0,
        'message': 'communication error'
    },
    ArduinoResponses.IMAGE_ERROR: {
        'code': ArduinoResponses.IMAGE_ERROR,
        'status': 0,
        'message': 'image error'
    },
    ArduinoResponses.UNKNOWN_ERROR: {
        'code': ArduinoResponses.UNKNOWN_ERROR,
        'status': 0,
        'message': 'unknown error'
    },
    ArduinoResponses.IMAGE_CONVERTED: {
        'code': ArduinoResponses.IMAGE_CONVERTED,
        'status': 1,
        'message': 'image converted'
    },
    ArduinoResponses.IMAGE_MESSY: {
        'code': ArduinoResponses.IMAGE_MESSY,
        'status': 0,
        'message': 'image messy'
    },
    ArduinoResponses.NO_FEATURES: {
        'code': ArduinoResponses.NO_FEATURES,
        'status': 0,
        'message': 'no features found for finger'
    },
    ArduinoResponses.REMOVE_FINGER: {
        'code': ArduinoResponses.REMOVE_FINGER,
        'status': 1,
        'message': 'remove finger'
    },
    ArduinoResponses.PLACE_SAME_FINGER: {
        'code': ArduinoResponses.PLACE_SAME_FINGER,
        'status': 1,
        'message': 'place same finger'
    },
    ArduinoResponses.MATCH: {
        'code': ArduinoResponses.MATCH,
        'status': 1,
        'message': 'fingers match'
    },
    ArduinoResponses.NO_MATCH: {
        'code': ArduinoResponses.NO_MATCH,
        'status': 0,
        'message': 'no match'
    },
    ArduinoResponses.CREATING_MODEL: {
        'code': ArduinoResponses.CREATING_MODEL,
        'status': 1,
        'message': 'creating model'
    },
    ArduinoResponses.STORING_MODEL: {
        'code': ArduinoResponses.STORING_MODEL,
        'status': 1,
        'message': 'storing model'
    },
    ArduinoResponses.STORED: {
        'code': ArduinoResponses.STORED,
        'status': 1,
        'message': 'model stored'
    },
    ArduinoResponses.BAD_LOCATION: {
        'code': ArduinoResponses.BAD_LOCATION,
        'status': 0,
        'message': 'bad location in flash'
    },
    ArduinoResponses.FLASH_ERROR: {
        'code': ArduinoResponses.FLASH_ERROR,
        'status': 0,
        'message': 'flash error'
    },
    ArduinoResponses.WAITING_FOR_CARD: {
        'code': ArduinoResponses.WAITING_FOR_CARD,
        'status': 1,
        'message': 'waiting for card - place card on reader...'
    },
    ArduinoResponses.CARD_READ_FAIL: {
        'code': ArduinoResponses.CARD_READ_FAIL,
        'status': 0,
        'message': 'card read failed'
    },
    ArduinoResponses.CARD_READ_SUCCESS: {
        'code': ArduinoResponses.CARD_READ_SUCCESS,
        'status': 1,
        'message': 'card read success'
    },
    ArduinoResponses.DB_READ_FAIL: {
        'code': ArduinoResponses.DB_READ_FAIL,
        'status': 0,
        'message': 'arduino db read failed'
    },
    ArduinoResponses.DB_READ_SUCCESS: {
        'code': ArduinoResponses.DB_READ_SUCCESS,
        'status': 1,
        'message': 'arduino db read success'
    },
    ArduinoResponses.MATCH_TOKEN: {
        'code': ArduinoResponses.MATCH_TOKEN,
        'status': 1,
        'message': 'tokens match'
    },
    ArduinoResponses.NO_MATCH_TOKEN: {
        'code': ArduinoResponses.NO_MATCH_TOKEN,
        'status': 0,
        'message': 'tokens do not match'
    },
    ArduinoResponses.MATCH_FINGER: {
        'code': ArduinoResponses.MATCH_FINGER,
        'status': 1,
        'message': 'fingers match'
    },
    ArduinoResponses.NO_MATCH_FINGER: {
        'code': ArduinoResponses.NO_MATCH_FINGER,
        'status': 0,
        'message': 'fingers do not match'
    },
    ArduinoResponses.USER_REGISTERED: {
        'code': ArduinoResponses.USER_REGISTERED,
        'status': 1,
        'message': 'user register success'
    },
    ArduinoResponses.USER_ACCESS: {
        'code': ArduinoResponses.USER_ACCESS,
        'status': 1,
        'message': 'user access success'
    },
    ArduinoResponses.DB_CLEAR: {
        'code': ArduinoResponses.DB_CLEAR,
        'status': 1,
        'message': '*arduino user database and fingerprint databases have been cleared*'
    },
    ArduinoResponses.WAITING_FOR_USER_ID: {
        'code': ArduinoResponses.WAITING_FOR_USER_ID,
        'status': 1,
        'message': 'waiting for user id'
    },
    ArduinoResponses.USER_DELETED: {
        'code': ArduinoResponses.USER_DELETED,
        'status': 1,
        'message': 'user deleted'
    },
}


# DELETING_USER = 41
#     WAITING_FOR_USER_ID = 42
#     USER_DELETED = 43


# CARD_READ_FAIL = 30
#     CARD_READ_SUCCESS = 31
#     DB_READ_FAIL = 32
#     DB_READ_SUCCESS = 33
#     MATCH_TOKEN = 34
#     NO_MATCH_TOKEN = 35
#     MATCH_FINER = 36
#     NO_MATCH_FINGER = 37

# ENROLLING_FINGER = 10
#     WAITING_FOR_FINGER = 11
#     IMAGE_TAKEN = 12
#     NO_FINGER = 13
#     COMMUNICATION_ERROR = 14
#     IMAGE_ERROR = 15
#     UNKNOWN_ERROR = 16
#     IMAGE_CONVERTED = 17
#     IMAGE_MESSY = 18
#     NO_FEATURES = 19
#     REMOVE_FINGER = 20
#     PLACE_SAME_FINGER = 21
#     MATCH = 22
#     NO_MATCH = 23
#     CREATING_MODEL = 24
#     STORING_MODEL = 25
#     STORED = 26
#     BAD_LOCATION = 27
#     FLASH_ERROR = 28