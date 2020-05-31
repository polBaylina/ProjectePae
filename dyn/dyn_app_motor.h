#include <ntsid.h>
//
// Created by Pol Baylina on 25/04/2020.
//

#ifndef JOYSTICK_DYN_MOTORS_H
#define JOYSTICK_DYN_MOTORS_H
#include <stdint.h>
#include <stdbool.h>
typedef uint8_t byte;
static byte BROADCASTING = 0xFE;
static byte WRITE_DATA = 0x03;
static CW_ANGLE_LIMIT_L = 0x06;

void endlessTurn(byte idMotor);//to rotate wheels

void moveWheel(byte ID, bool rotation,unsigned int speed);

void stop(void);

void turnLeft(unsigned int speed);

void turnOnItselfLeft(unsigned int speed);

void turnRight(unsigned int speed);

void turnOnItselfRight(unsigned int speed);

void forward(unsigned int speed);

void backward(unsigned int speed);



#endif //JOYSTICK_DYN_MOTORS_H
