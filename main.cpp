#include "mbed.h"
#include "arm_math.h"
#include <math.h>
#include <stdlib.h>

#include "bbcar.h"

#define bound 0.9

Ticker servo_ticker;
Ticker encoder_ticker_left;
Ticker encoder_ticker_right;

PwmOut pin8(D8), pin9(D9);
DigitalIn pin3(D3);
DigitalIn pin4(D4);
DigitalInOut ping1(D10);
BBCar car(pin8, pin9, servo_ticker);


RawSerial xbee(D12, D11);
Serial uart(D1,D0); //tx,rx

DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
DigitalOut blueLED(LED3);


float state[3] = {0};
float Kp = 0, Ki = 0, Kd = 0;
float a0 = 0, a1 = 0, a2 = 0;
void pid_init(){
    state[0] = 0;
    state[1] = 0;
    state[2] = 0;
    a0 = Kp + Ki + Kd;
    a1 = (-Kp) - 2*Kd;
    a2 = Kd;
}

float pid_process(float in){
    int out = in*a0 + a1*state[0] + a2*state[1] + state[2];
    //update state
    state[1] = state[0];
    state[0] = in;
    state[2] = out;
    return out;
}

int status = 0;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void status_print(){
    if(status == 0){
        xbee.printf("stop");    
    }
    else if(status == 1){
        xbee.printf("straight");
    }
}

int main() {
    redLED = 1;
    greenLED = 1;
    blueLED = 1;
    // printf("SYSTEM INITIALIZED\r\n");
    xbee.printf("xbee ready");

    queue.call_every(1000, status_print);
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    
    parallax_encoder encoder0(pin3, encoder_ticker_left);
    parallax_encoder encoder1(pin4, encoder_ticker_right);
    encoder0.reset();
    encoder1.reset();

    xbee.printf("GO STRAIGHT. parameter: 100\r\n");
    status = 1;
    car.goStraight(-140,-90);
    //while(encoder1.get_cm()<100) wait_ms(50);
    xbee.printf("encoder : %f %f\r\n",encoder0.get_cm(),encoder1.get_cm());
    //encoder0.reset();
    //encoder1.reset();
    wait(6.0);

    xbee.printf("STRAIGHT STOP\r\n");
    car.stop();

    //rotate(90,'r');
    xbee.printf("ROTATE LEFT parameter: -90,-0,1.5 \r\n");
    encoder1.reset();
    car.goStraight(-90,0);
    //while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(1.5);
    xbee.printf("encoders : %f %f\r\n",encoder0.get_cm(),encoder1.get_cm());
    
    
    xbee.printf("ROTATE STOP\r\n");
    car.stop();
    encoder1.reset();

    // go for the ID stuff
    xbee.printf("GO STRAIGHT Parameter: -60 ,-120,3\r\n");
    car.goStraight(-140,-90);
    //while(encoder1.get_cm()<100) wait_ms(50);
    wait(3.0);
    xbee.printf("STOP\r\n");
    car.stop();
    encoder1.reset();
        // turn right
    xbee.printf("turn right: -100\r\n");
    car.goStraight(0,-100);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(1.5);
    xbee.printf("STOP\r\n");
    car.stop();
    encoder1.reset();

    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    wait(0.5);
    char recv;
    while(1){
        if(uart.readable()){
            recv = uart.getc();
            xbee.printf("%c\r\n",recv);
            xbee.printf("1\r\n");
            break;
        }
    }
    // reverse parking
    xbee.printf("reverse right 0 100 1.5");
    car.goStraight(0,100);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(1.5);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("back 90 120 3");
    car.goStraight(90,120);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(3.0);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("reverse left 100 0 1.5");
    car.goStraight(100,0);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(1.5);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("back 90 120 3");
    car.goStraight(90,120);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(2.0);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("halt");
    wait(3.0);

    xbee.printf("GO STRAIGHT Parameter: -120 ,-90,3\r\n");
    car.goStraight(-120,-90);
    //while(encoder1.get_cm()<100) wait_ms(50);
    wait(3.0);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("turn right: -100 , 3\r\n");
    car.goStraight(0,-100);
    // while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    wait(3.0);
    xbee.printf("STOP\r\n");
    car.stop();

// ////////// MISSION 2 ///////////////////

    car.turn(100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    encoder1.reset();
    car.goStraight(100);
    while(encoder1.get_cm()<25) wait_ms(50);
    car.stop();

    car.turn(100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    int find = 0;
    int loop = 0;
    int i;
    while ((find == 0) && (loop < 3)) {
        float object[5] ={0};
        for (i = 0; i < 5; i++) {
            encoder1.reset();
            encoder0.reset();
            car.goStraight(100);
            while(encoder1.get_cm()<45&&encoder0.get_cm()<45) wait_ms(50);
            car.stop();
            encoder1.reset();
            encoder0.reset();
            car.turn(100,0.1);
            while(encoder1.get_cm()<10.7*2*PI/4&&encoder0.get_cm()<10.7*2*PI/4) wait_ms(50);
            car.stop();
            object[i] = ping1;
            encoder1.reset();
            encoder0.reset();
            //car.back(100,0.1);
            while(encoder1.get_cm()<10.7*2*PI/4&&encoder0.get_cm()<10.7*2*PI/4) wait_ms(50);
            car.stop();

            /*            
            encoder1.reset();
            car.goStraight(100);
            while(encoder1.get_cm()<45) wait_ms(50);
            car.stop();
            wait_ms(1000);
            encoder1.reset();
            car.goStraight(-100);
            while(encoder1.get_cm()<25) wait_ms(50);
            car.stop();
            car.turn(-100,-0.1);
            encoder1.reset();
            while(encoder1.get_cm()<5*2*PI/4) wait_ms(50);
            car.stop();
            encoder1.reset();
            car.goStraight(-100);
            while(encoder1.get_cm()<15) wait_ms(50);
            car.stop();
            car.turn(-100,0.1);
            encoder0.reset();
            while(encoder0.get_cm()<5*2*PI/4) wait_ms(50);
            car.stop();*/
        }
        // output object
    }
    car.turn(-100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    encoder1.reset();
    car.goStraight(100);
    while(encoder1.get_cm()<50) wait_ms(50);
    car.stop();

    car.turn(100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    encoder1.reset();
    car.goStraight(100);
    while(encoder1.get_cm()<150) wait_ms(50);
    car.stop();


    xbee.printf("end\r\n");

}
