#include "config.h"


DisplayPage_t xdata sys_ui;  // 定义系统UI状态结构体变量

// 上次显示值跟踪
static uint16_t xdata last_display_real_time_voltage = 0xFFFF;
static uint16_t xdata last_display_real_time_current = 0xFFFF;
static uint16_t xdata last_display_real_time_temperature = 0xFFFF;
static uint8_t xdata last_display_output_state = 0xFF;

// 多模式跟踪
static uint16_t xdata last_display_setting_value = 0xFFFF;  // 统一设定值跟踪
static uint8_t xdata last_op_mode = 0xFF;                   // 上次显示的模式，0xFF强制刷新
static uint8_t xdata disp_buf[24];                      // sprintf 显示缓冲区

// 按键状态
static uint8_t s_mode_long_handled = 0;                // MODE长按已处理标志

// 设置页状态变量
static SettingSubState_e idata s_setting_sub_state;
static uint8_t idata s_setting_index;
static uint8_t idata s_setting_scroll_offset;
static uint8_t idata s_edit_sel_index;
static uint16_t idata s_edit_temp_value;


void UI_Init(void) {
    sys_ui.current_page = SYS_STATE_START_PAGE;  // 初始化当前页面为启动页
    sys_ui.ui_data.real_time_voltage_mV = 0;     // 初始化实时电压值
    sys_ui.ui_data.real_time_current_mA = 0;     // 初始化实时电流值
    sys_ui.ui_data.op_mode = OP_MODE_CC;         // 初始化工作模式为恒流模式
    sys_ui.ui_data.setting_current_mA = 1000;       // 初始化设置电流值
    sys_ui.ui_data.setting_resistance_mOhm = 5000;       // 初始化设置电阻值
    sys_ui.ui_data.setting_power_mW = 2500;             // 初始化设置功率
    sys_ui.ui_data.real_time_temperature_C = 0;  // 初始化实时温度值
    sys_ui.ui_data.device_address = 21;           // 初始化设备地址
    sys_ui.ui_data.max_current_mA = 3000;       // 初始化最大电流值3A
    sys_ui.ui_data.max_voltage_mV = 5500;      // 初始化最大电压值5.5V
    sys_ui.ui_data.max_temperature_C = 80;         // 初始化 OTP 阈值 80°C
    sys_ui.ui_data.is_over_voltage = 0;          // 初始化过压报警标志
    sys_ui.ui_data.is_over_temperature = 0;      // 初始化过温报警标志
    sys_ui.is_changed = 1;                 // 页面切换标志设为1，表示需要刷新显示
    s_mode_long_handled = 0;               // 清除MODE长按标记
}


// ========== 子菜单辅助函数 ==========

#define MENU_ITEM_COUNT     7

static uint8_t MenuItem_IsActive(uint8_t index)
{
    uint8_t mode_bit;

    if (index == 0 || index == 4 || index == 5 || index == 6)
        return 1;
    mode_bit = 1 << (uint8_t)sys_ui.ui_data.op_mode;
    if (index == 1 && (mode_bit & 0x01)) return 1;
    if (index == 2 && (mode_bit & 0x02)) return 1;
    if (index == 3 && (mode_bit & 0x04)) return 1;
    return 0;
}

static const char code * MenuItem_GetName(uint8_t index)
{
    static const char code * const names[MENU_ITEM_COUNT] = {
        "Mode Select", "Target I", "Target R", "Target P",
        "OVP Limit",   "OTP Limit", "RS485 Addr"
    };
    return names[index];
}

static void MenuItem_FormatValue(uint8_t index, uint8_t *buf)
{
    uint16_t val;
    switch (index) {
    case 1:
        val = sys_ui.ui_data.setting_current_mA;
        sprintf((char*)buf, "%d.%03d A", (int)(val / 1000), (int)(val % 1000));
        break;
    case 2:
        val = sys_ui.ui_data.setting_resistance_mOhm;
        sprintf((char*)buf, "%d.%03d R", (int)(val / 1000), (int)(val % 1000));
        break;
    case 3:
        val = sys_ui.ui_data.setting_power_mW;
        sprintf((char*)buf, "%d.%03d W", (int)(val / 1000), (int)(val % 1000));
        break;
    case 4:
        val = sys_ui.ui_data.max_voltage_mV;
        sprintf((char*)buf, "%d.%02d V", (int)(val / 1000), (int)((val % 1000) / 10));
        break;
    case 5:
        val = sys_ui.ui_data.max_temperature_C;
        sprintf((char*)buf, "%d C", (int)val);
        break;
    case 6:
        val = sys_ui.ui_data.device_address;
        sprintf((char*)buf, "#%d", (int)val);
        break;
    default: 
        buf[0] = 0; 
        break;
    }
}

static uint16_t* MenuItem_GetEditParams(uint8_t index, uint16_t *p_min, uint16_t *p_max, uint16_t *p_step)
{
    uint16_t min_v = 0, max_v = 0, step_v = 0;
    uint16_t *ret_ptr = NULL;

    switch (index) {
    case 1:  min_v = 100;   max_v = 3000;    step_v = 10;   ret_ptr = &sys_ui.ui_data.setting_current_mA; break;
    case 2:  min_v = 0;     max_v = 100000;  step_v = 100;  ret_ptr = &sys_ui.ui_data.setting_resistance_mOhm; break;
    case 3:  min_v = 0;     max_v = 50000;   step_v = 100;  ret_ptr = &sys_ui.ui_data.setting_power_mW; break;
    case 4:  min_v = 0;     max_v = 60000;   step_v = 100;  ret_ptr = &sys_ui.ui_data.max_voltage_mV; break;
    case 5:  min_v = 0;     max_v = 150;     step_v = 5;    ret_ptr = &sys_ui.ui_data.max_temperature_C; break;
    case 6:  min_v = 1;     max_v = 247;     step_v = 1;    ret_ptr = &sys_ui.ui_data.device_address; break;
    default: return NULL;
    }
    
    if (p_min != NULL) *p_min = min_v;
    if (p_max != NULL) *p_max = max_v;
    if (p_step != NULL) *p_step = step_v;
    
    return ret_ptr;
}

static void OLED_ShowCharDimmed(u8 x, u8 y, u8 chr)
{
    unsigned char c = chr - ' ';
    uint16_t offset = (uint16_t)c << 4;
    u8 i;

    // 上8像素: 批量写入
    OLED_Set_Pos(x, y);
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
    Write_IIC_Byte(0x40);           IIC_Wait_Ack();
    for (i = 0; i < 8; i++) {
        Write_IIC_Byte(F8X16[offset + i] & 0x55); IIC_Wait_Ack();
    }
    IIC_Stop();

    // 下8像素: 批量写入
    OLED_Set_Pos(x, y + 1);
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
    Write_IIC_Byte(0x40);           IIC_Wait_Ack();
    for (i = 0; i < 8; i++) {
        Write_IIC_Byte(F8X16[offset + i + 8] & 0x55); IIC_Wait_Ack();
    }
    IIC_Stop();
}

static uint8_t FindNextActiveItem(uint8_t current, int8_t direction)
{
    uint8_t next = current;
    uint8_t i;
    for (i = 0; i < MENU_ITEM_COUNT; i++) {
        if (direction > 0) {
            next = (next + 1 >= MENU_ITEM_COUNT) ? 0 : next + 1;
        } else {
            next = (next == 0) ? MENU_ITEM_COUNT - 1 : next - 1;
        }
        if (MenuItem_IsActive(next))
            return next;
        if (next == current) break;
    }
    return current;
}

// ========== 设置页渲染函数 ==========

static void DrawSettingList(void)
{
    uint8_t i;
    uint8_t col = 16;

    OLED_Clear(0);

    // ��ʾ�˵��� (ҳ 2 ��  ҳ 7) ��ʹ�õ���I2C������
    for (i = 2; i <= 7; i++) {
        uint8_t j;
        OLED_Set_Pos(0, i);
        IIC_Start();
        Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
        Write_IIC_Byte(0x40);           IIC_Wait_Ack();
        for (j = 0; j < 128; j++) {
            Write_IIC_Byte(0);          IIC_Wait_Ack();
        }
        IIC_Stop();
    }

    // 标题
    OLED_ShowString(0, 0, "--- SETTING ---", 16);

    // 最多绘制 3 个可见项
    for (i = 0; i < 3; i++) {
        uint8_t line_y = 2 + i * 2;
        uint8_t item_idx = s_setting_scroll_offset + i;
        uint8_t active = MenuItem_IsActive(item_idx);
        uint8_t is_cur = (item_idx == s_setting_index);

        if (item_idx >= MENU_ITEM_COUNT) 
            break;

        OLED_ShowChar(0, line_y, is_cur ? '>' : ' ', 16);
        OLED_ShowChar(col, line_y, '0' + item_idx, 16);
        OLED_ShowChar(col + 8, line_y, '.', 16);

        {
            const char code *name = MenuItem_GetName(item_idx);
            uint8_t j = 0;
            while (name[j] != '\0' && j < 12) {
                if (active) {
                    OLED_ShowChar(col + 16 + j * 8, line_y, name[j], 16);
                } else {
                    OLED_ShowCharDimmed(col + 16 + j * 8, line_y, name[j]);
                }
                j++;
            }
        }
    }

    // // 进度指示 (y=6 右侧)
    // if (MENU_ITEM_COUNT > 3) {
    //     disp_buf[0] = '0' + s_setting_index + 1;
    //     disp_buf[1] = '/';
    //     disp_buf[2] = '0' + MENU_ITEM_COUNT;
    //     disp_buf[3] = '\0';
    //     OLED_ShowString(128 - 3 * 8, 7, disp_buf, 12);
    // }
}

static void DrawEditValue(uint8_t item_idx)
{
    const char code *name = MenuItem_GetName(item_idx);
    uint8_t len;
    int16_t start_x;

    OLED_Clear(0);

    sprintf((char*)disp_buf, "-- %s --", name);
    len = strlen((char*)disp_buf);
    
    start_x = (128 - len * 8) / 2;
    if (start_x < 0) start_x = 0;

    OLED_ShowString((u8)start_x, 0, disp_buf, 16);

    MenuItem_FormatValue(item_idx, disp_buf);
    len = strlen((char*)disp_buf);
    start_x = (128 - (int16_t)len * 8) / 2;
    if (start_x < 0) start_x = 0;
    OLED_ShowString((u8)start_x, 4, disp_buf, 16);
}

static void DrawModeSelect(void)
{
    static const char code * const modes[3] = { "CC", "CR", "CP" };
    uint8_t i;

    OLED_Clear(0);
    OLED_ShowString(0, 0, "- Mode Select -", 16);

    for (i = 0; i < 3; i++) {
        uint8_t y = 2 + i * 2;
        OLED_ShowChar(0, y, (i == s_edit_sel_index) ? '>' : ' ', 16);
        OLED_ShowString(16, y, (uint8_t *)modes[i], 16);
    }
}

/*
 * @brief   UI 显示主任务
 */
void UI_Task(void)
{
    // 页面切换时的单次初始化
    if (sys_ui.is_changed) 
    {
        OLED_Clear(0); // 切换页面先清屏
        
        switch (sys_ui.current_page) 
        {   
            case SYS_STATE_START_PAGE:
                OLED_ShowString(0, 0, "DC E-LOAD V1.0", 16);
                OLED_ShowString(0, 2, "User:", 16);
                OLED_ShowChinese(0 + 16 * 3, 2, fu2);  // '傅'
                OLED_ShowChinese(0 + 16 * 4, 2, bei);  // '倍'
                OLED_ShowChinese(0 + 16 * 5, 2, liang);  // '梁'
                OLED_ShowString(0, 4, "No.:0521", 12);
                OLED_ShowString(0, 5, "Class:EE", 12);
                OLED_ShowString(0, 7, "Initializing...", 12);
                OLED_DrawBMP(90, 4, 32, 32, Icon_32x32_School_Emble); // 显示图标
                break;

            case SYS_STATE_MONITOR_PAGE:
                // === Line 1: 实时电压 (16px, y=0) ===
                sprintf((char*)disp_buf, "V_in :%2d.%03d V",
                    (int)(sys_ui.ui_data.real_time_voltage_mV / 1000),
                    (int)(sys_ui.ui_data.real_time_voltage_mV % 1000));
                OLED_ShowString(0, 0, disp_buf, 16);

                // === Line 2: 实时电流 (16px, y=2) ===
                sprintf((char*)disp_buf, "I_out:%2d.%03d A",
                    (int)(sys_ui.ui_data.real_time_current_mA / 1000),
                    (int)(sys_ui.ui_data.real_time_current_mA % 1000));
                OLED_ShowString(0, 2, disp_buf, 16);

                // === Line 3: 设定值 (16px, y=4)，按模式显示 ===
                switch (sys_ui.ui_data.op_mode) {
                case OP_MODE_CC:
                    sprintf((char*)disp_buf, "I_set:%2d.%03d A",
                        (int)(sys_ui.ui_data.setting_current_mA / 1000),
                        (int)(sys_ui.ui_data.setting_current_mA % 1000));
                    break;
                case OP_MODE_CR:
                    sprintf((char*)disp_buf, "R_set:%2d.%03d R",
                        (int)(sys_ui.ui_data.setting_resistance_mOhm / 1000),
                        (int)(sys_ui.ui_data.setting_resistance_mOhm % 1000));
                    break;
                case OP_MODE_CP:
                    sprintf((char*)disp_buf, "P_set:%2d.%03d W",
                        (int)(sys_ui.ui_data.setting_power_mW / 1000),
                        (int)(sys_ui.ui_data.setting_power_mW % 1000));
                    break;
                }
                OLED_ShowString(0, 4, disp_buf, 16);

                // === Line 4: 状态栏 (16x16图标 + 16px文字, y=6) ===
                OLED_ShowChinese(0, 6, _CC + sys_ui.ui_data.op_mode);
                if (OUTPUT_EN_IS_ON) {
                sprintf((char*)disp_buf, "ON  T:%03d #%02d",
                    (int)(sys_ui.ui_data.real_time_temperature_C / 100),
                    (int)(sys_ui.ui_data.device_address));
                } else {
                    sprintf((char*)disp_buf, "OFF T:%03d #%02d",
                    (int)(sys_ui.ui_data.real_time_temperature_C / 100),
                    (int)(sys_ui.ui_data.device_address));
                }
                OLED_ShowString(24, 6, disp_buf, 16);
                break;

            case SYS_STATE_SETTING_PAGE:
                s_setting_sub_state = SETTING_SUB_LIST;
                s_setting_index = 0;
                s_setting_scroll_offset = 0;
                DrawSettingList();
                break;
            
            case SYS_STATE_AUTO_TEST_PAGE:
                // 暂时没用
                break;
            
            case SYS_STATE_ALARM_OVERLAY:
                // 暂时没用
                break;

            default:
                break;
        }
        // 强制跟踪变量与 UI 同步，确保动态刷新能检测到变化
        last_display_setting_value = 0xFFFF;
        last_display_output_state = 0xFF;
        sys_ui.is_changed = 0; // 标记初始化完成
    }

    // 平稳运行时的动态刷新
    switch (sys_ui.current_page) 
    {
        case SYS_STATE_START_PAGE:
                // 启动页通常不需要频繁刷新数值，可以在这里添加一些动画效果或者保持静态显示
            break;
            
        case SYS_STATE_MONITOR_PAGE:
        {
            // --- 检测模式变化（影响 Line 3 标签和 Line 4 图标）---
            if (last_op_mode != (uint8_t)sys_ui.ui_data.op_mode) {
                last_op_mode = (uint8_t)sys_ui.ui_data.op_mode;
                last_display_setting_value = 0xFFFF;  // 强制 Line 3 重绘
                last_display_real_time_temperature = 0xFFFF; // 强制 Line 4 重绘
            }

            // --- Line 1: 电压 (16px, y=0) ---
            // 修复：增加对未初始化状态(0xFFFF)的放行判断
            if (last_display_real_time_voltage == 0xFFFF || 
                abs((int16_t)last_display_real_time_voltage - (int16_t)sys_ui.ui_data.real_time_voltage_mV) > 10) {
                sprintf((char*)disp_buf, "V_in :%2d.%03d V",
                    (int)(sys_ui.ui_data.real_time_voltage_mV / 1000),
                    (int)(sys_ui.ui_data.real_time_voltage_mV % 1000));
                OLED_ShowString(0, 0, disp_buf, 16);
                last_display_real_time_voltage = sys_ui.ui_data.real_time_voltage_mV;
            }

            // --- Line 2: 电流 (16px, y=2) ---
            if (last_display_real_time_current == 0xFFFF || 
                abs((int16_t)last_display_real_time_current - (int16_t)sys_ui.ui_data.real_time_current_mA) > 5) {
                sprintf((char*)disp_buf, "I_out:%2d.%03d A",
                    (int)(sys_ui.ui_data.real_time_current_mA / 1000),
                    (int)(sys_ui.ui_data.real_time_current_mA % 1000));
                OLED_ShowString(0, 2, disp_buf, 16);
                last_display_real_time_current = sys_ui.ui_data.real_time_current_mA;
            }

            // --- Line 3: 设定值 (16px, y=4) ---
            {
                uint16_t cur_val;
                switch (sys_ui.ui_data.op_mode) {
                case OP_MODE_CC: 
                    cur_val = sys_ui.ui_data.setting_current_mA; 
                    break;
                case OP_MODE_CR: 
                    cur_val = sys_ui.ui_data.setting_resistance_mOhm; 
                    break;
                case OP_MODE_CP: 
                    cur_val = sys_ui.ui_data.setting_power_mW; 
                    break;
                default:         
                    cur_val = 0; 
                    break;
                }
                if (last_display_setting_value != cur_val) {
                    last_display_setting_value = cur_val;
                    switch (sys_ui.ui_data.op_mode) {
                    case OP_MODE_CC:
                        sprintf((char*)disp_buf, "I_set:%2d.%03d A",
                            (int)(sys_ui.ui_data.setting_current_mA / 1000),
                            (int)(sys_ui.ui_data.setting_current_mA % 1000));
                        break;
                    case OP_MODE_CR:
                        sprintf((char*)disp_buf, "R_set:%2d.%03d R",
                            (int)(sys_ui.ui_data.setting_resistance_mOhm / 1000),
                            (int)(sys_ui.ui_data.setting_resistance_mOhm % 1000));
                        break;
                    case OP_MODE_CP:
                        sprintf((char*)disp_buf, "P_set:%2d.%03d W",
                            (int)(sys_ui.ui_data.setting_power_mW / 1000),
                            (int)(sys_ui.ui_data.setting_power_mW % 1000));
                        break;
                    }
                    OLED_ShowString(0, 4, disp_buf, 16);
                }
            }

            // --- Line 4: 状态栏 (y=6) ---
            // 判定条件：开关状态变了 OR 温度变了
            if (last_display_output_state != OUTPUT_EN_IS_ON || 
                last_display_real_time_temperature == 0xFFFF || 
                abs((int16_t)last_display_real_time_temperature - (int16_t)sys_ui.ui_data.real_time_temperature_C) > 5) 
            {
                // 更新所有跟踪变量
                last_display_output_state = OUTPUT_EN_IS_ON;
                last_display_real_time_temperature = sys_ui.ui_data.real_time_temperature_C;

                // 重绘模式图标
                OLED_ShowChinese(0, 6, _CC + sys_ui.ui_data.op_mode);
                
                // 格式化并显示（按你之前的要求，包含一位小数）
                if (OUTPUT_EN_IS_ON) {
                    sprintf((char*)disp_buf, "ON  T:%03d #%02d",
                        (int)(sys_ui.ui_data.real_time_temperature_C / 100),
                        (int)(sys_ui.ui_data.device_address));
                } else {
                    sprintf((char*)disp_buf, "OFF T:%03d #%02d",
                        (int)(sys_ui.ui_data.real_time_temperature_C / 100),
                        (int)(sys_ui.ui_data.device_address));
                }
                OLED_ShowString(24, 6, disp_buf, 16);
            }
            break;
        }

        case SYS_STATE_SETTING_PAGE:
                // 设置页的动态刷新主要在编辑模式下，根据当前编辑项动态更新数值显示
            break;

        case SYS_STATE_AUTO_TEST_PAGE:
                // 自动测试页可以根据测试进度动态更新显示内容
            break;

        case SYS_STATE_ALARM_OVERLAY:
                // 报警覆盖页可以根据报警状态动态显示不同的报警信息
            break;

        default:
            break;
    }
}


/*
 * @brief   按键事件处理回调函数
 * @note    此函数由 key.c 的状态机自动调用，我们在这里集中处理 UI 逻辑
 */
void Key_Event_Handler(KeyID_e key_id, KeyEvent_e event)
{
    uint16_t *p_val;
    uint8_t next;

    if (event == KEY_EVENT_UP)
    {
        uint16_t min_v, max_v, step;
        Buzzer_KeyBeep();
        switch (key_id)
        {
            case KEY_ID_MODE:
                // 如果刚发生过 MODE 长按，则跳过本次短按的页面切换
                if (s_mode_long_handled) {
                    s_mode_long_handled = 0; // 消耗长按标记
                    break;
                }
                // 设置页内部 MODE 短按：子状态切换
                if (sys_ui.current_page == SYS_STATE_SETTING_PAGE)
                {
                    if (s_setting_sub_state == SETTING_SUB_LIST) {
                        if (s_setting_index == 0) {
                            // Mode Select: 进入子菜单
                            s_setting_sub_state = SETTING_SUB_MODE_SELECT;
                            s_edit_sel_index = (uint8_t)sys_ui.ui_data.op_mode;
                            DrawModeSelect();
                        } else if (MenuItem_IsActive(s_setting_index)) {
                            // 进入编辑模式
                            s_setting_sub_state = SETTING_SUB_EDIT;
                            // 保存当前值供取消时恢复
                            p_val = MenuItem_GetEditParams(s_setting_index, NULL, NULL, NULL);
                            if (p_val) s_edit_temp_value = *p_val;
                            DrawEditValue(s_setting_index);
                        }
                    } else if (s_setting_sub_state == SETTING_SUB_EDIT) {
                        // 确认修改 → 保存到 EEPROM
                        Settings_Save();
                        s_setting_sub_state = SETTING_SUB_LIST;
                        sys_ui.is_changed = 1;
                    } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                        // 确认模式选择
                        sys_ui.ui_data.op_mode = (OpMode_e)s_edit_sel_index;
                        Settings_Save();
                        s_setting_sub_state = SETTING_SUB_LIST;
                        sys_ui.is_changed = 1;
                    }
                    break;  // 不执行下面的页面切换
                }
                // 按下 MODE 键（短按）：切换页面
                sys_ui.current_page = sys_ui.current_page + 1;
                if (sys_ui.current_page > SYS_STATE_SETTING_PAGE) {
                    sys_ui.current_page = SYS_STATE_START_PAGE; // 循环回第一个页面
                }
                sys_ui.is_changed = 1; // 标记状态改变，触发 UI_Task 清屏重绘
                break;

            case KEY_ID_INC:
                switch (sys_ui.current_page) {
                    case SYS_STATE_SETTING_PAGE:
                        if (s_setting_sub_state == SETTING_SUB_LIST) {
                            next = FindNextActiveItem(s_setting_index, -1);
                            if (next != s_setting_index) {
                                s_setting_index = next;
                                if (s_setting_index < s_setting_scroll_offset)
                                    s_setting_scroll_offset = s_setting_index;
                                else if (s_setting_index >= s_setting_scroll_offset + 3)
                                    s_setting_scroll_offset = s_setting_index - 2;
                                DrawSettingList();
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_EDIT) {
                            p_val = MenuItem_GetEditParams(s_setting_index, &min_v, &max_v, &step);
                            if (p_val) {
                                if (*p_val + step <= max_v) *p_val += step;
                                else *p_val = max_v;
                                DrawEditValue(s_setting_index);
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                            if (s_edit_sel_index > 0) s_edit_sel_index--;
                            DrawModeSelect();
                        }
                        break;
                    case SYS_STATE_MONITOR_PAGE:
                        // 根据当前工作模式调节对应的设定值
                        switch (sys_ui.ui_data.op_mode) {
                        case OP_MODE_CC:
                            sys_ui.ui_data.setting_current_mA += 10;
                            if (sys_ui.ui_data.setting_current_mA > sys_ui.ui_data.max_current_mA)
                                sys_ui.ui_data.setting_current_mA = sys_ui.ui_data.max_current_mA;
                            break;
                        case OP_MODE_CR:
                            if (sys_ui.ui_data.setting_resistance_mOhm <= 65535 - 100)
                                sys_ui.ui_data.setting_resistance_mOhm += 100;
                            break;
                        case OP_MODE_CP:
                            if (sys_ui.ui_data.setting_power_mW <= 30000 - 100)
                                sys_ui.ui_data.setting_power_mW += 100;
                            break;
                        }
                        break;
                    default:
                        break;
                }
                break;

            case KEY_ID_DEC:
                switch (sys_ui.current_page) {
                    case SYS_STATE_SETTING_PAGE:
                        if (s_setting_sub_state == SETTING_SUB_LIST) {
                            next = FindNextActiveItem(s_setting_index, 1);
                            if (next != s_setting_index) {
                                s_setting_index = next;
                                if (s_setting_index < s_setting_scroll_offset)
                                    s_setting_scroll_offset = s_setting_index;
                                else if (s_setting_index >= s_setting_scroll_offset + 3)
                                    s_setting_scroll_offset = s_setting_index - 2;
                                DrawSettingList();
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_EDIT) {
                            p_val = MenuItem_GetEditParams(s_setting_index, &min_v, &max_v, &step);
                            if (p_val) {
                                if (*p_val >= min_v + step) *p_val -= step;
                                else *p_val = min_v;
                                DrawEditValue(s_setting_index);
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                            if (s_edit_sel_index < 2) s_edit_sel_index++;
                            DrawModeSelect();
                        }
                        break;
                    case SYS_STATE_MONITOR_PAGE:
                        switch (sys_ui.ui_data.op_mode) {
                        case OP_MODE_CC:
                            if (sys_ui.ui_data.setting_current_mA >= 110)
                                sys_ui.ui_data.setting_current_mA -= 10;
                            else
                                sys_ui.ui_data.setting_current_mA = 100;
                            break;
                        case OP_MODE_CR:
                            if (sys_ui.ui_data.setting_resistance_mOhm >= 1100)
                                sys_ui.ui_data.setting_resistance_mOhm -= 100;
                            else
                                sys_ui.ui_data.setting_resistance_mOhm = 1000;
                            break;
                        case OP_MODE_CP:
                            if (sys_ui.ui_data.setting_power_mW >= 100)
                                sys_ui.ui_data.setting_power_mW -= 100;
                            else
                                sys_ui.ui_data.setting_power_mW = 0;
                            break;
                        }
                        break;
                    default:
                        break;
                }
                break;

            case KEY_ID_ONOFF:
                if (sys_ui.current_page == SYS_STATE_SETTING_PAGE) {
                    // 设置页中：取消当前编辑或子菜单
                    if (s_setting_sub_state == SETTING_SUB_EDIT) {
                        p_val = MenuItem_GetEditParams(s_setting_index, NULL, NULL, NULL);
                        if (p_val) *p_val = s_edit_temp_value;
                        s_setting_sub_state = SETTING_SUB_LIST;
                        sys_ui.is_changed = 1;
                    } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                        s_setting_sub_state = SETTING_SUB_LIST;
                        sys_ui.is_changed = 1;
                    }
                } else {
                    OUTPUT_EN_TOGGLE(); // 每次按ON/OFF键切换OUTPUT_EN控制的开关状态
                    if (!OUTPUT_EN_IS_ON) {
                        Current_Control_ResetDuty(); // 关闭输出时复位占空比
                    }
                    printf("OUTPUT_EN is now %s\n", OUTPUT_EN_IS_ON ? "ON" : "OFF");
                }
                break;
        }
    }
    
    // 处理连按事件 (比如长按 INC 键快速增加数值)
    else if (event == KEY_EVENT_REPEAT)
    {
        uint16_t min_v, max_v, step;
        Buzzer_KeyBeep();
        switch (key_id)
        {
            case KEY_ID_INC:
                switch (sys_ui.current_page) {
                    case SYS_STATE_SETTING_PAGE:
                        if (s_setting_sub_state == SETTING_SUB_EDIT) {
                            p_val = MenuItem_GetEditParams(s_setting_index, &min_v, &max_v, &step);
                            if (p_val) {
                                if (*p_val + step <= max_v) *p_val += step;
                                else *p_val = max_v;
                                DrawEditValue(s_setting_index);
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                            if (s_edit_sel_index < 2) s_edit_sel_index++;
                            DrawModeSelect();
                        }
                        break;
                    case SYS_STATE_MONITOR_PAGE:
                        switch (sys_ui.ui_data.op_mode) {
                        case OP_MODE_CC:
                            sys_ui.ui_data.setting_current_mA += 10;
                            if (sys_ui.ui_data.setting_current_mA > sys_ui.ui_data.max_current_mA)
                                sys_ui.ui_data.setting_current_mA = sys_ui.ui_data.max_current_mA;
                            break;
                        case OP_MODE_CR:
                            if (sys_ui.ui_data.setting_resistance_mOhm <= 65535 - 100)
                                sys_ui.ui_data.setting_resistance_mOhm += 100;
                            break;
                        case OP_MODE_CP:
                            if (sys_ui.ui_data.setting_power_mW <= 30000 - 100)
                                sys_ui.ui_data.setting_power_mW += 100;
                            break;
                        }
                        break;
                    default:
                        break;
                }
                break;
                
            case KEY_ID_DEC:
                switch (sys_ui.current_page) {
                    case SYS_STATE_SETTING_PAGE:
                        if (s_setting_sub_state == SETTING_SUB_EDIT) {
                            p_val = MenuItem_GetEditParams(s_setting_index, &min_v, &max_v, &step);
                            if (p_val) {
                                if (*p_val >= min_v + step) *p_val -= step;
                                else *p_val = min_v;
                                DrawEditValue(s_setting_index);
                            }
                        } else if (s_setting_sub_state == SETTING_SUB_MODE_SELECT) {
                            if (s_edit_sel_index > 0) s_edit_sel_index--;
                            DrawModeSelect();
                        }
                        break;
                    case SYS_STATE_MONITOR_PAGE:
                        switch (sys_ui.ui_data.op_mode) {
                        case OP_MODE_CC:
                            if (sys_ui.ui_data.setting_current_mA >= 110)
                                sys_ui.ui_data.setting_current_mA -= 10;
                            else
                                sys_ui.ui_data.setting_current_mA = 100;
                            break;
                        case OP_MODE_CR:
                            if (sys_ui.ui_data.setting_resistance_mOhm >= 1100)
                                sys_ui.ui_data.setting_resistance_mOhm -= 100;
                            else
                                sys_ui.ui_data.setting_resistance_mOhm = 1000;
                            break;
                        case OP_MODE_CP:
                            if (sys_ui.ui_data.setting_power_mW >= 100)
                                sys_ui.ui_data.setting_power_mW -= 100;
                            else
                                sys_ui.ui_data.setting_power_mW = 0;
                            break;
                        }
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    // 处理长按事件
    else if (event == KEY_EVENT_LONG)
    {
        Buzzer_KeyBeep();
        switch (key_id)
        {
        case KEY_ID_MODE:
            if (sys_ui.current_page == SYS_STATE_SETTING_PAGE) {
                // 长按退出设置页；编辑中则取消不保存
                if (s_setting_sub_state == SETTING_SUB_EDIT) {
                    p_val = MenuItem_GetEditParams(s_setting_index, NULL, NULL, NULL);
                    if (p_val) *p_val = s_edit_temp_value;
                }
                sys_ui.current_page = SYS_STATE_MONITOR_PAGE;
                sys_ui.is_changed = 1;
                s_mode_long_handled = 1;
            } else if (sys_ui.current_page == SYS_STATE_MONITOR_PAGE) {
                // 长按 MODE 在监视页循环切换 CC→CR→CP
                sys_ui.ui_data.op_mode = (sys_ui.ui_data.op_mode + 1) % 3;
                s_mode_long_handled = 1; // 标记，避免松开时触发页面切换
                printf("Mode: %s\n",
                    sys_ui.ui_data.op_mode == OP_MODE_CC ? "CC" :
                    sys_ui.ui_data.op_mode == OP_MODE_CR ? "CR" : "CP");
                OUTPUT_EN_OFF(); // 长按 MODE 键时先关闭输出，避免切换模式时的瞬间过流过压
            }
            break;
        default:
            break;
        }
    }
}
