#include "user_DB.h"
#include "MyRTC.h"
#include "stm32f10x.h"                  // Device header
#include "rtthread.h"
extern uint32_t Unix_Time;


#define FDB_LOG_TAG "[user_db]"
#ifdef FDB_USING_TIMESTAMP_64BIT
#define __PRITS "ld"
#else
#define __PRITS "d"
#endif
/* KVDB object */
struct fdb_kvdb kvdb = { 0 };
struct fdb_tsdb tsdb = { 0 };

/* default KV nodes */
static void lock(fdb_db_t db)
{
    __disable_irq();
}

static void unlock(fdb_db_t db)
{
    __enable_irq();
}
static fdb_time_t get_time(void)
{
    /* Using the counts instead of timestamp.
     * Please change this function to return RTC time.
     */
		MyRTC_ReadTime();
    return Unix_Time;
}

uint16_t boot_count = 0;
static struct fdb_default_kv_node default_kv_table[] = {
    { "boot", &boot_count, sizeof(&boot_count) },
};
int user_db_init(void)
{
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;
        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, lock);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, unlock);
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        uint8_t result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, NULL);
        if (result != FDB_NO_ERR) {
            return -1;
        }

				Read_KV_Data("boot", &boot_count, sizeof(boot_count));
        boot_count ++;
        Save_KV_Data("boot", &boot_count, sizeof(boot_count));
    }
		return 1;
}
INIT_APP_EXPORT(user_db_init);

static bool query_cb(fdb_tsl_t tsl, void *arg);
static bool query_by_time_cb(fdb_tsl_t tsl, void *arg);
static bool set_status_cb(fdb_tsl_t tsl, void *arg);
struct env_status {
        int temp;
        int humi;
};
void db_tsdb_sample(void)
{
    struct fdb_blob blob;
    struct env_status status;

    status.temp = 36;
    status.humi = 85;
    fdb_tsl_append(&tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
    
    FDB_INFO("hellp");
    FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);

    status.temp = 38;
    status.humi = 90;
    fdb_tsl_append(&tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
    FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);

    { /* QUERY the TSDB */
        /* query all TSL in TSDB by iterator */
        fdb_tsl_iter(&tsdb, query_cb, &tsdb);
    }

    { /* QUERY the TSDB by time */
        /* prepare query time (from 1970-01-01 00:00:00 to 2020-05-05 00:00:00) */
        struct tm tm_from = { .tm_year = 1970 - 1900, .tm_mon = 0, .tm_mday = 1, .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        struct tm tm_to = { .tm_year = 2020 - 1900, .tm_mon = 4, .tm_mday = 5, .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        time_t from_time = mktime(&tm_from), to_time = mktime(&tm_to);
        volatile size_t count;
        /* query all TSL in TSDB by time */
        fdb_tsl_iter_by_time(&tsdb, from_time, to_time, query_by_time_cb, &tsdb);
        /* query all FDB_TSL_WRITE status TSL's count in TSDB by time */
        count = fdb_tsl_query_count(&tsdb, from_time, to_time, FDB_TSL_WRITE);
        FDB_INFO("query count is: %zu\n", count);
    }

    { /* SET the TSL status */
        /* Change the TSL status by iterator or time iterator
         * set_status_cb: the change operation will in this callback
         *
         * NOTE: The actions to modify the state must be in order.
         *       like: FDB_TSL_WRITE -> FDB_TSL_USER_STATUS1 -> FDB_TSL_DELETED -> FDB_TSL_USER_STATUS2
         *       The intermediate states can also be ignored.
         *       such as: FDB_TSL_WRITE -> FDB_TSL_DELETED
         */
        fdb_tsl_iter(&tsdb, set_status_cb, &tsdb);
    }

    FDB_INFO("===========================================================\n");
}

static bool query_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status;
    fdb_tsdb_t db = arg;

    fdb_blob_read((fdb_db_t) db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
    FDB_INFO("[query_cb] queried a TSL: time: %" __PRITS ", temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

    return false;
}

static bool query_by_time_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status;
    fdb_tsdb_t db = arg;

    fdb_blob_read((fdb_db_t) db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
    FDB_INFO("[query_by_time_cb] queried a TSL: time: %" __PRITS ", temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

    return false;
}

static bool set_status_cb(fdb_tsl_t tsl, void *arg)
{
    fdb_tsdb_t db = arg;

    FDB_INFO("set the TSL (time %" __PRITS ") status from %d to %d\n", tsl->time, tsl->status, FDB_TSL_USER_STATUS1);
    fdb_tsl_set_status(db, tsl, FDB_TSL_USER_STATUS1);

    return false;
}

void Save_KV_Data(char* Key,void* Data,uint16_t Data_Len)
{
	struct fdb_blob blob;
	
	fdb_kv_set_blob(&kvdb, Key, fdb_blob_make(&blob, Data, Data_Len));
}

void Read_KV_Data(char* Key,void* Data,uint16_t Data_Len)
{
	struct fdb_blob blob;
	
	fdb_kv_get_blob(&kvdb, Key, fdb_blob_make(&blob, Data, Data_Len));
}
