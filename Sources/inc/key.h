#ifndef __KEY_H__
#define __KEY_H__


// 定义按键 ID 枚举
typedef enum {
    KEY_ID_MODE   = 0,
    KEY_ID_INC    = 1,
    KEY_ID_DEC    = 2,
    KEY_ID_ONOFF  = 3,
    KEY_NUM
} KeyID_e;

// 定义按键事件枚举
typedef enum {
    KEY_EVENT_DOWN,   // 按下瞬间
    KEY_EVENT_UP,     // 释放瞬间 (常用作单击触发)
    KEY_EVENT_LONG,   // 达到长按阈值
    KEY_EVENT_REPEAT  // 连按触发 (用于加减数值)
} KeyEvent_e;


// 核心扫描任务
void Key_Task(void);

// 按键事件回调函数
extern void Key_Event_Handler(KeyID_e key_id, KeyEvent_e event);


#endif

