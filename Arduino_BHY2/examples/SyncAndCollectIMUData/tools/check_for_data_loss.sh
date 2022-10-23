#!/bin/sh


log_encoding_is_base64=false

if [ ":""$1" = ":-b" ] ; then
    log_encoding_is_base64=true
    echo "log is using base64 encoding"
elif [ ":""$1" = ":-a" ] ; then
    log_encoding_is_base64=false
    echo "log is using ascii encoding"
else
    echo "usage: ./check_for_data_loss.sh [OPTION] [log_filename] [info_included_in_data]"
    echo "\t[OPTION]:"
    echo "\t\t" "-a"
    echo "\t\t" "\t:log use ascii encoding"
    echo "\t\t" "-b"
    echo "\t\t" "\t:log use base64 encoding"
    echo "\t[info_included_in_data]:"
    echo "\t\t" '"accel" or "accel+meta"'
    echo "\t\t" '"gyro" or "gyro+meta"'
    echo "\t\t" '"accel+gyro" or "accel+gyro+meta"'


    echo "\texample: ./check_for_data_loss.sh -b minicom.cap accel+gyro+meta"
    return
fi

if [ ":""$2" = ":" ] ; then
    log_file="./minicom.cap"
else
    log_file="$2"
fi

if [ ":""$3" = ":" ] ; then
    info_included_in_data="accel+gyro+meta"
else
    info_included_in_data="$3"
fi


tmp_file="./tmp.csv"
log_file_cp="${log_file}.cp"
log_file_in="${log_file_cp}.tmp"

echo "log_file:$log_file"


if [ $log_encoding_is_base64 = true ] ; then
    cp $log_file $log_file_cp
    ./process_nicla_bhy2_log_base64.py $log_file_cp $info_included_in_data > $log_file_in
else
    cp $log_file $log_file_in
fi

echo "log_id,seq,ax,ay,az,gx,gy,gz" > $tmp_file

sed 's/.*\([a-zA-Z]\)\([0-9]\)/\1,\2/g' $log_file_in >> $tmp_file
./check_for_data_loss.py $tmp_file

rm -f $tmp_file
rm  -f $log_file_cp
rm  -f $log_file_in
