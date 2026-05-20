#ifndef __OLEDFONT_H
#define __OLEDFONT_H

// 中文字体枚举类型
typedef enum {
    zhi = 0,  // 'ֱ直'
    liu = 1,  // '流'
    dian = 2, // '电'
    zi = 3,   // '子'
    fu = 4,   // '负'
    zai = 5,  // '载'
    _CC = 6,   // 'CC'
    _CR = 7,   // 'CR'
    _CP = 8,   // 'CP'
    _space = 9,// ' ' (空格)
    zhe = 10, // '浙'
    shi = 11, // '师'
    fu2 = 12, // '傅'
    bei = 13, // '倍'
    liang = 14,// '梁'
    ying = 15, // '英'
    max_chinese
} Chinese_font;


//ASCII
extern const unsigned char code F6x8[][6];
extern const unsigned char code F8X16[];

// 中文
extern const unsigned char code Hzk[][32];

// 图标资源
extern unsigned char code Icon_32x32_School_Emble[];


#endif
