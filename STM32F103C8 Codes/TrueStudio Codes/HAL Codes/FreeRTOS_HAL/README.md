# CMSIS-RTOS CODE
The CMSIS-RTOS is a common API for Real-Time operating systems. It provides a standardized programming interface that is portable to many RTOS and enables therefore software templates, middleware, libraries, and other components that can work across supported the RTOS systems. FreeRTOS is a market-leading real-time operating system (RTOS) for microcontrollers and small microprocessors specified to use with multi-task applications, where processing time is critical.
When working with non dependant tasks, where each task can not postpone the others, RTOS solution is needed to handle all kinds of multi-task environments.   

**see References at the end. 

## 1.TUTORIAL: STARTING WITH FreeRTOS ON STM32CUBEMX

Follow the video link: https://www.youtube.com/watch?v=Ds1xcyn-vEU&t=525s
*This is how to implement a simple code running on a STM32 BluePill board*

### 1.1.ADD NEW THREADS MANUALLY
 Step 1. Add between the begin and the end of user code a name to define the new thread:
![Defining a Handle](https://github.com/EMendesS/STM32F103C8-Codes/blob/master/FreeRTOS_HAL/osThreadId.JPG)

 Step 2. Add between the begin and the end of user code the *osThreadId* you defined previously, a name for the function called as the task is instatiated, the priority, number of instances and the stack size (in bytes). 
![Creating a Thread](https://github.com/EMendesS/STM32F103C8-Codes/blob/master/FreeRTOS_HAL/Priority.JPG)
In addition, the *osThreadCreate* adds and passes the pointer to the local variable created (called in the figure above as "myTask03") in the *osThreadDef*. The second parameter of this function is the initialization value to the created task.

*OBS.1: The creation of the task will return a ThreadID, that was stored in "myTask03Handle". It can be checked: if not NULL, the thread creation was successful. If not, it means error (so you will not be able to use this thread).
To suspend or get the state of the thread, you need this handle to the task.*

*OBS.2: The instances can be 3, in the exemple bellow:*

``` C

#include "cmsis_os.h"
 
void Thread (void const *arg);                             // function prototype for a Thread
osThreadDef (Thread, osPriorityNormal, 3, 0);              // define Thread and specify to allow three instances
 
void ThreadCreate_example (void) {
  osThreadId id1, id2, id3;
  
  id1 = osThreadCreate (osThread (Thread), NULL);          // create the thread with id1
  id2 = osThreadCreate (osThread (Thread), NULL);          // create the thread with id2
  id3 = osThreadCreate (osThread (Thread), NULL);          // create the thread with id3
  if (id1 == NULL) {                                       // handle thread creation for id1
    // Failed to create the thread with id1
  }
  if (id2 == NULL) {                                       // handle thread creation for id2
    // Failed to create the thread with id2
  }
  if (id3 == NULL) {                                       // handle thread creation for id3
    // Failed to create the thread with id3
  }
  :
  osThreadTerminate (id1);                                  // stop the thread with id1
  osThreadTerminate (id2);                                  // stop the thread with id2
  osThreadTerminate (id3);                                  // stop the thread with id3
}

```

 Step 3. Add between the begin and the end of user code a function prototype as described in the thread definition (in the number 2, above):
![Function prototype](https://github.com/EMendesS/STM32F103C8-Codes/blob/master/FreeRTOS_HAL/Thread3.JPG)

### 1.2.INTERTASK COMMUNICATION
[To Do]

## 2. REFERENCES

* FreeRTOS Documentation: https://www.freertos.org/Documentation/RTOS_book.html (set of videos of FreeRTOS: https://www.youtube.com/watch?v=7efj3bJbGbk&list=PLEfMFrwVdbPYzMgeaLiFRb4ogjV8m3lt6&index=1)

* CMSIS-RTOS Documentation: https://www.keil.com/pack/doc/CMSIS/RTOS/html/index.html

* The summarize (in portuguese): https://www.embarcados.com.br/rtos-sistema-operacional-de-tempo-real/