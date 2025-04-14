#!/usr/bin/env python3

import threading
import serial
import time
import queue

def readerThread(args):
    ser = args['serial']
    queue = args['queue']
    while(True):
        data = ser.readline()
        queue.put([data, time.time()])

def main():
    ser = serial.Serial("/dev/ttyACM0")
    # Non threaded
    # while(True):
    #     print(ser.readline())

    # Threaded
    q = queue.Queue(maxsize=16)
    t1 = threading.Thread(target = readerThread, args = ({"serial":ser, "queue":q},))
    t1.start()
    while(True):
        print(q.get(), time.time())

if __name__ == "__main__":
    main()
