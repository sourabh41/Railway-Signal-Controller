import serial
import sys
from threading import Thread

port1 = serial.Serial(
	port=sys.argv[1],
	baudrate=115200
)

port2 = serial.Serial(
	port=sys.argv[2],
	baudrate=115200
)

def copy1():
	print("Linked 1 to 2")
	while True:
		data1 = port1.read(1)
		port2.write(data1)

def copy2():
	print("Linked 2 to 1")
	while True:
		data2 = port2.read(1)
		port1.write(data2)

t1 = Thread(target = copy1)
t2 = Thread(target = copy2)

t1.start()
t2.start()