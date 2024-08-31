#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "lcd_lib.h"

class ILI9341Component : public Component {
public:
    void setup() override;
    void loop() override;

    void set_width(int width);
    void set_height(int height);
    void set_interface(int interface);

private:
    int width_;
    int height_;
    INTERFACE _interface; // Cambiar a tipo INTERFACE
    GPIOPin *_d0, *_d1, *_d2, *_d3, *_d4, *_d5, *_d6, *_d7; // Pines de datos
};

#endif
