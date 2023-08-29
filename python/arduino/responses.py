class ArduinoResponses:
    FAIL = 0
    SUCC = 1
    ENTERED_REGISTER_MODE = 2
    CARD_PRESENT = 3
    CARD_AUTH_FAIL = 4
    CARD_WRITE_FAIL = 5
    CARD_WRITE_SUCCESS = 6


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
    ArduinoResponses.CARD_WRITE_FAIL: {
        'code': ArduinoResponses.CARD_WRITE_FAIL,
        'status': 0,
        'message': 'card write failed'
    },
    ArduinoResponses.CARD_WRITE_SUCCESS: {
        'code': ArduinoResponses.CARD_WRITE_SUCCESS,
        'status': 1,
        'message': 'card write success'
    }
}
