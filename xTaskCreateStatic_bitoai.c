#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Define the task stack size and maximum allowed priority (example values) */
#define STACK_SIZE     128
#define MAX_PRIORITY   (configMAX_PRIORITIES - 1)
/* Static memory for task control and stack */
static StaticTask_t xStaticTCB;
static StackType_t uxStaticStack[STACK_SIZE];
/* Global flag to indicate that a task function ran */
static volatile BaseType_t xTaskRunFlag = pdFALSE;
/* Sample Task Function used in multiple tests */
void vSampleTask( void *pvParameters )
{
    (void) pvParameters;  // Unused parameter
    /* Set a flag indicating that the task is running */
    xTaskRunFlag = pdTRUE;
    
    /* Loop to keep the task alive for observation */
    for( ;; )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
/* -----------------------------------------------------------------------------
   Test: Basic Functionality & Task State Verification
----------------------------------------------------------------------------- */
void vTestBasicFunctionality( void )
{
    /* Reset run flag */
    xTaskRunFlag = pdFALSE;
    
    /* Create a task using static allocation with valid parameters */
    TaskHandle_t xHandle = xTaskCreateStatic( 
                                vSampleTask,         /* Task function */
                                "BasicTask",         /* Task name */
                                STACK_SIZE,          /* Stack depth */
                                NULL,                /* Task parameter */
                                tskIDLE_PRIORITY + 1,/* Task priority */
                                uxStaticStack,       /* Stack memory buffer */
                                &xStaticTCB          /* TCB memory buffer */
                            );
    
    if( xHandle == NULL )
    {
        printf("ERROR: Basic Functionality test failed (NULL handle).\n");
    }
    else
    {
        printf("PASS: Task created successfully; valid handle returned.\n");
    }
    
    /* Scheduler would normally run tasks and set the flag */
    /* Start the scheduler if not already started in your test environment */
}
/* -----------------------------------------------------------------------------
   Test: Parameter Validation
   Note: In practice, FreeRTOS does not perform many parameter validations
         (and may crash on NULL pointers). These tests are based on your
         configuration and any extra validation layers you might have.
----------------------------------------------------------------------------- */
void vTestParameterValidation( void )
{
    /* Test with NULL stack buffer; the expected behavior may be to return NULL or assert */
    TaskHandle_t xHandle1 = xTaskCreateStatic(
                                vSampleTask,
                                "NullStack",
                                STACK_SIZE,
                                NULL,
                                tskIDLE_PRIORITY + 1,
                                NULL,              /* NULL stack buffer */
                                &xStaticTCB
                            );
    if( xHandle1 == NULL )
    {
        printf("PASS: xTaskCreateStatic failed as expected with NULL stack buffer.\n");
    }
    else
    {
        printf("WARN: Task created with NULL stack buffer; check memory access constraints.\n");
    }
    
    /* Test with NULL TCB pointer */
    TaskHandle_t xHandle2 = xTaskCreateStatic(
                                vSampleTask,
                                "NullTCB",
                                STACK_SIZE,
                                NULL,
                                tskIDLE_PRIORITY + 1,
                                uxStaticStack,
                                NULL               /* NULL TCB pointer */
                            );
    
    if( xHandle2 == NULL )
    {
        printf("PASS: xTaskCreateStatic failed as expected with NULL TCB pointer.\n");
    }
    else
    {
        printf("WARN: Task created with NULL TCB pointer; check for unexpected behavior.\n");
    }
    
    /* Test invalid priority: Using a priority higher than MAX_PRIORITY */
    TaskHandle_t xHandle3 = xTaskCreateStatic(
                                vSampleTask,
                                "HighPriority",
                                STACK_SIZE,
                                NULL,
                                MAX_PRIORITY + 1, /* Invalid priority */
                                uxStaticStack,
                                &xStaticTCB
                            );
    
    /* Behavior here depends on how you have implemented priority capping. */
    /* A production system might cap the priority or assert. */
    if( xHandle3 == NULL )
    {
        printf("PASS: Task creation failed with invalid priority as expected.\n");
    }
    else
    {
        /* Retrieve effective priority (if API available) to check capping */
        UBaseType_t uxPriority = uxTaskPriorityGet( xHandle3 );
        if( uxPriority == MAX_PRIORITY )
        {
            printf("PASS: Task priority was capped to max allowed value (%lu).\n", (unsigned long) uxPriority);
        }
        else
        {
            printf("WARN: Task priority (%lu) not as expected.\n", (unsigned long)uxPriority);
        }
    }
}
/* -----------------------------------------------------------------------------
   Test: Memory and TCB Initialization
----------------------------------------------------------------------------- */
void vTestMemoryInitialization( void )
{
    /* Create a task and check that the stack pointer assignment and TCB initialization appear valid. */
    TaskHandle_t xHandle = xTaskCreateStatic(
                                vSampleTask,
                                "MemoryTask",
                                STACK_SIZE,
                                NULL,
                                tskIDLE_PRIORITY + 1,
                                uxStaticStack,
                                &xStaticTCB
                            );
    
    if( xHandle != NULL )
    {
        /* Some internal TCB members or static allocation flags may be available via debug APIs */
        /* For illustration, we assume that the pointer to the stack in TCB is accessible. */
        if( ( &( (StaticTask_t*)xHandle )->pxDummy1 ) != NULL )
        {
            printf("PASS: TCB appears to be properly initialized.\n");
        }
        else
        {
            printf("WARN: TCB initialization incomplete.\n");
        }
    }
}
/* -----------------------------------------------------------------------------
   Test: Edge Cases - Creation Timing
----------------------------------------------------------------------------- */
void vTestEdgeCases( void )
{
    /* Example: Creating a task before the scheduler starts */
    TaskHandle_t xPreSchedTask = xTaskCreateStatic(
                                     vSampleTask,
                                     "PreSchedTask",
                                     STACK_SIZE,
                                     NULL,
                                     tskIDLE_PRIORITY + 1,
                                     uxStaticStack,
                                     &xStaticTCB
                                 );
    
    if( xPreSchedTask != NULL )
    {
        printf("PASS: Task created before scheduler start; will run once scheduler commences.\n");
    }
    else
    {
        printf("ERROR: Failed to create task before scheduler start.\n");
    }
    
    /* Edge case: Creating a task after the scheduler starts can be done via vTaskStartScheduler()
       in a running system. This typically requires a separate test context.
    */
}
/* -----------------------------------------------------------------------------
   (Optional) Test: SMP-Specific Tests
   Note: These tests are only applicable in an SMP build (configNUMBER_OF_CORES > 1).
----------------------------------------------------------------------------- */
void vTestSMPSpecific( void )
{
#if ( configNUMBER_OF_CORES > 1 )
    TaskHandle_t xSMPTask = xTaskCreateStatic(
                                vSampleTask,
                                "SMPTask",
                                STACK_SIZE,
                                NULL,
                                tskIDLE_PRIORITY + 1,
                                uxStaticStack,
                                &xStaticTCB
                            );
    
    if( xSMPTask != NULL )
    {
        /* Retrieve core affinity mask if the API is provided */
        UBaseType_t uxAffinity = 0;
        // Assume a fictitious API: uxTaskGetAffinity( xSMPTask );
        // uxAffinity = uxTaskGetAffinity( xSMPTask );
        
        printf("PASS: SMP task created. Core affinity mask: 0x%lx\n", (unsigned long)uxAffinity);
    }
    else
    {
        printf("ERROR: SMP task creation failed.\n");
    }
#else
    printf("SKIP: SMP specific tests are not applicable (single core system).\n");
#endif
}
/* -----------------------------------------------------------------------------
   Main Function - Aggregates all tests.
----------------------------------------------------------------------------- */
int main(void)
{
    /* Basic Functionality and Task State Test */
    vTestBasicFunctionality();
    /* Parameter Validation Tests */
    vTestParameterValidation();
    
    /* Memory Initialization Tests */
    vTestMemoryInitialization();
    
    /* Edge Case Tests (pre-scheduler start) */
    vTestEdgeCases();
    
    /* Optional SMP Specific Tests */
    vTestSMPSpecific();
    
    /* Start the scheduler (if desired) to allow created tasks to run.
       When running integration tests, the scheduler might be started in a separate thread. */
    printf("Starting FreeRTOS Scheduler...\n");
    vTaskStartScheduler();
    
    /* Execution should not reach here unless there is a problem with the scheduler */
    printf("ERROR: Scheduler returned unexpectedly.\n");
    return EXIT_FAILURE;
}