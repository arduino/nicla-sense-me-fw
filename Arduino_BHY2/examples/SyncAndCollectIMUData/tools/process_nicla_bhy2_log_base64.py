#!/usr/bin/env python
import math
import base64
import struct
import sys

#configurations
#DEBUG = True
DEBUG = False
lines_to_ignore = 500

# the following should be set according to the fimrware settings on the Nicla Senes ME firmware/sketch
data_include_acc = False
data_include_gyr = False
data_include_meta_info = False


if (len(sys.argv) > 1):
    log_file_name = sys.argv[1]
else:
    log_file_name = 'log_nicla_bhy2.txt'

if (len(sys.argv) > 2):
    info_included_in_data = sys.argv[2]
    if "accel" in info_included_in_data:
        data_include_acc = True
        if DEBUG:
            print("accel data included")
    if "gyro" in info_included_in_data:
        data_include_gyr = True
        if DEBUG:
            print("gyro data included")
    if "meta" in info_included_in_data:
        data_include_meta_info= True
        if DEBUG:
            print("meta data included")

#working variables
file_log = open(log_file_name, 'r')
lines = file_log.readlines()

lineCnt = 0
record_len = 0
record_len_before_encoding = 0

if data_include_meta_info:
    record_len_before_encoding += 2

if data_include_acc:
    record_len_before_encoding += 6

if data_include_gyr:
    record_len_before_encoding += 6


record_len = int(math.ceil(record_len_before_encoding / 3)) * 4
if DEBUG:
    print("record_len:", record_len)


DEBUG  = False
# Strips the newline character
for line in lines:
    lineCnt += 1

    if (lineCnt <= lines_to_ignore):
        continue

    line = line.strip()
    len_line = len(line)

    if (len_line < record_len) or (line[len_line - 1] != '='):
        if (lineCnt < len(lines)):
            print("line:", lineCnt, " skipped")
        else:
            if DEBUG:
                print("last line:", lineCnt, " skipped")
        continue

    line = line[-record_len:]
    try:
        data_imu = base64.b64decode(line)
        if DEBUG:
            print(data_imu)
            print(len(data_imu))

        try:
            if (data_include_acc and data_include_gyr):
                (log_id, seq, ax,ay,az,gx,gy,gz) = struct.unpack("<cBhhhhhh", data_imu)
                log_id = log_id.decode('ascii')
                print(log_id.strip(), ",", seq, ",", ax, ",", ay, ",", az, ",",  gx,",", gy, ",", gz, sep='')
            else:
                (log_id,seq, x,y,z) = struct.unpack("<cBhhh", data_imu)
                log_id = log_id.decode('ascii')
                print(log_id, ",", seq, ",",  x,",", y, ",", z, sep='')
        except struct.error:
            print("error: line:", lineCnt, " unpack")

    except base64.binascii.Error:
        print("error: line:", lineCnt, " incomplete data")
        pass

