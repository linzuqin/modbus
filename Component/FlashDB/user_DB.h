#ifndef __USER_DB_H
#define	__USER_DB_H
#include <flashdb.h>
#define VERSION		1.0.0
extern struct fdb_kvdb kvdb;
extern struct fdb_tsdb tsdb;

int user_db_init(void);
void kvdb_basic_sample(fdb_kvdb_t kvdb);
void Save_KV_Data(char* Key,void* Data,uint16_t Data_Len);
void Read_KV_Data(char* Key,void* Data,uint16_t Data_Len);
#endif
