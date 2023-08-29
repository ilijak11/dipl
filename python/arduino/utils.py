import serial
import time
from arduino.config import Config
from arduino.responses import response_message_map

s = serial.Serial(
    port=Config.ARDUINO_SERIAL_PORT,
    baudrate=Config.ARDUINO_BAUD_RATE,
    timeout=1
)
time.sleep(3)

class ArduinoUtils:

    @staticmethod
    def decode_response(response):
        print(response)
        res = int(response)
        if res in response_message_map:
            return response_message_map[res]
        else:
            return {
                'status': 0,
                'message': 'message for response not found'
            }

    @staticmethod
    def send_command(command):
        s.write(f'{command}\r\n'.encode())
        time.sleep(1)


    @staticmethod
    def send_data(data):
        s.write(f'{str(data)}~'.encode())
        time.sleep(1)

    @staticmethod
    def wait_for_response():
        while True:
            res = s.readline()
            print(res)
            if res:
                return res.decode().strip()

    @staticmethod
    def close_connection():
        s.close()
