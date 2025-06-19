#ifndef _ONENET_H_
#define _ONENET_H_
#include "sys.h"



unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag);

#endif
