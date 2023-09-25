
import serial
import time
import statistics

s = serial.Serial(port = 'COM3', baudrate = 9600, timeout = .1)

list = []

try:
    while True:
        data = s.readline().decode()
        if not data:
            continue
        print(data)
        data = data.split()
        list.append(int(data[1]))
        if len(list) == 20:
            break
except KeyboardInterrupt:
    s.close()

print(list)
print(f'avg: {sum(list)/len(list)}')
print(f'stdev: {statistics.stdev(list)}')