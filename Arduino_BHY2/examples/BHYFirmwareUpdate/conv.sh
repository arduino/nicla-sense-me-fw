#!/bin/sh

if [ -f "$1" ] ; then
    cp -a "$1" BHI260AP_NiclaSenseME-flash.fw
    echo const > fw.h && xxd -i BHI260AP_NiclaSenseME-flash.fw >> fw.h
else
    echo "Usage: $0 <bhy_fw_bin_filename>"
    echo "\t<bhy_fw_bin_filename> ususally has a pattern: xxxx-flash.fw"
    echo "\tExample:"
    echo "\t\t$0 ./BHI260AP_NiclaSenseME_basic+bsec-flash.fw"
fi

