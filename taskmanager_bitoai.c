#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "task_manager.h"
// Test helper functions
static void assert_task_equals(task_t* task, uint32_t id, const char* name, 
                             task_state_t state, uint32_t priority, uint32_t stack_size) {
    assert(task != NULL);
    assert(task->task_id == id);
    assert(strcmp(task->name, name) == 0);
    assert(task->state == state);
    assert(task->priority == priority);
    assert(task->stack_size == stack_size);
}
// Test cases
void test_task_manager_init(void) {
    task_manager_init();
    assert(task_get_count() == 0);
    printf("✓ test_task_manager_init passed\n");
}
void test_task_create(void) {
    task_manager_init();
    
    // Test successful task creation
    assert(task_create(1, "Task1", 5, 1024) == true);
    assert(task_get_count() == 1);
    
    // Test duplicate task ID
    assert(task_create(1, "Task2", 3, 2048) == false);
    
    // Test NULL name
    assert(task_create(2, NULL, 1, 512) == false);
    
    // Test max tasks limit
    for (uint32_t i = 2; i <= MAX_TASKS; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Task%u", i);
        task_create(i, name, 1, 512);
    }
    assert(task_create(MAX_TASKS + 1, "ExtraTask", 1, 512) == false);
    
    printf("✓ test_task_create passed\n");
}
void test_task_delete(void) {
    task_manager_init();
    
    // Create some tasks
    task_create(1, "Task1", 5, 1024);
    task_create(2, "Task2", 3, 2048);
    
    // Test successful deletion
    assert(task_delete(1) == true);
    assert(task_get_count() == 1);
    
    // Test deleting non-existent task
    assert(task_delete(99) == false);
    
    printf("✓ test_task_delete passed\n");
}
void test_task_get(void) {
    task_manager_init();
    
    // Create a task
    task_create(1, "Task1", 5, 1024);
    
    // Test getting existing task
    task_t* task = task_get(1);
    assert_task_equals(task, 1, "Task1", TASK_READY, 5, 1024);
    
    // Test getting non-existent task
    assert(task_get(99) == NULL);
    
    printf("✓ test_task_get passed\n");
}
void test_task_set_state(void) {
    task_manager_init();
    
    // Create a task
    task_create(1, "Task1", 5, 1024);
    
    // Test setting valid states
    assert(task_set_state(1, TASK_RUNNING) == true);
    assert(task_get(1)->state == TASK_RUNNING);
    
    assert(task_set_state(1, TASK_BLOCKED) == true);
    assert(task_get(1)->state == TASK_BLOCKED);
    
    // Test setting state for non-existent task
    assert(task_set_state(99, TASK_RUNNING) == false);
    
    printf("✓ test_task_set_state passed\n");
}
int main(void) {
    printf("Running task manager tests...\n");
    
    test_task_manager_init();
    test_task_create();
    test_task_delete();
    test_task_get();
    test_task_set_state();
    
    printf("\nAll tests passed!\n");
    return 0;
}