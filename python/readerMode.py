from arduino.utils import ArduinoUtils
from arduino.commands import ArduinoCommands
from arduino.responses import ArduinoResponses

ArduinoUtils.send_command(ArduinoCommands.READER_MODE)
while True:
    res = ArduinoUtils.wait_for_response(decode=False)
    print(res)