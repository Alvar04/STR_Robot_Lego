#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define SONAR_PORT NXT_PORT_S1
#define PULSADOR1_PORT NXT_PORT_S2
/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(Contador);
DeclareEvent(Colision);
DeclareTask(Avance);
DeclareTask(Retroceso);
DeclareTask(ReducirVelocidad);
DeclareTask(ComprobarColision);
DeclareTask(Final);



void ecrobot_device_initialize()
{
	// Inicializar los sensores activos
	ecrobot_init_sonar_sensor(NXT_PORT_S1);


}
void ecrobot_device_terminate()
{
	// Finalizar los sensores activos
	ecrobot_term_sonar_sensor(NXT_PORT_S1);

}

/*--------------------------------------------------------------------------*/
/* Definitions                                                              */
/*--------------------------------------------------------------------------*/

int velocidadInicialB = 97, velocidadB; //M30
int velocidadInicialC = 100, velocidadC; //M2

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(){
	SignalCounter(Contador);
}

/*-------------------------------------------------------------------------*/
/* Task information:                                                       */
/* -----------------                                                       */
/* Name    : Avance                                                        */
/* Priority: ??                                                            */
/* Typ     : EXTENDED TASK                                                 */
/* Schedule: ??		                                     	               */
/* Objective: Move forward.						 						   */
/*-------------------------------------------------------------------------*/
TASK(Avance)
{
  velocidadB = velocidadInicialB;
  velocidadC = velocidadInicialC;

	int time_out = systick_get_ms() + 30000;
	while(systick_get_ms()<time_out){
		nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
		nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
		WaitEvent(Colision);
		ClearEvent(Colision);
		ActivateTask(Retroceso);
	}
	ChainTask(Final);
	TerminateTask();
}


/*-------------------------------------------------------------------------*/
/* Task information:                                                       */
/* -----------------                                                       */
/* Name    : Retroceso                                                     */
/* Priority: ??                                                            */
/* Typ     : EXTENDED TASK                                                 */
/* Schedule: ??		                                         	           */
/* Objective: Move backward and change direction	   					   */
/*-------------------------------------------------------------------------*/
TASK(Retroceso)
{
   int time_out = systick_get_ms() + 1000 ;
   while(systick_get_ms() < time_out){
		nxt_motor_set_speed(NXT_PORT_B, -100, 1);
		nxt_motor_set_speed(NXT_PORT_C, -100, 1);
	}

	//Paramos B
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	nxt_motor_set_count(NXT_PORT_C, 0);
	//Giramos el motor izquierdo para girar a la derecha
	while(nxt_motor_get_count(NXT_PORT_C)<360)
	{
		nxt_motor_set_speed(NXT_PORT_C, 100, 1);
	};
	nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
	nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
	TerminateTask();
}

TASK(ComprobarColision)
{
	if (ecrobot_get_touch_sensor(NXT_PORT_S2)==1)
	{
		SetEvent(Avance,Colision);
	}
	TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : CambioDir		                                            */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Change direction of the robot.				    */
/*--------------------------------------------------------------------------*/


TASK(ReducirVelocidad)
{
	if(ecrobot_get_sonar_sensor(SONAR_PORT)<35){
		nxt_motor_set_speed(NXT_PORT_B, velocidadB/4, 1);
		nxt_motor_set_speed(NXT_PORT_C, velocidadC/4, 1);
	}else if(ecrobot_get_sonar_sensor(SONAR_PORT)<115){
		nxt_motor_set_speed(NXT_PORT_B, velocidadB/2, 1);
		nxt_motor_set_speed(NXT_PORT_C, velocidadC/2, 1);
	}else{
		nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
		nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
	}
	TerminateTask();
}

TASK(Final)
{
    // Para los motores
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
}



