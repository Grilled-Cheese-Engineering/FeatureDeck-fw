#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "bsp/board.h"
#include "tusb.h"

#include "rotary.h"

int Aarr[] = { 19, 18, 27, 28, 0 };
int Barr[] = { 10, 20, 26, 29, 1 };

int SWarr[] = { 6, 5, 4, 3, 2, 7, 8, 9 };
int SWstates[] = { -1, -1, -1, -1, -1, -1, -1, -1 };

int rotStates[] = { 0, 0, 0, 0, 0 };

std::vector<Encoder> encoders;

uint8_t msg[3];

bool SWlock = false;
void initPin(int x);
void gpio_callback(uint gpio, uint32_t events);

int64_t unlcokSW(alarm_id_t id, __unused void* user_data) {
    SWlock = false;
    return 0;
}


int main() {
    stdio_init_all();
    board_init();
    tusb_init();
    for (int i = 0; i < 5; i++) {
        encoders.push_back(Encoder(
            Aarr[i],
            Barr[i],
            [x = i + 1]() {
                if (rotStates[x - 1] == 0) {
                    rotStates[x - 1] = 127;
                } else {
                    rotStates[x - 1]--;
                }
                uint8_t msg[3];
                msg[0] = 0x90;
                msg[1] = (x * 2) - 2;
                msg[2] = 127;
                tud_midi_n_stream_write(0, 0, msg, 3);

                msg[0] = 0xB0;
                msg[1] = 101 + x;
                msg[2] = rotStates[x - 1];
                tud_midi_n_stream_write(0, 0, msg, 3);

                msg[0] = 0x80;
                msg[1] = (x * 2) - 2;
                msg[2] = 0;
                tud_midi_n_stream_write(0, 0, msg, 3);

            }, [x = i + 1]() {
                if (rotStates[x - 1] == 127) {
                    rotStates[x - 1] = 0;
                } else {
                    rotStates[x - 1]++;
                }
                uint8_t msg[3];
                msg[0] = 0x90;
                msg[1] = (x * 2) - 1;
                msg[2] = 127;
                tud_midi_n_stream_write(0, 0, msg, 3);

                msg[0] = 0xB0;
                msg[1] = 101 + x;
                msg[2] = rotStates[x - 1];
                tud_midi_n_stream_write(0, 0, msg, 3);

                msg[0] = 0x80;
                msg[1] = (x * 2) - 1;
                msg[2] = 0;
                tud_midi_n_stream_write(0, 0, msg, 3);
                },
                &gpio_callback
                ));
    }

    for (int i = 0; i < 8; i++) {
        initPin(SWarr[i]);
        //SWstates[i] = gpio_get(SWarr[i]);
        gpio_set_irq_enabled_with_callback(SWarr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }

    while (true) {
        tud_task();
        sleep_ms(2);
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

    if (sw >= 0 && !SWlock) {
        if (events & GPIO_IRQ_EDGE_FALL && (SWstates[sw] == 1 || SWstates[sw] == -1)) {
            SWlock = true;
            msg[0] = 0x90;
            msg[1] = (uint8_t)(10 + sw);
            msg[2] = 127;
            tud_midi_n_stream_write(0, 0, msg, 3);
            SWstates[sw] = 0;
        } else if (events & GPIO_IRQ_EDGE_RISE && SWstates[sw] == 0) {
            SWlock = true;
            msg[0] = 0x80;
            msg[1] = (uint8_t)(10 + sw);
            msg[2] = 0;
            tud_midi_n_stream_write(0, 0, msg, 3);
            SWstates[sw] = 1;
        }
        add_alarm_in_ms(10, unlcokSW, NULL, false);
    }



}



void initPin(int x) {
    gpio_init(x);
    gpio_set_dir(x, GPIO_IN);
    gpio_pull_up(x);
}


