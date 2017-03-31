/*
 * PublicFunction.h
 *
 *  Created on: Sep 11, 2015
 *      Author: fzh
 */

#ifndef PUBLICFUNCTION_H_
#define PUBLICFUNCTION_H_

#include <signal.h>
#include <semaphore.h>
#include <syslog.h>
#include "Shmem.h"
#include "StdDataType.h"
#include "ParaDef.h"

#define HEX_TO_BCD(x) (((x)/0x0A)*0x10+((x)%0x0A))
#define BCD_TO_HEX(x) (((x)/0x10)*0x0A+((x)%0x10))
#define ASCII_TO_HEX(c) (((c) >='0' && (c) <='9')?((c)-'0'):(((c)>='A'&&(c)<='F')?((c)-'A'+10):(((c)>='a'&&c<='f')?((c)-'a'+10):0)))
#define isdigit(c) ((unsigned) ((c)-'0') < 10)
#define HEX_TO_ASCII(x) ((x<=0x09)?(x+0x30):(x+0x37))

#define U8_TO_ASCII_H(x) HEX_TO_ASCII(((x)&0x0F))
#define U8_TO_ASCII_L(x) HEX_TO_ASCII(((x)&0x0F))
#define BIT_N(byte, n) (((byte)>>(n))&0x01)
#define	NELEM(array)	(sizeof(array)/sizeof(array[0]))

#define	FILE_LINE		__FILE__,__FUNCTION__,__LINE__
#define	DEBUG_TIME_LINE(format, ...)	debug(FILE_LINE, format, ##__VA_ARGS__)

extern void setsystime(DateTimeBCD datetime);
extern void TSGet(TS* ts);
extern INT8U TScompare(TS ts1, TS ts2);
extern time_t tmtotime_t(TS ptm);
extern void DataTimeGet(DateTimeBCD* ts);
extern INT8S tminc(TS* tmi, Time_Units unit, INT32S val);
extern void Setsig(struct sigaction* psa, void (*pfun)(ProjectInfo* proinfo));
/*
 * 共享内存操作函数
 * */
extern void* CreateShMem(char* shname, int memsize, void* pmem);
extern void* OpenShMem(char* shname, int memsize, void* pmem);
extern void shmm_unregister(char* shname, int memsize);
extern INT32S prog_find_pid_by_name(INT8S* ProcName, INT32S* foundpid);
/*
 * 串口操作函数
 * */
extern int OpenCom(int port, int baud, unsigned char* par, unsigned char stopb, unsigned char bits);
extern void CloseCom(int ComPort);

/*
 * 时间转换函数
 * */
extern void TSGet(TS* ts);
extern time_t tmtotime_t(TS ptm);
extern void DataTimeGet(DateTimeBCD* ts);

/* BCD码转int32u
 *参数：bcd为bcd码头指针，len为bcd码长度，order为positive正序/inverted反序，dint转换结果
 * 返回:0：成功；-1：asc为空；-2：en为0；-3：order有误
 * 例如:0x12 0x34 -> 1234
 * */
extern INT8S bcd2int32u(INT8U* bcd, INT8U len, ORDER order, INT32U* dint);
extern INT32S int32u2bcd(INT32U dint32, INT8U* bcd, ORDER order);
INT32S asc2bcd(INT8U* asc, INT32U len, INT8U* bcd,ORDER order);
/*
 * gpio操作函数
 */
extern INT8S gpio_readbyte(char* devpath);
extern INT32S gpio_readint(char* devpath);
extern INT32S gpio_writebyte(char* devpath, INT8S data);
extern INT32S gpio_writebytes(char* devpath, INT8S* vals, INT32S valnum);

extern BOOLEAN pwr_has();
extern BOOLEAN pwr_has_byVolt(INT8U valid, INT32U volt, INT16U limit);
extern BOOLEAN bettery_getV(FP32* clock_bt, FP32* tmnl_bt);

extern INT8S getSpiAnalogState();
/*
 * 信号量操作函数
 * */
extern sem_t* nsem_open(const char* name);
extern sem_t* create_named_sem(const char* name, int flag);
extern sem_t* open_named_sem(const char* name);
extern void close_named_sem(const char* name);

/*
 * 数据从大到小排序 arr数组 len长度
 */
extern INT8U getarryb2s(INT32S* arr, INT8U len);

/*
 * 日志处理函数
 */
void asyslog(int priority, const char* fmt, ...);
void bufsyslog(const INT8U* buf, const char* title, int head, int tail, int len);


/*
 * 数据处理
 * */
extern INT8U getBase_DataTypeLen(Base_DataType dataType);
extern INT8S reversebuff(INT8U* buff,INT32U len,INT8U* invbuff);

extern void debug(const char* file, const char* func, INT32U line, const char *fmt, ...);
extern void readFrm(char* str,  INT32U strLen, INT8U* buf, INT32U* bufSize);
#endif /* PUBLICFUNCTION_H_ */
