Practica1b.c

/*--------------------------------------------------------------------------
 	Practica: 1.b


--------------------------------------------------------------------------*/
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#define velocidadInicialB 50 //M30
#define velocidadInicialC 50 //M2

/*--------------------------------------------------------------------------*/
    /* OSEK declarations                                                        */
    /*--------------------------------------------------------------------------*/
    DeclareTask(Avance);
    DeclareTask(Final);
    DeclareTask(Calibrar);
    DeclareTask(Giro90Izq);
    DeclareTask(Giro90Der);
    DeclareTask(AvanzaAparca);
    DeclareCounter(contador);
    DeclareAlarm(alarma);

 /*--------------------------------------------------------------------------*/
    /* Definitions                                                              */
    /*--------------------------------------------------------------------------*/
    //int velocidadB = 100;
    //int velocidadC = 100;

    int velocidadB = velocidadInicialB;
    int velocidadC = velocidadInicialC;
    /*--------------------------------------------------------------------------*/
    /* Function to be invoked from a category 2 interrupt                       */
    /*--------------------------------------------------------------------------*/
    void user_1ms_isr_type2(){
        SignalCounter(contador);
    }
    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : Avance											*/
    /* Priority: 1												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: Reliza un avance de entre 30-35 cm.			*/
    /*----------------------------------------------------------*/
    TASK(Avance)
    {
        systick_wait_ms(3000);
        // Activa motores para comprobar si existe desvio en la navegacion
        nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
        nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);

        // Espera hasta que se agote el time_out
        systick_wait_ms(1700);

        ChainTask(Giro90Izq);

        // Termina la tarea actual
        TerminateTask();
    }
	
    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : Calibrar										*/
    /* Priority: 2												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: Corrige los motores para evitar el desvio		*/
    /*----------------------------------------------------------*/
    TASK(Calibrar)
    {
        int rpmB = nxt_motor_get_count(NXT_PORT_B);
        int rpmC = nxt_motor_get_count(NXT_PORT_C);

        int margen = 5;

        if(rpmB < rpmC) {
            if(velocidadB < velocidadInicialB + margen)
                velocidadB++;
            else
                velocidadC--;
            nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
            nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
        }

        if(rpmC < rpmB) {
            if(velocidadC < velocidadInicialC + margen)
                velocidadC++;
            else
                velocidadB--;
            nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
            nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
        }

        nxt_motor_set_count(NXT_PORT_B, 0);
        nxt_motor_set_count(NXT_PORT_C, 0);


        TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : Giro90Izq										*/
    /* Priority: 3												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: El robot gira 90º a la izquierda				*/
    /*----------------------------------------------------------*/
    TASK(Giro90Izq)
    {
      nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    	nxt_motor_set_count(NXT_PORT_B, 0);
    	//Giramos el motor izquierdo para girar a la derecha
    	while(nxt_motor_get_count(NXT_PORT_B)<360)
    	{
    		nxt_motor_set_speed(NXT_PORT_B, 60, 1);
    	};
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);

      nxt_motor_set_count(NXT_PORT_B, 0);
      nxt_motor_set_count(NXT_PORT_C, 0);

      ChainTask(AvanzaAparca);
      TerminateTask();
    }
	
    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : AvanzaAparca									*/
    /* Priority: 1												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: Avanza 17 cm. despuesde girar a la izquierda	*/
    /*----------------------------------------------------------*/
    TASK(AvanzaAparca)
    {
      // Activa motores para comprobar si existe desvio en la navegacion
      nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
      nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);

      // Espera hasta que se agote el time_out
      systick_wait_ms(300);

      //Activar el giro a la izquierda
      ChainTask(Giro90Der);

      // Termina la tarea actual
      TerminateTask();

    }
	
    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : Giro90Der										*/
    /* Priority: 3												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: El robot gira 90º a la derecha				*/
    /*----------------------------------------------------------*/
    TASK(Giro90Der)
    {
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    	nxt_motor_set_count(NXT_PORT_C, 0);
    	//Giramos el motor izquierdo para girar a la derecha
    	while(nxt_motor_get_count(NXT_PORT_C)<360)
    	{
    		nxt_motor_set_speed(NXT_PORT_C, 60, 1);
    	};
      nxt_motor_set_speed(NXT_PORT_C, 0, 1);

      nxt_motor_set_count(NXT_PORT_B, 0);
      nxt_motor_set_count(NXT_PORT_C, 0);

      ChainTask(Final);
      TerminateTask();
    }
	
    /*----------------------------------------------------------*/
    /* Task information:										*/
    /* -----------------										*/
    /* Name    : Final											*/
    /* Priority: 10												*/
    /* Typ     : EXTENDED TASK									*/
    /* Schedule: FULL											*/
    /* Objective: Para motores y cancela todas las alarmas		*/
    /*----------------------------------------------------------*/
    TASK(Final)
    {
        // Para los motores
        nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        nxt_motor_set_speed(NXT_PORT_C, 0, 1);

        CancelAlarm(alarma);

        // Termina la tarea actual
        TerminateTask();

    }
