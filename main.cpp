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
DigitalInOut pin10(D10);
BBCar car(pin8, pin9, servo_ticker);


RawSerial xbee(D12, D11);
Serial uart(D1,D0); //tx,rx

DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
DigitalOut blueLED(LED3);



int manipulate(int speed1, int speed2, float wait_time){
    car.goStraight(speed1,speed2);
    wait(wait_time);
    car.stop();
    wait(1);
}

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

int status;
void status_print(){
    xbee.printf("hello I've been sending cause i can\r\n");
}

int str_par1 = -80;
int str_par2 = -70;

int main() {
    redLED = 1;
    greenLED = 1;
    blueLED = 1;
    float buffer = 0;


    // printf("SYSTEM INITIALIZED\r\n");
    parallax_ping  ping1(pin10);

    xbee.printf("xbee ready");

    queue.call_every(1000, status_print);
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    

    xbee.printf("GO STRAIGHT. parameter: -130,80,7\r\n");
    //status = 1;
    manipulate(str_par1,str_par2,8.5);
    xbee.printf("STRAIGHT STOP\r\n");
    
    /*
    while(1){
        if(buffer > 20){
            redLED = 1;
        }
        else{
            redLED = 0;
        }
        buffer = float(ping1);
        wait(0.1);
    }
    */
    /*
    car.goStraight(str_par1,str_par2);
    buffer = float(ping1);
    while(buffer > 20){
        wait(0.2);
        buffer = ping1;
        printf("%f",buffer);
    }
    car.stop();
    */

    xbee.printf("ROTATE LEFT parameter: -90,-0,1.45\r\n");
    manipulate(str_par1,0,1.5);
    xbee.printf("ROTATE STOP\r\n");

    // go for the ID stuff
    xbee.printf("GO STRAIGHT Parameter: -130 ,-90,8\r\n");
    manipulate(str_par1,str_par2,7);
    xbee.printf("STOP\r\n");
    
    
    xbee.printf("turn right: 0,-100,1.5\r\n");
    manipulate(0,str_par2,1.5);
    xbee.printf("STOP\r\n");
    

    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    wait(0.5);
    char recv;
    int counter = 0;
    while(1){
        if(uart.readable()){
            recv = uart.getc();
            xbee.printf("%c\r\n",recv);
            greenLED = 0;
            break;
        }
        counter++;
        if(counter >= 100000){
            xbee.printf("I didn't make it \r\n");
            break;
        }
    }
    
    // reverse parking
    xbee.printf("reverse right 0 140 1.5");
    manipulate(0,-1*str_par2,1.4);
    xbee.printf("STOP\r\n");

    xbee.printf("back 90 120 2");
    manipulate(-1*str_par2,-1*str_par1,3);
    xbee.printf("STOP\r\n");

    xbee.printf("reverse left 100 0 1.6");
    manipulate(-1*str_par1,0,1.4);
    xbee.printf("STOP\r\n");

    xbee.printf("back 90 120 1.5");
    manipulate(-1*str_par1,-1*str_par2,1.5);
    xbee.printf("STOP\r\n");

    xbee.printf("halt");
    wait(3.0);

    xbee.printf("GO STRAIGHT Parameter: -120 ,-90,1.5\r\n");    
    manipulate(-120,-90,1.5);
    xbee.printf("STOP\r\n");

    xbee.printf("U turn right: -10,-100 , 3.3\r\n");
    manipulate(-20,-100,3.5);
    xbee.printf("STOP\r\n");

    xbee.printf("GO STRAIGHT Parameter: -120 ,-90,6\r\n");
    manipulate(str_par1,str_par2,8);
    xbee.printf("STOP\r\n");

// ////////// MISSION 2 ///////////////////

    xbee.printf("turn right: -100 , 1.5\r\n");
    car.goStraight(0,-100);
    wait(1.5);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("GO STRAIGHT Parameter: -120 ,-90,4\r\n");
    manipulate(str_par1,str_par2,1);
    xbee.printf("STOP\r\n");
    
    xbee.printf("turn right: -100 , 1.4\r\n");
    car.goStraight(0,-100);
    wait(1.4);
    xbee.printf("STOP\r\n");
    car.stop();

    xbee.printf("GO STRAIGHT Parameter: -120 ,-90,4\r\n");
    manipulate(str_par1,str_par2,1);
    xbee.printf("STOP\r\n");

    wait(5.0);
    
    int find = 0;
    int loop = 0;
    int i;

    float object[5] ={0};
    
    //xbee.printf("GO STRAIGHT Parameter: -120 ,-90,4\r\n");
    manipulate(str_par1,-1*str_par2,0.5);
    xbee.printf("STOP\r\n");    

    for (i = 0; i < 5; i++) {
        object[i] = ping1;
        xbee.printf("GO STRAIGHT Parameter: -120 ,-90,4\r\n");
        manipulate(-1*str_par1,str_par2,0.25);
        xbee.printf("STOP\r\n");
        wait(1);
    }
    manipulate(-1*str_par1,str_par2,0.5);
    xbee.printf("STOP\r\n");    


    
   xbee.printf("BACK Parameter: -120 ,-90,\r\n");
   manipulate(-1*str_par2,-1*str_par2,2);
   xbee.printf("STOP\r\n");

   xbee.printf("reverse right 0 140 1.5");
   manipulate(0,-1*str_par2,1.4);
   xbee.printf("STOP\r\n");

   xbee.printf("GO STRAIGHT Parameter: -120 ,-90,4\r\n");
   manipulate(str_par1,str_par2,6);
   xbee.printf("STOP\r\n");

   xbee.printf("RIGHT Parameter: -120 ,-90,4\r\n");
   manipulate(0,str_par2,1.4);
   xbee.printf("STOP\r\n");

   xbee.printf("GO STRAIGHT Parameter: -120 ,-90,8\r\n");
   manipulate(str_par1,str_par2,8);
   xbee.printf("STOP\r\n");

   xbee.printf("end\r\n");

}
