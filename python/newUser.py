from arduino.utils import ArduinoUtils
from arduino.commands import ArduinoCommands
from arduino.responses import ArduinoResponses
import hashlib


hash = hashlib.sha256('ilija kulic'.encode()).hexdigest()
data = str(hash)[0:16]




#send message to arduino to enter register mode
ArduinoUtils.send_command(ArduinoCommands.REGISTER_MODE)
#rfid
#finger
try:
    while True:
        response = ArduinoUtils.wait_for_response()
        res = ArduinoUtils.decode_response(response)
        print(f'arduino response: {res["message"]}')
        if not res['status']:
            print(f'closing connection')
            ArduinoUtils.close_connection()
            break
        if res['code'] == ArduinoResponses.CARD_PRESENT:
            ArduinoUtils.send_data(data)
except KeyboardInterrupt:
    ArduinoUtils.close_connection()


