#!/usr/bin/env python
# this script resamples the data from the log to 1/60hz

import sys
import time
import datetime
import pandas as pd
import math


#DEBUG  = True
DEBUG  = False
SEP_COLUMN = ','

LINE_TO_SKIP = 500

DOWNSAMPLE_FACTOR=1

def PDBG(*args):
    if DEBUG:
        print("DEBUG:", *args, file=sys.stderr)


PDBG('Number of arguments:', len(sys.argv))

if (len(sys.argv) < 2):
    raise BaseException("missing argument")

filename_in  = sys.argv[1]
file_out = None

if (len(sys.argv) > 2):
    filename_out = sys.argv[2]
    file_out = open(filename_out, 'w')
    sys.stdout = file_out


line_cnt = 1    #first row is the header
df_in = pd.read_csv(filename_in, sep=SEP_COLUMN)

seq_last = None


for index, row in df_in.iterrows():
    line_cnt += 1
    if (line_cnt <= LINE_TO_SKIP):
        continue

    seq = row['seq']
    if (seq_last is not None):
        try:
            delta = seq - seq_last
            if (seq < seq_last):
                delta += 10
            if (delta != 1):
                if (line_cnt < len(df_in.index)):
                    print("error: line: ", line_cnt, " has data missing, delta:", delta, "seq:", seq)
                else:
                   PDBG("last line: ignored")
            seq_last = seq
        except:
            if (line_cnt < len(df_in.index)):
                print("error: line: ", line_cnt, " has data missing")
            else:
                PDBG("last line: ignored")
            pass
    else:
        PDBG("first row")
        seq_last = seq


if file_out is not None:
    file_out.close()
