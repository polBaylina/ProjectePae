#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>

#include "main.h"
#include "../dyn/dyn_app_common.h"
#include "../dyn_test/dyn_emu.h"
#include "../dyn_test/b_queue.h"
#include "../joystick_emu/joystick.h"
#include "../dyn/dyn_app_motor.h"
#include "../dyn//dyn_instr.h"
#include "../dyn/dyn_app_sensor.h"

#define P_CW_ANGLE_LIMIT_L 0x06
#define P_CW_ANGLE_LIMIT_H 0x07
#define P_CCW_ANGLE_LIMIT_L 0x08
#define P_CCW_ANGLE_LIMIT_H 0x09
#define MOV_SPEED_L 0x20
#define MOV_SPEED_H 0x21

uint8_t estado = Ninguno, estado_anterior = Ninguno, finalizar = 0;
/**
 * main.c
 */
int main(void)
{
	pthread_t tid, jid;
	uint8_t tmp;

	//Init semaphores for TX data handshake
	#ifdef __OSX__
    sem_unlink("/semaphore_tx_msp");
    sem_unlink("/semaphore_tx_cli");
    sem_tx_msp = sem_open("/semaphore_tx_msp",O_CREAT,0644,0);
    sem_tx_cli = sem_open("/semaphore_tx_cli",O_CREAT,0644,0);
    #else
    sem_t sem_a, sem_b;
    sem_tx_msp = &sem_a;
    sem_tx_cli = &sem_b;
    sem_init(sem_tx_msp, 0, 0);
    sem_init(sem_tx_cli, 0, 0);
    #endif

	//Init queue for RX data
	init_queue();


	//Start thread for dynamixel module emulation
	pthread_create(&tid, NULL, dyn_emu, (void*) &tid);
	pthread_create(&jid, NULL, joystick_emu, (void*) &jid);

	//Testing some high level function
	printf("MAIN: Setting LED to 0 \n");
    dyn_led_control(1, 0);
    printf("MAIN: Getting LED value \n");
    dyn_led_read(1, &tmp);
    assert(tmp == 0);
    printf("MAIN: Setting LED to 1 \n");
	dyn_led_control(1, 1);
	printf("MAIN: Getting LED value \n");
    dyn_led_read(1, &tmp);
    assert(tmp == 1);

    printf("Main: setting the left motor\n");
    endlessTurn(1);
    endlessTurn(2);

    dyn_read_byte(1, P_CW_ANGLE_LIMIT_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, P_CW_ANGLE_LIMIT_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(1, P_CW_ANGLE_LIMIT_H, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, P_CW_ANGLE_LIMIT_H, &tmp);
    assert(tmp==0);
    dyn_read_byte(1, P_CCW_ANGLE_LIMIT_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, P_CCW_ANGLE_LIMIT_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(1, P_CCW_ANGLE_LIMIT_H, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, P_CCW_ANGLE_LIMIT_H, &tmp);
    assert(tmp==0);
    
    // Test forward
    printf("\nTest move_forward P_CW_ANGLE_LIMIT_L");
    forward(200);
    dyn_read_byte(1,MOV_SPEED_L, &tmp);
    assert(tmp == 0);
    dyn_read_byte(2,MOV_SPEED_H, &tmp);
    assert(tmp == 0);

    // Test backward
    printf("\nTest move_forward P_CW_ANGLE_LIMIT_L");
    backward(200);
    dyn_read_byte(1, MOV_SPEED_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, MOV_SPEED_H, &tmp);
    assert(tmp==0);

    // Test stop
    stop();
    dyn_read_byte(1, MOV_SPEED_L, &tmp);
    assert(tmp==0);
    dyn_read_byte(2, MOV_SPEED_H, &tmp);
    assert(tmp==0);

    //Test sensor
    redObsDistance(0x03, 100);
    sensorRead(0x03, 100);
    getObstacleFlag(0x03);

    printf("************************\n");
	printf("Test passed successfully\n");
	printf("Pulsar 'q' para terminar, qualquier tecla para seguir\r");
	fflush(stdout);//	return 0;

	while(estado != Quit)
	{
		Get_estado(&estado, &estado_anterior);
		if(estado != estado_anterior){
			Set_estado_anterior(estado);
			printf("estado = %d\n", estado);
			fflush(stdout);
			switch(estado){
			case Sw1:
				printf("Boton Sw1 ('a') apretado\n");
				break;
			case Sw2:
				printf("Boton Sw2 ('s') apretado\n");
				break;
			case Quit:
				printf("Adios!\n");
				break;
			//etc, etc...
			}
			fflush(stdout);
		}
	}
	printf("Programa terminado\n");
	//Signal the emulation thread to stop
	pthread_kill(tid, SIGTERM);
	pthread_kill(jid, SIGTERM);
}