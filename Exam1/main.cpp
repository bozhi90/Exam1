#include "mbed.h"
#include "uLCD_4DGL.h"
#include "mbed_events.h"
using namespace std::chrono;

uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn up(D10);
InterruptIn down(D9);
InterruptIn sel(D11);
DigitalOut led1(LED1);
AnalogOut aout(PA_4);
EventQueue queue;
Timer debounce;
Thread t;

char rate = 1;

void Generate() {
    while (1) {
        for (float j = 0.0f; j < 1.0f; j += 0.1f) {
            ThisThread::sleep_for(8ms / rate);
	        aout = j;
        }
        ThisThread::sleep_for(80ms - 80ms / rate);
        ThisThread::sleep_for(80ms);
        ThisThread::sleep_for(80ms - 80ms / rate);
        for (float j = 0.9f; j >= 0.0f; j -= 0.1f) {
            ThisThread::sleep_for(8ms / rate);
	        aout = j;
        }
    }
}

void down_bt() {
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        if (rate < 8) rate *= 2;
        debounce.reset();
    }
}

void up_bt() {
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        if (rate > 1) rate /= 2;
        debounce.reset(); 
    }
}

void select_bt() {
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        queue.call(Generate);
        debounce.reset(); 
    }
}

void uLCD_initial(void) {
    uLCD.reset();
    uLCD.background_color(BLACK);
    uLCD.textbackground_color(BLACK);
    uLCD.cls();
    uLCD.color(WHITE);
}

int main()
{
    char rate_last = 0;
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    uLCD_initial();
    debounce.start();
    up.rise(&up_bt);
    down.rise(&down_bt);
    sel.rise(&select_bt);
    while (1) {
        if (rate != rate_last) {
            uLCD.locate(1,2);
            if (rate == 1) uLCD.printf("rate = 1  ");
            else if (rate == 2) uLCD.printf("rate = 1/2");
            else if (rate == 4) uLCD.printf("rate = 1/4");
            else uLCD.printf("rate = 1/8");
        }
        rate_last = rate;
        ThisThread::sleep_for(100ms);
    }
}