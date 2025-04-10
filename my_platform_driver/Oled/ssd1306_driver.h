#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H

#define SSD1306_MAGIC 'S'


struct ShowChar {
    uint8_t Line;
    uint8_t Column;
    char Char;
};
struct ShowString {
    uint8_t Line;
    uint8_t Column;
    char String[127];
};
struct ShowNum {
    uint8_t Line;
    uint8_t Column;
    uint32_t Number;
    uint8_t Length;
};


#define OLED_OPEN _IOR(SSD1306_MAGIC, 0, uint8_t)
#define OLED_CLOSE  _IOR(SSD1306_MAGIC, 1, uint8_t) 
#define OLED_CLEAR  _IOR(SSD1306_MAGIC, 2, uint8_t) 
#define OLED_Show_Char  _IOR(SSD1306_MAGIC, 3, struct ShowChar) 
#define OLED_Show_String  _IOR(SSD1306_MAGIC, 4, struct ShowString) 
#define OLED_Show_Num  _IOR(SSD1306_MAGIC, 5, struct ShowNum) 

#endif