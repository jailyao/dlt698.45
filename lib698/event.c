#include <time.h>
#include <string.h>
#include <strings.h>

#include "event.h"
#include "EventObject.h"
#include "PublicFunction.h"

static TSA TSA_LIST[MAX_POINT_NUM];
static int TSA_NUMS;
static TerminalEvent_Object EVENT_OBJS;

static TSA tsa_list;
static tsa_list_num;

//计算时间差
void DataTimeCut(DateTimeBCD* ts) {
    struct tm set;
    time_t times;
    times = time(NULL);
    localtime_r(&times, &set);
    set.tm_year = ts->year.data - 1900;
    set.tm_mon  = ts->month.data - 1;
    set.tm_mday = ts->day.data;
    set.tm_hour = ts->hour.data;
    set.tm_min  = ts->min.data;
    set.tm_sec  = ts->sec.data;
}

INT8U Event_Init() {
    //初始化EVENT_OBJS结构体（读文件）
    bzero(&EVENT_OBJS, sizeof(TerminalEvent_Object));
    //扫描、初始化电表表号档案TSA_LIST（读文件）
    return 1;
}

/*
 * 分析抄表存储的报文，输入任务id和抄读对象地址。
 */
INT8U Event_AnalyseData(INT8U* buf, INT32U id, TSA tsa) {
    //存储事件产生所需要的判据
    INT8U data[64];
    bzero(data, sizeof(data));

    //寻找已知表号，并对这个产生未知电表事件
    if (Event_FindTsa(tsa)) {
        Event_3111(tsa, data); //发现未知电能表事件16
        Event_3112(tsa, data); //跨台区电能表事件17
        Event_311A(tsa, data); //电能表在网状态切换事件24
    }

    //找正向有功数据
    if (Event_FindOAD(buf, 0x0010, data)) {
        Event_310B(tsa, data); //电能表示度下降事件10
        Event_310C(tsa, data); //电能量超差事件11
        Event_310D(tsa, data); //电能表飞走事件12
        Event_310E(tsa, data); //电能表停走事件13
    }

    //找电表校时数据
    if (Event_FindOAD(buf, 0x4000, data)) {
        Event_3105(tsa, data); //电能表时钟超差事件4
    }

    return 1;
}

/*
 * 输入报文和想要找到的OAD，将结果数据放入data中，返回1为成功，返回0为失败。
 */
INT8U Event_FindOAD(INT8U* buf, OI_698 oad, INT8U* data) {
    return 1;
}

/*
 * 寻找已知表号，并对这个产生未知电表事件
 */
INT8U Event_FindTsa(TSA tsa) {
    for (int i = 0; i < TSA_NUMS; i++) {
        int find = 1;
        for (int j = 0; j < tsa.addr[0]; j++) {
            if (tsa.addr[j] != TSA_LIST[i].addr[j]) {
                find = 0;
                break;
            }
        }
        if (find == 1) {
            return 1;
        }
    }
    return 0;
}

//电能表时钟超差事件4
INT8U Event_3105(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event3105_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3105_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    EVENT_OBJS.Event3105_obj.event_obj.crrentnum++;

    return 1;
}

//设备故障记录9
INT8U Event_310A(INT8U type, INT8U* data) {
    if (EVENT_OBJS.Event310A_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310A_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //    事件发生源∷=enum
    //    {
    //    终端主板内存故障（0），
    //    时钟故障    （1），
    //    主板通信故障    （2），
    //    485 抄表故障  （3），
    //    显示板故障  （4），
    //    载波通道异常  （5）
    //    }

    Save_buf[25] = 0x16;
    Save_buf[26] = type && 0xff;

    //事件上报状态
    Save_buf[27] = 0x01;
    Save_buf[28] = 0x01;
    //通道号
    Save_buf[29] = 0x33;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x02;
    Save_buf[32] = 0x00;
    Save_buf[33] = 0x01;
    //上报状态
    Save_buf[34] = 0x11;
    Save_buf[35] = 0x00;

    EVENT_OBJS.Event310A_obj.crrentnum++;

    return 1;
}

//电能表示度下降事件10
INT8U Event_310B(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event310B_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310B_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //事件发生前正向有功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x10;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    memcpy(&Save_buf[40], data, sizeof(INT32U));

    //事件发生后正向有功总电能
    Save_buf[44 + tsalen] = 0x00;
    Save_buf[45 + tsalen] = 0x10;
    Save_buf[46 + tsalen] = 0x82;
    Save_buf[47 + tsalen] = 0x01;

    memcpy(&Save_buf[48], data + 4, sizeof(INT32U));

    EVENT_OBJS.Event310B_obj.event_obj.crrentnum++;

    return 1;
}

//电能量超差事件11
INT8U Event_310C(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event310C_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310C_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //事件发生前正向有功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x10;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    memcpy(&Save_buf[40], data, sizeof(INT32U));

    //事件发生后正向有功总电能
    Save_buf[44 + tsalen] = 0x00;
    Save_buf[45 + tsalen] = 0x10;
    Save_buf[46 + tsalen] = 0x82;
    Save_buf[47 + tsalen] = 0x01;

    memcpy(&Save_buf[48], data + 4, sizeof(INT32U));

    EVENT_OBJS.Event310C_obj.event_obj.crrentnum++;

    return 1;
}

//电能表飞走事件12
INT8U Event_310D(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event310D_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310D_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //事件发生前正向有功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x10;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    memcpy(&Save_buf[40], data, sizeof(INT32U));

    //事件发生后正向有功总电能
    Save_buf[44 + tsalen] = 0x00;
    Save_buf[45 + tsalen] = 0x10;
    Save_buf[46 + tsalen] = 0x82;
    Save_buf[47 + tsalen] = 0x01;

    memcpy(&Save_buf[48], data + 4, sizeof(INT32U));

    EVENT_OBJS.Event310D_obj.event_obj.crrentnum++;

    return 1;
}

//电能表停走事件13
INT8U Event_310E(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event310E_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310E_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //事件发生前正向有功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x10;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    memcpy(&Save_buf[40], data, sizeof(INT32U));

    EVENT_OBJS.Event310E_obj.event_obj.crrentnum++;

    return 1;
}

//终端抄表失败事件14
INT8U Event_310F(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event310F_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event310F_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //事件发生前最近一次抄表成功时间,需要抄表数据
    Save_buf[36 + tsalen] = 0x60;
    Save_buf[37 + tsalen] = 0x41;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x00;

    //最近一次正向有功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x10;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    //最近一次正向无功总电能
    Save_buf[36 + tsalen] = 0x00;
    Save_buf[37 + tsalen] = 0x30;
    Save_buf[38 + tsalen] = 0x22;
    Save_buf[39 + tsalen] = 0x01;

    EVENT_OBJS.Event310F_obj.event_obj.crrentnum++;

    return 1;
}

//发现未知电能表事件16
INT8U Event_3111(TSA tsa, INT8U* data) {
    /*
     * 需要搜表结果，无法获得台区属性
     */
    return 1;
}

//跨台区电能表事件17
INT8U Event_3112(TSA tsa, INT8U* data) {
    /*
     * 需要搜表结果，无法获得台区属性
     */
    return 1;
}
//电能表在网状态切换事件24
INT8U Event_311A(TSA tsa, INT8U* data) {
    return 1;
}

//终端对电表校时记录25
INT8U Event_311B(TSA tsa, INT8U* data) {
    if (EVENT_OBJS.Event311B_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event311B_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源TSA
    int tsalen   = tsa.addr[0];
    Save_buf[25] = 0x55;
    memcpy(&Save_buf[26], &tsa, sizeof(TSA));

    //事件上报状态
    Save_buf[27 + tsalen] = 0x01;
    Save_buf[28 + tsalen] = 0x01;
    //通道号
    Save_buf[29 + tsalen] = 0x33;
    Save_buf[30 + tsalen] = 0x00;
    Save_buf[31 + tsalen] = 0x02;
    Save_buf[32 + tsalen] = 0x00;
    Save_buf[33 + tsalen] = 0x01;
    //上报状态
    Save_buf[34 + tsalen] = 0x11;
    Save_buf[35 + tsalen] = 0x00;

    //校表前时间
    Save_buf[36 + tsalen] = 0x1C;
    memcpy(&Save_buf[37 + tsalen], data, sizeof(ntime));

    //时钟误差
    Save_buf[44 + tsalen] = 0x0F;
    Save_buf[45 + tsalen] = 0x00;

    EVENT_OBJS.Event311B_obj.crrentnum++;

    return 1;
}

//电能表数据变更监控记录26
INT8U Event_311C(TSA tsa, INT8U* data) {
    return 1;
}

//终端初始化事件1
INT8U Event_3100(INT8U* data) {
    if (EVENT_OBJS.Event3100_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3100_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件上报状态
    Save_buf[25] = 0x01;
    Save_buf[26] = 0x01;
    //通道号
    Save_buf[27] = 0x33;
    Save_buf[28] = 0x00;
    Save_buf[29] = 0x02;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x01;
    //上报状态
    Save_buf[32] = 0x11;
    Save_buf[33] = 0x00;

    EVENT_OBJS.Event3100_obj.crrentnum++;

    return 1;
}

//终端消息认证错误事件8
INT8U Event_3109(INT8U* data) {
    if (EVENT_OBJS.Event3109_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3109_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件上报状态
    Save_buf[25] = 0x01;
    Save_buf[26] = 0x01;
    //通道号
    Save_buf[27] = 0x33;
    Save_buf[28] = 0x00;
    Save_buf[29] = 0x02;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x01;
    //上报状态
    Save_buf[32] = 0x11;
    Save_buf[33] = 0x00;

    EVENT_OBJS.Event3109_obj.crrentnum++;

    return 1;
}

//月通信流量超限事件15
INT8U Event_3110(INT8U* data) {
    if (EVENT_OBJS.Event3110_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3110_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件上报状态
    Save_buf[25] = 0x01;
    Save_buf[26] = 0x01;
    //通道号
    Save_buf[27] = 0x33;
    Save_buf[28] = 0x00;
    Save_buf[29] = 0x02;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x01;
    //上报状态
    Save_buf[32] = 0x11;
    Save_buf[33] = 0x00;

    //已发生通信流量
    Save_buf[34] = 0x22;
    Save_buf[35] = 0x00;
    Save_buf[36] = 0x42;
    Save_buf[37] = 0x02;

    Save_buf[38] = 0x02;
    Save_buf[39] = 0x02;
    Save_buf[40] = 0x06;
    memcpy(&Save_buf[41], data, sizeof(INT32U));
    Save_buf[45] = 0x06;
    memcpy(&Save_buf[46], data + 4, sizeof(INT32U));

    //月通信流量门限
    Save_buf[50] = 0x31;
    Save_buf[51] = 0x10;
    Save_buf[52] = 0x06;
    Save_buf[53] = 0x01;

    Save_buf[40] = 0x06;
    memcpy(&Save_buf[41], EVENT_OBJS.Event3110_obj.Monthtrans_obj.month_offset, sizeof(INT32U));

    EVENT_OBJS.Event3110_obj.event_obj.crrentnum++;

    return 1;
}

//终端对时事件18
INT8U Event_3114(INT8U* data) {
    if (EVENT_OBJS.Event3114_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3114_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件上报状态
    Save_buf[25] = 0x01;
    Save_buf[26] = 0x01;
    //通道号
    Save_buf[27] = 0x33;
    Save_buf[28] = 0x00;
    Save_buf[29] = 0x02;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x01;
    //上报状态
    Save_buf[32] = 0x11;
    Save_buf[33] = 0x00;

    //事件发生前时间
    Save_buf[34] = 0x40;
    Save_buf[35] = 0x00;
    Save_buf[36] = 0x22;
    Save_buf[37] = 0x00;

    Save_buf[38] = 0x1C;
    memcpy(&Save_buf[39], &ntime, sizeof(ntime));

    //事件发生后时间
    Save_buf[47] = 0x40;
    Save_buf[48] = 0x00;
    Save_buf[49] = 0x82;
    Save_buf[50] = 0x00;

    Save_buf[51] = 0x1C;
    memcpy(&Save_buf[52], data, sizeof(ntime));

    EVENT_OBJS.Event3114_obj.crrentnum++;

    return 1;
}

//购电参数设置记录29
INT8U Event_3202(INT8U* data) {
    //暂时不使用
    return 1;
}

//终端停/上电事件5-停电事件-放在交采模块
INT8U Event_3202_1(INT8U* data) {
    return 1;
}

//终端停/上电事件5-上电事件-放在交采模块-发起抄表动作
INT8U Event_3202_2(INT8U* data) {
    return 1;
}

//终端停/上电事件5-判定事件-放在停电抄表模块，判定停电有效性
INT8U Event_3202_3(INT8U* data) {
    return 1;
}

//终端停/上电事件5-放在轻量级轮训模块，用于处理停电事件抄表未回情况
INT8U Event_3202_clean(INT8U* data) {
    return 1;
}

/*
 * 分析交采数据，产生对应的配置事件。
 */
INT8U Event_AnalyseACS(INT8U* data) {
    return 1;
}

//终端直流模拟量越上限事件6
INT8U Event_3107(INT8U* data) {
    if (EVENT_OBJS.Event3107_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3107_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源
    Save_buf[25] = 0xF2;
    Save_buf[25] = 0x04;
    Save_buf[25] = 0x02;
    Save_buf[25] = 0x00;
    memcpy(&Save_buf[26], data, sizeof(INT32S));
    //事件上报状态
    Save_buf[30] = 0x01;
    Save_buf[31] = 0x01;
    //通道号
    Save_buf[32] = 0x33;
    Save_buf[33] = 0x00;
    Save_buf[34] = 0x02;
    Save_buf[35] = 0x00;
    Save_buf[36] = 0x01;
    //上报状态
    Save_buf[37] = 0x11;
    Save_buf[38] = 0x00;

    EVENT_OBJS.Event3107_obj.event_obj.crrentnum++;

    return 1;
}
//终端直流模拟量越下限事件7
INT8U Event_3108(INT8U* data) {
    if (EVENT_OBJS.Event3108_obj.event_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3108_obj.event_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //事件发生源
    Save_buf[25] = 0xF2;
    Save_buf[25] = 0x04;
    Save_buf[25] = 0x02;
    Save_buf[25] = 0x00;
    memcpy(&Save_buf[26], data, sizeof(INT32S));
    //事件上报状态
    Save_buf[30] = 0x01;
    Save_buf[31] = 0x01;
    //通道号
    Save_buf[32] = 0x33;
    Save_buf[33] = 0x00;
    Save_buf[34] = 0x02;
    Save_buf[35] = 0x00;
    Save_buf[36] = 0x01;
    //上报状态
    Save_buf[37] = 0x11;
    Save_buf[38] = 0x00;

    EVENT_OBJS.Event3108_obj.event_obj.crrentnum++;

    return 1;
}
//终端电流回路异常事件23,II型集中器没有电流，暂时不处理
INT8U Event_3119(INT8U type, INT8U* data) {
    if (EVENT_OBJS.Event3119_obj.enableflag == 0) {
        return 0;
    }

    //事件判定
    if (0) {
        return 0;
    }

    INT8U Save_buf[256];
    bzero(Save_buf, sizeof(Save_buf));
    INT32U crrentnum = EVENT_OBJS.Event3119_obj.crrentnum;

    //标准事件记录单元
    Save_buf[0] = 0x33;
    Save_buf[1] = 0x01;
    // Type Struct
    Save_buf[2] = 0x02;
    //单元数量
    Save_buf[3] = 0x05;
    //事件记录序号
    Save_buf[4] = 0x06;
    memcpy(&Save_buf[5], &crrentnum, sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生事件
    Save_buf[9] = 0x1C;
    memcpy(&Save_buf[10], &ntime, sizeof(ntime));
    //事件结束时间
    Save_buf[17] = 0x1C;
    memcpy(&Save_buf[18], &ntime, sizeof(ntime));

    //    事件发生源∷=enum
    //    {
    //    短路（0），
    //    开路（1），
    //    }

    Save_buf[25] = 0x16;
    Save_buf[26] = type && 0xff;

    //事件上报状态
    Save_buf[27] = 0x01;
    Save_buf[28] = 0x01;
    //通道号
    Save_buf[29] = 0x33;
    Save_buf[30] = 0x00;
    Save_buf[31] = 0x02;
    Save_buf[32] = 0x00;
    Save_buf[33] = 0x01;
    //上报状态
    Save_buf[34] = 0x11;
    Save_buf[35] = 0x00;

    EVENT_OBJS.Event3119_obj.crrentnum++;

    return 1;
}

INT8U GetEvent(INT32U type, E3301_Object* data) {
    INT8U File_buf[128];
    bzero(File_buf, sizeof(File_buf));
    //通过存储获取文件buf
    memcpy(data->id, &File_buf[5], sizeof(INT32U));

    DateTimeBCD ntime;
    DataTimeGet(&ntime);

    //事件发生时间
    memcpy(&data->begin, &File_buf[10], sizeof(ntime));
    //事件结束时间
    memcpy(&data->end, &File_buf[18], sizeof(ntime));
}
