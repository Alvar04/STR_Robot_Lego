/*--------------------------------------------------------------------------
    Practica: 2
--------------------------------------------------------------------------*/
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#define PULSADOR_PORT NXT_PORT_S1
#define SONAR_PORT NXT_PORT_S4
#define velocidadInicialB 30 //M30
#define velocidadInicialC 30 //M2


/*--------------------------------------------------------------------------*/
    /* OSEK declarations                                                        */
    /*--------------------------------------------------------------------------*/
    DeclareTask(Principal);
    DeclareTask(Avance);
    DeclareTask(Final);
    DeclareTask(EvitarColision);
    DeclareTask(Calibrar);
    DeclareTask(Giro90Izq);
    DeclareTask(Giro90Der);
    DeclareTask(DistFrontal);
    DeclareTask(paredCercana);
    DeclareTask(Pulsador);
    DeclareCounter(contador);
    DeclareAlarm(AlCalibrar);
    DeclareAlarm(AlDistancia);
    DeclareAlarm(AlPulsador);

    DeclareEvent(giroDCom);
    DeclareEvent(giroICom);
    DeclareEvent(medir);
    DeclareEvent(colision);




    void ecrobot_device_initialize()
    {
        // Inicializar los sensores activos
        ecrobot_init_sonar_sensor(SONAR_PORT);
    }
    void ecrobot_device_terminate()
    {
        // Finalizar los sensores activos
        ecrobot_term_sonar_sensor(SONAR_PORT);
    }
 /*--------------------------------------------------------------------------*/
    /* Definitions                                                              */
    /*--------------------------------------------------------------------------*/
    int velocidadB = velocidadInicialB; //M30
    int velocidadC = velocidadInicialC; //M2

    int paredFrontal=0, paredDerecha=0;

    int pri=300, seg=300, ter=300, cua=300;
    /*--------------------------------------------------------------------------*/
    /* Function to be invoked from a category 2 interrupt                       */
    /*--------------------------------------------------------------------------*/
    void user_1ms_isr_type2(){
        SignalCounter(contador);
    }
    
    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Principal                                      */
    /* Priority: 1                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Realiza las tareas principales. tarea pensante*/
    /*----------------------------------------------------------*/
    TASK(Principal)
    {
      //CancelAlarm(AlCalibrar);

      ActivateTask(Avance);

      int time_out = systick_get_ms() + 2000 ; //Calculamos 2 segundos
      while(systick_get_ms() < time_out);
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);
      nxt_motor_set_speed(NXT_PORT_C, 0, 1);



      ClearEvent(giroDCom);
      ActivateTask(Giro90Der);
      WaitEvent(giroDCom);


      ClearEvent(medir);
      ActivateTask(paredCercana);
      WaitEvent(medir);

      // display_goto_xy(0,0);
      // display_unsigned(pri, 3);
      // display_unsigned(seg, 3);
      // display_goto_xy(0,5);
      // display_unsigned(ter, 3);
      // display_unsigned(cua, 3);
      // display_update();


      if(cua<30){
        paredDerecha = 1;
        ClearEvent(giroICom);
        ActivateTask(Giro90Izq);
        WaitEvent(giroICom);

        ClearEvent(medir);
        ActivateTask(paredCercana);
        WaitEvent(medir);
        if(cua<30)
        {
          ClearEvent(giroICom);
          ActivateTask(Giro90Izq);
          WaitEvent(giroICom);
        }
      }

       


      ChainTask(Avance);

        // Termina la tarea actual
      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Avance                                         */
    /* Priority: 2                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Activa los motores para avanzar               */
    /*----------------------------------------------------------*/
    TASK(Avance){

      ActivateTask(EvitarColision);
      nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
      nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);
      ActivateTask(Principal);
      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Giro90Der                                      */
    /* Priority: 6                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: El robot gira 90º a la derecha                */
    /*----------------------------------------------------------*/
    TASK(Giro90Der)
    {

      nxt_motor_set_speed(NXT_PORT_C, 0, 1);
      nxt_motor_set_count(NXT_PORT_B, 0);
      //Giramos el motor izquierdo para girar a la derecha
      while(nxt_motor_get_count(NXT_PORT_B)>-180)//346)
      {
        nxt_motor_set_speed(NXT_PORT_B, -30, 1);
        nxt_motor_set_speed(NXT_PORT_C, 30, 1);
      };
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);

      nxt_motor_set_count(NXT_PORT_C, 0);
      nxt_motor_set_count(NXT_PORT_B, 0);

      SetEvent(Principal, giroDCom);
      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Giro90Izq                                      */
    /* Priority: 6                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: El robot gira 90º a la izquierda              */
    /*----------------------------------------------------------*/
    TASK(Giro90Izq)
    {
      nxt_motor_set_speed(NXT_PORT_C, 0, 1);
      nxt_motor_set_count(NXT_PORT_B, 0);
      //Giramos el motor izquierdo para girar a la derecha
      while(nxt_motor_get_count(NXT_PORT_B)<190)
      {
        nxt_motor_set_speed(NXT_PORT_B, 30, 1);
        nxt_motor_set_speed(NXT_PORT_C, -30, 1);
      };
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);

      nxt_motor_set_count(NXT_PORT_C, 0);
      nxt_motor_set_count(NXT_PORT_B, 0);

      SetEvent(Principal, giroICom);
      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : DistFrontal                                    */
    /* Priority: 4                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Comprueba la distancia frontal a un objeto    */
    /*----------------------------------------------------------*/
    TASK(DistFrontal)
    {
      if(ecrobot_get_sonar_sensor(SONAR_PORT)<27){
        // paredFrontal = 1;

        SetEvent(EvitarColision, colision);

        nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        nxt_motor_set_speed(NXT_PORT_C, 0, 1);

        nxt_motor_set_count(NXT_PORT_B, 0);
        nxt_motor_set_count(NXT_PORT_C, 0);
        }
      else{

        // paredFrontal = 0;

        // nxt_motor_set_speed(NXT_PORT_B, velocidadB, 1);
        // nxt_motor_set_speed(NXT_PORT_C, velocidadC, 1);

      }

        TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : EvitarColision                                 */
    /* Priority: 4                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Para motores para evitar choque frontal       */
    /*----------------------------------------------------------*/
    TASK(EvitarColision)
    {
      ClearEvent(colision);
      WaitEvent(colision);


      nxt_motor_set_speed(NXT_PORT_B, 0, 1);
      nxt_motor_set_speed(NXT_PORT_C, 0, 1);


      nxt_motor_set_count(NXT_PORT_C, 0);
      nxt_motor_set_count(NXT_PORT_B, 0);


      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : paredCercana                                   */
    /* Priority: 5                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Realiza lectura del sensor de distancia       */
    /*----------------------------------------------------------*/
    TASK(paredCercana)
    {
      pri = ecrobot_get_sonar_sensor(SONAR_PORT);
      systick_wait_ms(50);
      seg = ecrobot_get_sonar_sensor(SONAR_PORT);
      systick_wait_ms(50);
      ter = ecrobot_get_sonar_sensor(SONAR_PORT);
      systick_wait_ms(50);
      cua = ecrobot_get_sonar_sensor(SONAR_PORT);
      systick_wait_ms(50);
        SetEvent(Principal,medir);
        TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Calibrar                                       */
    /* Priority: 3                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Ajusta los motores y corrige el avance        */
    /*----------------------------------------------------------*/
    TASK(Calibrar)
    {
        int rpmB = nxt_motor_get_count(NXT_PORT_B);
        int rpmC = nxt_motor_get_count(NXT_PORT_C);

        int margen = 10;

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
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Pulsador                                       */
    /* Priority: 9                                              */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Detecta si se pulsa el pulsador               */
    /*----------------------------------------------------------*/
    TASK(Pulsador)
    {
              if(ecrobot_get_touch_sensor(PULSADOR_PORT)==1){
       
      
                /*Retroceso*/
       int time_out_a = systick_get_ms() + 500 ;
       while(systick_get_ms() < time_out_a){
        nxt_motor_set_speed(NXT_PORT_B, -100, 1);
        nxt_motor_set_speed(NXT_PORT_C, -100, 1);
  
    }
  }
      TerminateTask();
    }

    /*----------------------------------------------------------*/
    /* Task information:                                        */
    /* -----------------                                        */
    /* Name    : Final                                          */
    /* Priority: 10                                             */
    /* Typ     : EXTENDED TASK                                  */
    /* Schedule: FULL                                           */
    /* Objective: Detiene motores y cancela las alarmas         */
    /*----------------------------------------------------------*/
    TASK(Final)
    {
        // Para los motores
        nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        nxt_motor_set_speed(NXT_PORT_C, 0, 1);

        CancelAlarm(AlCalibrar);
        CancelAlarm(AlDistancia);
        CancelAlarm(AlPulsador);


    }
