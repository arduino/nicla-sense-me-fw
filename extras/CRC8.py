#!/usr/bin/python3

import sys
import crccheck

if len(sys.argv) != 3:
    print ("Usage: CRC8.py sketch.bin sketchCRC.bin")
    sys.exit()

ifile = sys.argv[1]
ofile = sys.argv[2]

# Read the binary file
in_file = open(ifile, "rb")
bin_data = bytearray(in_file.read()+'0')
bin_data_len = len(bin_data) -1
in_file.close()

print(bin_data_len)

bin_data[bin_data_len] = bin_data[0]^bin_data[1] 
for i in range(2,bin_data_len-1):
    bin_data[bin_data_len] ^= bin_data[i]

# Write to outfile
out_file = open(ofile, "wb")
out_file.write(bin_data)
out_file.close()
