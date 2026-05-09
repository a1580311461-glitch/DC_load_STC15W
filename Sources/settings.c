#include "config.h"

/*
 * Settings persistence using STC15W IAP.
 *
 * 底层 IAP 函数由 AICube 生成 (iap.c)：
 *   IAP_ReadByte(addr)     — 从 Flash 地址读取一个字节
 *   IAP_ProgramByte(addr,d) — 编程一个字节（必须先擦除）
 *   IAP_EraseSector(addr)   — 擦除包含 addr 的 512 字节扇区
 *
 * IAP（In-Application Programming）操作原理：
 *   STC15W 的内部 Flash 是 NOR Flash 类型。
 *   读：可直接寻址读取任意字节。
 *   写（编程）：只能将 1 改为 0，不能将 0 改回 1。
 *   擦除：将整个扇区（512 字节）全部恢复为 0xFF（所有位=1）。
 *
 *   因此保存时必须：先擦除扇区 → 再写入新数据。
 *
 *   IAP 操作受触发器保护（向 IAP_TRIG 写入 0x5A、0xA5），
 *   防止程序跑飞时意外修改 Flash 内容。
 *
 * EEPROM 基地址：
 *   IAP_OFFSET 定义在 iap.h（AICube 中设置的偏移地址 = 0x8000）。
 *   数据保存在 IAP_OFFSET ~ IAP_OFFSET + sizeof(SettingsEeprom_t)
 */

/*
 * 计算异或和校验值。
 * 对所有字段（checksum 自身除外）做 XOR。
 */
static uint16_t CalcChecksum(SettingsEeprom_t *s)
{
    uint8_t *p = (uint8_t *)s;
    uint8_t len = sizeof(SettingsEeprom_t) - sizeof(s->checksum);
    uint16_t sum = 0;
    uint8_t i;
    for (i = 0; i < len; i++) {
        sum ^= p[i];
    }
    return sum;
}

/*
 * 从 EEPROM 加载设置参数。
 * 在系统启动时调用（SYS_Init → UI_Init → Settings_Load）。
 *
 * 首次上电或未初始化时，Flash 内容全为 0xFF，
 * magic 字段不匹配 SETTINGS_MAGIC(0xA5)，此时返回 0，
 * 系统使用 UI_Init() 中设置的 RAM 默认值。
 *
 * IAP_ReadByte 的触发序列（由 AICube 代码实现）：
 *   设置地址 → IAP_CMD=1（读）→ 写入 0x5A → 写入 0xA5 触发 → 读取数据 → IAP_Idle
 */
uint8_t Settings_Load(void)
{
    SettingsEeprom_t st;
    uint16_t base = IAP_OFFSET;
    uint8_t *p = (uint8_t *)&st;
    uint8_t i;

    // 逐字节从 EEPROM 读取结构体
    for (i = 0; i < sizeof(st); i++) {
        p[i] = IAP_ReadByte(base + i);
    }

    // 校验 magic + version
    if (st.magic != SETTINGS_MAGIC || st.version != SETTINGS_VERSION)
        return 0;

    // 校验 checksum
    {
        uint16_t saved_cs = st.checksum;
        st.checksum = 0;
        if (saved_cs != CalcChecksum(&st))
            return 0;
        st.checksum = saved_cs;
    }

    // 应用到全局 UI 数据
    sys_ui.ui_data.op_mode              = (OpMode_e)st.op_mode;
    sys_ui.ui_data.device_address       = st.device_address;
    sys_ui.ui_data.setting_current_mA   = st.setting_current_mA;
    sys_ui.ui_data.setting_resistance_mOhm = st.setting_resistance_mOhm;
    sys_ui.ui_data.setting_power_mW     = st.setting_power_mW;
    sys_ui.ui_data.max_voltage_mV       = st.max_voltage_mV;
    sys_ui.ui_data.max_temperature_C    = st.max_temperature_C;
    return 1;
}

/*
 * 将当前设置保存到 EEPROM。
 *
 * 保存序列：
 *   1. 用 sys_ui.ui_data 的当前值填充 SettingsEeprom_t
 *   2. 计算 checksum
 *   3. 擦除 IAP_OFFSET 所在的扇区（512 字节）—— 擦除后全为 0xFF
 *   4. 逐字节写入结构体数据（编程将需要的位从 1 改为 0）
 *
 * 注意：擦除扇区会清除该扇区所有先前数据，
 * 所以必须在擦除后立即完整写入。
 */
void Settings_Save(void)
{
    SettingsEeprom_t st;
    uint16_t base = IAP_OFFSET;
    uint8_t *p = (uint8_t *)&st;
    uint8_t i;

    st.magic                = SETTINGS_MAGIC;
    st.version              = SETTINGS_VERSION;
    st.op_mode              = (uint8_t)sys_ui.ui_data.op_mode;
    st.device_address       = (uint8_t)sys_ui.ui_data.device_address;
    st.setting_current_mA   = sys_ui.ui_data.setting_current_mA;
    st.setting_resistance_mOhm = sys_ui.ui_data.setting_resistance_mOhm;
    st.setting_power_mW     = sys_ui.ui_data.setting_power_mW;
    st.max_voltage_mV       = sys_ui.ui_data.max_voltage_mV;
    st.max_temperature_C    = sys_ui.ui_data.max_temperature_C;
    st.checksum             = 0;

    st.checksum = CalcChecksum(&st);

    // 先擦除扇区（NOR Flash 写入前必须擦除）
    IAP_EraseSector(base);

    // 逐字节写入
    for (i = 0; i < sizeof(st); i++) {
        IAP_ProgramByte(base + i, p[i]);
    }
}
