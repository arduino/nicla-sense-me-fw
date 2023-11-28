#!/usr/bin/env python3
#
# This scripts is the companion application for the DataHarvester sketch.
# It is a very basic application and can be improved and extended.
#
# It waits for user input to control the data acquisition operations and the
# connection to the Arduino board and to save the received data to a CSV file.
#
# It establish a serial communication to the board and uses COBS as encoding and
# reliable transport protocol.
#
# The main thread will always wait for user input while a second one is created
# to exchange both control and data with the Arduino board.
# The two threads coordinate via a simple Queue.
#
# Install the required module using this command in Windows
# pip -m install pyreadline3 pyserial cobs

# Thread and Queue
from threading import Thread
import queue

# Helpers
from datetime import date, datetime
import struct

# User input
from pyreadline3 import Readline # import readline
readline = Readline()
import argparse

# Communication with the Arduino board
import serial
from cobs import cobs


# The main serial object
# initialize later
ser = serial.Serial()

# The function to be run in the Arduino-facing thread
def receiver(cmd_q, uart, name, tag):
    f = None
    running = False
    cmd = 'N'

    while True:
        # Check for commands from the main thread
        try:
            cmd = cmd_q.get(block=False)
        except queue.Empty:
            pass

        # Start acquisition, prepare output file, etc.
        if cmd == 'R':
            running = True
            today = datetime.today()
            iso = datetime.isoformat(today)
            filename = f'{name}_{tag}_{iso}.csv'
            f = open(filename, 'wt')
            print(f'timestamp,accX,accY,accZ', file=f)
            cmd = 'N'
        # Stop acquisition
        elif cmd == 'S':
            running = False
            if f != None:
                f.close()
            cmd = 'N'
        # Close connection
        elif cmd == 'C':
            running = False
        # Quit program
        elif cmd == 'Q':
            running = False
            if f != None:
                if not f.closed:
                    f.close()
            break

        # Receive data packet-by-packet and save to file
        if running:
            if uart.is_open:
                # Read full COBS packet
                data = uart.read_until(b'\x00')
                n = len(data)
                if n > 0:
                    # Skip last byte
                    decoded = cobs.decode(data[0:(n - 1)])
                    # Unpack the binary data
                    ts, x, y, z = struct.unpack('ffff', decoded)
                    # Create CSV line and print
                    print(f'{ts},{x},{y},{z}', file=f)
                    f.flush()


# The main thread
if __name__ == '__main__':

    # Parse arguments
    # - port == Arduino Serial Port
    # - name == Filename
    # - tag == Custom tag to append to filename
    parser = argparse.ArgumentParser()
    parser.add_argument('port')
    parser.add_argument('name')
    parser.add_argument('tag')
    args = parser.parse_args()

    ser.port = args.port
    ser.baudrate = 115200

    ser.close()
    ser.open()

    # Inter-thread communication queue
    q = queue.Queue()

    # Arduino-facing thread
    recv_thread = Thread(target=receiver, args=(q, ser, args.name, args.tag))
    recv_thread.start()

    # Wait for user input
    while True:
        s = input("> ").upper()

        # Send commands to receiver thread
        if s == 'S':
            print('Stopping...')
            q.put('S')
        elif s == 'R':
            print('Running...')
            q.put('R')
        elif s == 'C':
            print('Closing...')
            q.put('C')
            if ser.is_open:
                ser.close()
        elif s == 'O':
            print('Opening...')
            if not ser.is_open:
                ser.open()
        elif s == 'Q':
            print('Quitting...')
            q.put('Q')
            recv_thread.join()
            if ser.is_open:
                ser.close()
            break

        # Send commands to the Arduino board
        if ser.is_open:
            enc = cobs.encode(s.encode())
            ser.write(enc)
            ser.write(b'\x00')
