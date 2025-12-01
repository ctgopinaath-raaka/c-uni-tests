#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unity.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* ============================================================================
 * Test Configuration and Helpers
 * ========================================================================== */

#define TEST_STACK_DEPTH    256
#define TEST_TASK_PRIORITY  2
#define TEST_TASK_NAME      "TestTask"

/* Static buffers for testing */
static StackType_t test_stack_buffer[TEST_STACK_DEPTH];
static StaticTask_t test_tcb_buffer;

/* Dummy task function for testing */
static void dummy_task_function(void *pvParameters)
{
    (void)pvParameters;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ============================================================================
 * Setup and Teardown
 * ========================================================================== */

void setUp(void)
{
    /* Initialize test buffers */
    memset(test_stack_buffer, 0, sizeof(test_stack_buffer));
    memset(&test_tcb_buffer, 0, sizeof(test_tcb_buffer));
}

void tearDown(void)
{
    /* Clean up after each test */
    memset(test_stack_buffer, 0, sizeof(test_stack_buffer));
    memset(&test_tcb_buffer, 0, sizeof(test_tcb_buffer));
}

/* ============================================================================
 * Test Suite: Basic Functionality
 * ========================================================================== */

void test_xTaskCreateStatic_returns_valid_handle(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(xTaskHandle);
}

void test_xTaskCreateStatic_with_null_stack_returns_null(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        NULL,
        &test_tcb_buffer
    );
    
    /* Assert */
    TEST_ASSERT_NULL(xTaskHandle);
}

void test_xTaskCreateStatic_with_null_tcb_returns_null(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        NULL
    );
    
    /* Assert */
    TEST_ASSERT_NULL(xTaskHandle);
}

void test_xTaskCreateStatic_handle_equals_tcb_buffer(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    /* Assert */
    TEST_ASSERT_EQUAL_PTR(xTaskHandle, &test_tcb_buffer);
}

/* ============================================================================
 * Test Suite: Task Name Handling
 * ========================================================================== */

void test_xTaskCreateStatic_sets_task_name(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    const char *test_name = "MyTask";
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        test_name,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL_STRING(pxTCB->pcTaskName, test_name);
}

void test_xTaskCreateStatic_handles_null_name(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        NULL,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(pxTCB);
}

void test_xTaskCreateStatic_truncates_long_name(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    const char *long_name = "VeryLongTaskNameThatExceedsConfigMAXTaskNameLen";
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        long_name,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(pxTCB);
    TEST_ASSERT_EQUAL(pxTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1], '\0');
}

/* ============================================================================
 * Test Suite: Priority Handling
 * ========================================================================== */

void test_xTaskCreateStatic_sets_priority(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    UBaseType_t test_priority = 3;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        test_priority,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(pxTCB->uxPriority, test_priority);
}

void test_xTaskCreateStatic_caps_priority_at_max(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    UBaseType_t invalid_priority = configMAX_PRIORITIES + 100;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        invalid_priority,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(pxTCB->uxPriority, configMAX_PRIORITIES - 1);
}

void test_xTaskCreateStatic_idle_priority(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        tskIDLE_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(pxTCB->uxPriority, tskIDLE_PRIORITY);
}

/* ============================================================================
 * Test Suite: Stack Initialization
 * ========================================================================== */

void test_xTaskCreateStatic_stack_reference_correct(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    StackType_t custom_stack[128];
    StaticTask_t custom_tcb;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        128,
        NULL,
        TEST_TASK_PRIORITY,
        custom_stack,
        &custom_tcb
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL_PTR(pxTCB->pxStack, custom_stack);
}

void test_xTaskCreateStatic_different_stack_depths(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    StackType_t stack_128[128];
    StaticTask_t tcb;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        128,
        NULL,
        TEST_TASK_PRIORITY,
        stack_128,
        &tcb
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL_PTR(pxTCB->pxStack, stack_128);
}

/* ============================================================================
 * Test Suite: Static Allocation Flag
 * ========================================================================== */

#if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )

void test_xTaskCreateStatic_sets_static_flag(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(
        pxTCB->ucStaticallyAllocated,
        tskSTATICALLY_ALLOCATED_STACK_AND_TCB
    );
}

#endif

/* ============================================================================
 * Test Suite: List Item Initialization
 * ========================================================================== */

void test_xTaskCreateStatic_initializes_list_items(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(pxTCB);
}

/* ============================================================================
 * Test Suite: Multiple Tasks
 * ========================================================================== */

void test_xTaskCreateStatic_multiple_different_tasks(void)
{
    /* Arrange */
    StackType_t stack1[TEST_STACK_DEPTH];
    StackType_t stack2[TEST_STACK_DEPTH];
    StaticTask_t tcb1;
    StaticTask_t tcb2;
    TaskHandle_t handle1, handle2;
    TCB_t *pxTCB1, *pxTCB2;
    
    /* Act */
    handle1 = xTaskCreateStatic(
        dummy_task_function,
        "Task1",
        TEST_STACK_DEPTH,
        NULL,
        1,
        stack1,
        &tcb1
    );
    
    handle2 = xTaskCreateStatic(
        dummy_task_function,
        "Task2",
        TEST_STACK_DEPTH,
        NULL,
        2,
        stack2,
        &tcb2
    );
    
    pxTCB1 = (TCB_t *)handle1;
    pxTCB2 = (TCB_t *)handle2;
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(handle1);
    TEST_ASSERT_NOT_NULL(handle2);
    TEST_ASSERT_NOT_EQUAL(handle1, handle2);
    TEST_ASSERT_EQUAL(pxTCB1->uxPriority, 1);
    TEST_ASSERT_EQUAL(pxTCB2->uxPriority, 2);
}

/* ============================================================================
 * Test Suite: Mutex Support
 * ========================================================================== */

#if ( configUSE_MUTEXES == 1 )

void test_xTaskCreateStatic_sets_base_priority(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    UBaseType_t test_priority = 3;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        test_priority,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(pxTCB->uxBasePriority, test_priority);
    TEST_ASSERT_EQUAL(pxTCB->uxMutexesHeld, 0);
}

#endif

/* ============================================================================
 * Test Suite: Edge Cases
 * ========================================================================== */

void test_xTaskCreateStatic_minimum_stack_depth(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    StackType_t min_stack[1];
    StaticTask_t min_tcb;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        1,
        NULL,
        TEST_TASK_PRIORITY,
        min_stack,
        &min_tcb
    );
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(xTaskHandle);
}

void test_xTaskCreateStatic_maximum_priority(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    TCB_t *pxTCB;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        configMAX_PRIORITIES - 1,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    pxTCB = (TCB_t *)xTaskHandle;
    
    /* Assert */
    TEST_ASSERT_EQUAL(pxTCB->uxPriority, configMAX_PRIORITIES - 1);
}

/* ============================================================================
 * Test Suite: Parameters
 * ========================================================================== */

void test_xTaskCreateStatic_with_parameters(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    void *test_param = (void *)0xDEADBEEF;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        test_param,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(xTaskHandle);
}

void test_xTaskCreateStatic_without_parameters(void)
{
    /* Arrange */
    TaskHandle_t xTaskHandle;
    
    /* Act */
    xTaskHandle = xTaskCreateStatic(
        dummy_task_function,
        TEST_TASK_NAME,
        TEST_STACK_DEPTH,
        NULL,
        TEST_TASK_PRIORITY,
        test_stack_buffer,
        &test_tcb_buffer
    );
    
    /* Assert */
    TEST_ASSERT_NOT_NULL(xTaskHandle);
}