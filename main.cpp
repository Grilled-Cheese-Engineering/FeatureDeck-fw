#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

int Aarr[] = { 19, 18, 27, 28, 0 };
int Barr[] = { 10, 20, 26, 29, 1 };
int states[] = { 0, 0, 0, 0, 0 };

int SWarr[] = { 6, 5, 4, 3, 2, 7, 8, 9 };

bool swlock = false;

void initPin(int x);
void gpio_callback(uint gpio, uint32_t events);


bool rotlock = false;

int64_t unlcokSW(alarm_id_t id, __unused void* user_data) {
    swlock = false;
    return 0;
}

int main() {
    stdio_init_all();


    gpio_set_irq_enabled_with_callback(Aarr[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    int state = 0;
    int a;
    int b;
    for (int i = 0; i < 5; i++) {
        initPin(Aarr[i]);
        initPin(Barr[i]);
        a = gpio_get(Aarr[i]);
        b = gpio_get(Barr[i]);
        if (!a && !b) {
            state = 3;
        } else if (a && !b) {
            state = 2;
        } else if (!a && b) {
            state = 1;
        }
        states[i] = state;
        gpio_set_irq_enabled(Aarr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(Barr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    for (int i = 0; i < 8; i++) {
        initPin(SWarr[i]);
        gpio_set_irq_enabled(SWarr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    while (true) {
        //printf("hello\n");
        sleep_ms(10);
    }
}

void gpio_callback(uint gpio, uint32_t events) {
    int encoder = -1;
    for (int i = 0; i < 5; i++) {
        if (gpio == Aarr[i] || gpio == Barr[i]) {
            encoder = i;
        }
    }
    int sw = -1;
    for (int i = 0; i < 8; i++) {
        if (gpio == SWarr[i]) {
            sw = i;
        }
    }
    if (!swlock) {
        swlock = true;
        if (sw >= 0 && events & GPIO_IRQ_EDGE_FALL) {
            printf("SW%d down\n", sw);
        } else if (sw >= 0 && events & GPIO_IRQ_EDGE_RISE) {
            printf("SW%d up\n", sw);
        }
        add_alarm_in_ms(2, unlcokSW, NULL, false);

    }

    if (encoder == -1) {
        return;
    }
    int state = 0;
    int a = gpio_get(Aarr[encoder]);
    int b = gpio_get(Barr[encoder]);
    if (!a && !b) {
        state = 3;
    } else if (a && !b) {
        state = 2;
    } else if (!a && b) {
        state = 1;
    }
    if (state == states[encoder]) return;
    if (states[encoder] == 0) {
        if (state == 2) {
            printf("%d right\n", encoder + 1);
        } else if (state == 1) {
            printf("%d left\n", encoder + 1);
        }
    } else if (states[encoder] == 1) {
        if (state == 0) {
            printf("%d right\n", encoder + 1);
        } else if (state == 3) {
            printf("%d left\n", encoder + 1);
        }
    } else if (states[encoder] == 2) {
        if (state == 3) {
            printf("%d right\n", encoder + 1);
        } else if (state == 0) {
            printf("%d left\n", encoder + 1);
        }
    } else if (states[encoder] == 3) {
        if (state == 1) {
            printf("%d right\n", encoder + 1);
        } else if (state == 2) {
            printf("%d left\n", encoder + 1);
        }
    }
    states[encoder] = state;

}



void initPin(int x) {
    gpio_init(x);
    gpio_set_dir(x, GPIO_IN);
    gpio_pull_up(x);
}