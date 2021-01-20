#ifndef BHY_UPLOAD_H_
#define BHY_UPLOAD_H_

#include "mbed.h"

#include "bhy2.h"
#include "bhy2_parse.h"
#include "common.h"

#define MOUNT_PATH           "fs"
#define BHY_UPDATE_FILE_PATH   "/" MOUNT_PATH "/BHY_UPDATE.BIN"

struct parse_ref
{
    struct
    {
        uint8_t accuracy;
        float scaling_factor;
    }
    sensor[BHY2_SENSOR_ID_MAX];
    uint8_t *verbose;
};

int fwupdate_bhi260(void);

#endif
