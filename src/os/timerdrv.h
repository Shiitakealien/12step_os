#ifndef _TIMERDRV_H_INCLUDED_
#define _TIMERDRV_H_INCLUDED_

#define TIMERDRV_DEVICE_NUM  1
#define TIMERDRV_CMD_EXPIRE 'e' /* indicates timer is expired */
#define TIMERDRV_CMD_START  's' /* indicates timer starts */

struct timerreq {
    kz_msgbox_id_t id;
    int msec;
};

#endif
