#include "config.h"


// ���������������飺˳������Ϊ MODE(0), INC(1), DEC(2), ONOFF(3)
const uint8_t KeyLongPressMIN[KEY_NUM]   = {50, 0, 0, 0};          // ������ֵ��0Ϊ����
const uint8_t KeyRepeatPressMIN[KEY_NUM] = {255, 10, 10, 255};    // ������ֵ��255Ϊ����

// �ڲ�״̬���ڴ����
static uint8_t mKeySwapTask[KEY_NUM] = {0};
static uint8_t mKeyLongPressCNT[KEY_NUM] = {0};
static uint8_t mKeyRepeatPressCNT[KEY_NUM] = {0};
static uint8_t mFirstDown[KEY_NUM] = {1,1,1,1};  // 首次按下标记，用于蜂鸣器单次触发


/*
 * @brief   ����״̬��ɨ������
 * @note    ������� 10ms �Ķ�ʱ����־λ����ѯ
 */
void Key_Task(void)
{
    uint8_t i;
    uint8_t key_level[KEY_NUM];

    // ��ȡ�������������ĵ�ǰ��ƽ (0Ϊ���£�1Ϊ�ͷ�)
    key_level[KEY_ID_MODE]  = KEY_MODE;
    key_level[KEY_ID_INC]   = KEY_INC;
    key_level[KEY_ID_DEC]   = KEY_DEC;
    key_level[KEY_ID_ONOFF] = KEY_ON_OFF;

    // ״̬����ѯ����
    for (i = 0; i < KEY_NUM; i++)
    {
        switch (mKeySwapTask[i])
        {
            case 0:  // ����Ƿ��а���
                mFirstDown[i] = 1;  // 为下一次按键重置首次按下标记
                if (key_level[i] == 0) mKeySwapTask[i] = 1;
                break;

            case 1:  // ����ȥ����ʱ
                mKeySwapTask[i] = 2;
                break;

            case 2:  // ȷ�ϰ���
                if (key_level[i] == 0) {
                    if (mFirstDown[i]) {
                    mFirstDown[i] = 0;
                    Buzzer_KeyBeep();
                    }
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_DOWN); // �����������¼���
                    mKeyLongPressCNT[i] = 0;
                    mKeyRepeatPressCNT[i] = 0;
                    mKeySwapTask[i] = 3;
                } else {
                    mKeySwapTask[i] = 0; // ��������λ
                    }
                break;

            case 3:  // ��ⰴס���ɿ�
                if (key_level[i] == 1) {
                    mKeySwapTask[i] = 4; // �����ɿ��������ͷ�ȥ��
                    }

                // �������
                if (KeyLongPressMIN[i] > 0 && mKeyLongPressCNT[i] < KeyLongPressMIN[i]) {
                    mKeyLongPressCNT[i]++;
                    if (mKeyLongPressCNT[i] == KeyLongPressMIN[i]) {
                            Buzzer_KeyBeep();
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_LONG); // �����������¼���
                    }
                    }

                // �������
                if (KeyRepeatPressMIN[i] < 255) {
                    mKeyRepeatPressCNT[i]++;
                    if (mKeyRepeatPressCNT[i] == KeyRepeatPressMIN[i]) {
                        Buzzer_ShortTick();
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_REPEAT); // �����������¼���
                    }
                    if (mKeyRepeatPressCNT[i] > KeyRepeatPressMIN[i]) {
                        mKeySwapTask[i] = 2; // ����״̬��������һ������ѭ��
                    }
                    }
                break;

            case 4:  // �ͷ�ȥ����ʱ
                mKeySwapTask[i] = 5;
                break;

            case 5:  // ȷ���ͷ�
                if (key_level[i] == 1) {
                    Key_Event_Handler((KeyID_e)i, KEY_EVENT_UP); // �������ͷ��¼���
                    mKeySwapTask[i] = 0;
                } else {
                    mKeySwapTask[i] = 3; // �������˻ذ�ס״̬
                    }
                break;

            default:
                mKeySwapTask[i] = 0;
                break;
        }
    }
}


