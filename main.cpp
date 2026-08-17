#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"


#include "rotary.h"

int Aarr[] = { 19, 18, 27, 28, 0 };
int Barr[] = { 10, 20, 26, 29, 1 };

int SWarr[] = { 6, 5, 4, 3, 2, 7, 8, 9 };

std::vector<Encoder> encoders;

bool SWDlock = false;
bool SWUlock = false;
void initPin(int x);
void gpio_callback(uint gpio, uint32_t events);


bool rotlock = false;


int64_t unlcokSWD(alarm_id_t id, __unused void* user_data) {
    SWDlock = false;
    return 0;
}
int64_t unlcokSWU(alarm_id_t id, __unused void* user_data) {
    SWUlock = false;
    return 0;
}


int main() {
    stdio_init_all();
    for (int i = 0; i < 5; i++) {
        encoders.push_back(Encoder(
            Aarr[i],
            Barr[i],
            [&i]() {printf("%d left\n", i);},
            [&i]() {printf("%d right\n", i);},
            &gpio_callback
        ));
    }

    for (int i = 0; i < 8; i++) {
        initPin(SWarr[i]);
        gpio_set_irq_enabled_with_callback(SWarr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }

    while (true) {
        sleep_ms(10);
    }
}

void gpio_callback(uint gpio, uint32_t events) {
    for (int i = 0; i < 5; i++) {
        encoders.at(i).check(gpio, events);
    }
    int sw = -1;
    for (int i = 0; i < 8; i++) {
        if (gpio == SWarr[i]) {
            sw = i;
        }
    }

    if (sw >= 0 && events & GPIO_IRQ_EDGE_FALL && !SWDlock) {
        SWDlock = true;
        if (sw < 5) {
            printf("rot%d down\n", sw + 1);
        } else {
            printf("SW%d down\n", sw - 4);
        }
        add_alarm_in_ms(100, unlcokSWD, NULL, false);
    } else if (sw >= 0 && events & GPIO_IRQ_EDGE_RISE && !SWUlock) {
        SWUlock = true;
        if (sw < 5) {
            printf("rot%d up\n", sw + 1);
        } else {
            printf("SW%d up\n", sw - 4);
        }
        add_alarm_in_ms(100, unlcokSWU, NULL, false);
    }


}



void initPin(int x) {
    gpio_init(x);
    gpio_set_dir(x, GPIO_IN);
    gpio_pull_up(x);
}