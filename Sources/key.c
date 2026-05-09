#include "config.h"


// 按键参数配置数组：顺序依次为 MODE(0), INC(1), DEC(2), ONOFF(3)
const uint8_t KeyLongPressMIN[KEY_NUM]   = {50, 0, 0, 0};          // 长按阈值，0为禁用
const uint8_t KeyRepeatPressMIN[KEY_NUM] = {255, 10, 10, 255};    // 连按阈值，255为禁用

// 内部状态机内存分配
static uint8_t mKeySwapTask[KEY_NUM] = {0};
static uint8_t mKeyLongPressCNT[KEY_NUM] = {0};
static uint8_t mKeyRepeatPressCNT[KEY_NUM] = {0};


/*
 * @brief   按键状态机扫描任务
 * @note    建议放入 10ms 的定时器标志位中轮询
 */
void Key_Task(void)
{
    uint8_t i;
    uint8_t key_level[KEY_NUM];

    // 获取所有物理按键的当前电平 (0为按下，1为释放)
    key_level[KEY_ID_MODE]  = KEY_MODE;
    key_level[KEY_ID_INC]   = KEY_INC;
    key_level[KEY_ID_DEC]   = KEY_DEC;
    key_level[KEY_ID_ONOFF] = KEY_ON_OFF;

    // 状态机轮询处理
    for (i = 0; i < KEY_NUM; i++)
    {
        switch (mKeySwapTask[i])
        {
            case 0:  // 检测是否有按下
                if (key_level[i] == 0) mKeySwapTask[i] = 1;
                break;

            case 1:  // 按下去抖延时
                mKeySwapTask[i] = 2;
                break;

            case 2:  // 确认按下
                if (key_level[i] == 0) {
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_DOWN); // 触发【按下事件】
                    mKeyLongPressCNT[i] = 0;
                    mKeyRepeatPressCNT[i] = 0;
                    mKeySwapTask[i] = 3;
                } else {
                    mKeySwapTask[i] = 0; // 抖动，复位
                }
                break;

            case 3:  // 检测按住与松开
                if (key_level[i] == 1) {
                    mKeySwapTask[i] = 4; // 按键松开，进入释放去抖
                }

                // 长按检测
                if (KeyLongPressMIN[i] > 0 && mKeyLongPressCNT[i] < KeyLongPressMIN[i]) {
                    mKeyLongPressCNT[i]++;
                    if (mKeyLongPressCNT[i] == KeyLongPressMIN[i]) {
                        Key_Event_Handler((KeyID_e)i, KEY_EVENT_LONG); // 触发【长按事件】
                    }
                }

                // 连按检测
                if (KeyRepeatPressMIN[i] < 255) {
                    mKeyRepeatPressCNT[i]++;
                    if (mKeyRepeatPressCNT[i] == KeyRepeatPressMIN[i]) {
                        Key_Event_Handler((KeyID_e)i, KEY_EVENT_REPEAT); // 触发【连按事件】
                    }
                    if (mKeyRepeatPressCNT[i] > KeyRepeatPressMIN[i]) {
                        mKeySwapTask[i] = 2; // 回退状态，开启下一次连击循环
                    }
                }
                break;

            case 4:  // 释放去抖延时
                mKeySwapTask[i] = 5;
                break;

            case 5:  // 确认释放
                if (key_level[i] == 1) {
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_UP); // 触发【释放事件】
                    mKeySwapTask[i] = 0;
                } else {
                    mKeySwapTask[i] = 3; // 抖动，退回按住状态
                }
                break;

            default:
                mKeySwapTask[i] = 0;
                break;
        }
    }
}


