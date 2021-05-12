#include "constants.h"
#include <string.h>


int set_message(Message * msg, long type, char * buf)
{
    if (!msg) return -1;
    msg->type = type;
    for (int i = 0; i < MAX_MSG_LEN; ++i) msg->buf[i] = 0;
    strcpy(msg->buf, buf);
    return 0;
}