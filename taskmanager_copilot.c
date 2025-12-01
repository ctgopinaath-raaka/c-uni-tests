#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "task_manager.h"

// Simple testing framework
#define TEST_PASSED(test_name) printf("✓ %s\n", test_name)
#define TEST_FAILED(test_name, msg) printf("✗ %s - %s\n", test_name, msg)

static int tests_passed = 0;
static int tests_failed = 0;

// Helper macro for assertions
#define ASSERT_TRUE(condition, test_name, msg) \
    if (!(condition)) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

#define ASSERT_FALSE(condition, test_name, msg) \
    if ((condition)) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

#define ASSERT_EQUAL(actual, expected, test_name, msg) \
    if ((actual) != (expected)) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

#define ASSERT_STRING_EQUAL(actual, expected, test_name, msg) \
    if (strcmp((actual), (expected)) != 0) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

#define ASSERT_NULL(ptr, test_name, msg) \
    if ((ptr) != NULL) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

#define ASSERT_NOT_NULL(ptr, test_name, msg) \
    if ((ptr) == NULL) { \
        TEST_FAILED(test_name, msg); \
        tests_failed++; \
    } else { \
        TEST_PASSED(test_name); \
        tests_passed++; \
    }

// ============================================================================
// TEST SUITE: Task Manager Initialization
// ============================================================================

void test_task_manager_init(void) {
    printf("\n--- TEST: task_manager_init ---\n");
    
    task_manager_init();
    ASSERT_EQUAL(task_get_count(), 0, "test_task_manager_init", 
                 "Task count should be 0 after init");
}

// ============================================================================
// TEST SUITE: Task Create
// ============================================================================

void test_task_create_single_task(void) {
    printf("\n--- TEST: task_create_single_task ---\n");
    
    task_manager_init();
    bool result = task_create(1, "Task1", 5, 1024);
    
    ASSERT_TRUE(result, "test_task_create_single_task", 
                "Should successfully create a task");
    ASSERT_EQUAL(task_get_count(), 1, "test_task_create_single_task", 
                 "Task count should be 1");
}

void test_task_create_multiple_tasks(void) {
    printf("\n--- TEST: task_create_multiple_tasks ---\n");
    
    task_manager_init();
    bool result1 = task_create(1, "Task1", 5, 1024);
    bool result2 = task_create(2, "Task2", 3, 2048);
    bool result3 = task_create(3, "Task3", 7, 512);
    
    ASSERT_TRUE(result1, "test_task_create_multiple_tasks", 
                "First task creation should succeed");
    ASSERT_TRUE(result2, "test_task_create_multiple_tasks", 
                "Second task creation should succeed");
    ASSERT_TRUE(result3, "test_task_create_multiple_tasks", 
                "Third task creation should succeed");
    ASSERT_EQUAL(task_get_count(), 3, "test_task_create_multiple_tasks", 
                 "Task count should be 3");
}

void test_task_create_max_tasks(void) {
    printf("\n--- TEST: task_create_max_tasks ---\n");
    
    task_manager_init();
    
    // Fill up to MAX_TASKS
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        char name[20];
        snprintf(name, sizeof(name), "Task%d", i);
        bool result = task_create(i, name, i, 1024);
        ASSERT_TRUE(result, "test_task_create_max_tasks", 
                    "Should create task up to MAX_TASKS");
    }
    
    // Try to create one more (should fail)
    bool result = task_create(MAX_TASKS, "ExtraTask", 5, 1024);
    ASSERT_FALSE(result, "test_task_create_max_tasks", 
                 "Should fail to create task beyond MAX_TASKS");
    ASSERT_EQUAL(task_get_count(), MAX_TASKS, "test_task_create_max_tasks", 
                 "Task count should still be MAX_TASKS");
}

void test_task_create_duplicate_id(void) {
    printf("\n--- TEST: task_create_duplicate_id ---\n");
    
    task_manager_init();
    bool result1 = task_create(1, "Task1", 5, 1024);
    bool result2 = task_create(1, "Task1Duplicate", 3, 2048);
    
    ASSERT_TRUE(result1, "test_task_create_duplicate_id", 
                "First task creation should succeed");
    ASSERT_FALSE(result2, "test_task_create_duplicate_id", 
                 "Should reject duplicate task ID");
    ASSERT_EQUAL(task_get_count(), 1, "test_task_create_duplicate_id", 
                 "Task count should still be 1");
}

void test_task_create_null_name(void) {
    printf("\n--- TEST: task_create_null_name ---\n");
    
    task_manager_init();
    bool result = task_create(1, NULL, 5, 1024);
    
    ASSERT_FALSE(result, "test_task_create_null_name", 
                 "Should reject task with NULL name");
    ASSERT_EQUAL(task_get_count(), 0, "test_task_create_null_name", 
                 "Task count should be 0");
}

void test_task_create_name_truncation(void) {
    printf("\n--- TEST: task_create_name_truncation ---\n");
    
    task_manager_init();
    const char* long_name = "VeryLongTaskNameThatExceedsMaxLength";
    bool result = task_create(1, long_name, 5, 1024);
    task_t* task = task_get(1);
    
    ASSERT_TRUE(result, "test_task_create_name_truncation", 
                "Should create task with long name");
    ASSERT_NOT_NULL(task, "test_task_create_name_truncation", 
                    "Task should be retrievable");
    ASSERT_TRUE(strlen(task->name) < strlen(long_name), 
                "test_task_create_name_truncation", 
                "Long name should be truncated");
}

void test_task_create_state_initialized(void) {
    printf("\n--- TEST: task_create_state_initialized ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    task_t* task = task_get(1);
    
    ASSERT_NOT_NULL(task, "test_task_create_state_initialized", 
                    "Task should be retrievable");
    ASSERT_EQUAL(task->state, TASK_READY, "test_task_create_state_initialized", 
                 "Initial state should be TASK_READY");
}

// ============================================================================
// TEST SUITE: Task Get
// ============================================================================

void test_task_get_existing_task(void) {
    printf("\n--- TEST: task_get_existing_task ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    task_t* task = task_get(1);
    
    ASSERT_NOT_NULL(task, "test_task_get_existing_task", 
                    "Should retrieve existing task");
    ASSERT_EQUAL(task->task_id, 1, "test_task_get_existing_task", 
                 "Task ID should match");
    ASSERT_STRING_EQUAL(task->name, "Task1", "test_task_get_existing_task", 
                        "Task name should match");
}

void test_task_get_non_existing_task(void) {
    printf("\n--- TEST: task_get_non_existing_task ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    task_t* task = task_get(999);
    
    ASSERT_NULL(task, "test_task_get_non_existing_task", 
                "Should return NULL for non-existing task");
}

void test_task_get_empty_list(void) {
    printf("\n--- TEST: task_get_empty_list ---\n");
    
    task_manager_init();
    task_t* task = task_get(1);
    
    ASSERT_NULL(task, "test_task_get_empty_list", 
                "Should return NULL when task list is empty");
}

// ============================================================================
// TEST SUITE: Task Delete
// ============================================================================

void test_task_delete_existing_task(void) {
    printf("\n--- TEST: task_delete_existing_task ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    bool result = task_delete(1);
    
    ASSERT_TRUE(result, "test_task_delete_existing_task", 
                "Should successfully delete existing task");
    ASSERT_EQUAL(task_get_count(), 0, "test_task_delete_existing_task", 
                 "Task count should be 0 after delete");
}

void test_task_delete_non_existing_task(void) {
    printf("\n--- TEST: task_delete_non_existing_task ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    bool result = task_delete(999);
    
    ASSERT_FALSE(result, "test_task_delete_non_existing_task", 
                 "Should fail to delete non-existing task");
    ASSERT_EQUAL(task_get_count(), 1, "test_task_delete_non_existing_task", 
                 "Task count should remain 1");
}

void test_task_delete_from_middle(void) {
    printf("\n--- TEST: task_delete_from_middle ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    task_create(2, "Task2", 3, 2048);
    task_create(3, "Task3", 7, 512);
    
    bool result = task_delete(2);
    
    ASSERT_TRUE(result, "test_task_delete_from_middle", 
                "Should successfully delete task from middle");
    ASSERT_EQUAL(task_get_count(), 2, "test_task_delete_from_middle", 
                 "Task count should be 2");
    ASSERT_NOT_NULL(task_get(1), "test_task_delete_from_middle", 
                    "Task 1 should still exist");
    ASSERT_NULL(task_get(2), "test_task_delete_from_middle", 
                "Task 2 should be deleted");
    ASSERT_NOT_NULL(task_get(3), "test_task_delete_from_middle", 
                    "Task 3 should still exist");
}

void test_task_delete_multiple_tasks(void) {
    printf("\n--- TEST: task_delete_multiple_tasks ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    task_create(2, "Task2", 3, 2048);
    task_create(3, "Task3", 7, 512);
    
    bool result1 = task_delete(1);
    bool result2 = task_delete(3);
    
    ASSERT_TRUE(result1, "test_task_delete_multiple_tasks", 
                "Should delete first task");
    ASSERT_TRUE(result2, "test_task_delete_multiple_tasks", 
                "Should delete third task");
    ASSERT_EQUAL(task_get_count(), 1, "test_task_delete_multiple_tasks", 
                 "Task count should be 1");
    ASSERT_NOT_NULL(task_get(2), "test_task_delete_multiple_tasks", 
                    "Task 2 should be the only remaining task");
}

// ============================================================================
// TEST SUITE: Task Set State
// ============================================================================

void test_task_set_state_valid(void) {
    printf("\n--- TEST: task_set_state_valid ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    bool result = task_set_state(1, TASK_RUNNING);
    task_t* task = task_get(1);
    
    ASSERT_TRUE(result, "test_task_set_state_valid", 
                "Should successfully set task state");
    ASSERT_EQUAL(task->state, TASK_RUNNING, "test_task_set_state_valid", 
                 "Task state should be updated");
}

void test_task_set_state_multiple_changes(void) {
    printf("\n--- TEST: task_set_state_multiple_changes ---\n");
    
    task_manager_init();
    task_create(1, "Task1", 5, 1024);
    
    bool result1 = task_set_state(1, TASK_RUNNING);
    bool result2 = task_set_state(1, TASK_BLOCKED);
    bool result3 = task_set_state(1, TASK_SUSPENDED);
    bool result4 = task_set_state(1, TASK_READY);
    
    task_t* task = task_get(1);
    
    ASSERT_TRUE(result1, "test_task_set_state_multiple_changes", 
                "First state change should succeed");
    ASSERT_TRUE(result2, "test_task_set_state_multiple_changes", 
                "Second state change should succeed");
    ASSERT_TRUE(result3, "test_task_set_state_multiple_changes", 
                "Third state change should succeed");
    ASSERT_TRUE(result4, "test_task_set_state_multiple_changes", 
                "Fourth state change should succeed");
    ASSERT_EQUAL(task->state, TASK_READY, "test_task_set_state_multiple_changes", 
                 "Task state should be TASK_READY");
}

void test_task_set_state_non_existing_task(void) {
    printf("\n--- TEST: task_set_state_non_existing_task ---\n");
    
    task_manager_init();
    bool result = task_set_state(999, TASK_RUNNING);
    
    ASSERT_FALSE(result, "test_task_set_state_non_existing_task", 
                 "Should fail to set state for non-existing task");
}

// ============================================================================
// TEST SUITE: Task Get Count
// ============================================================================

void test_task_get_count_empty(void) {
    printf("\n--- TEST: task_get_count_empty ---\n");
    
    task_manager_init();
    uint32_t count = task_get_count();
    
    ASSERT_EQUAL(count, 0, "test_task_get_count_empty", 
                 "Count should be 0 for empty task list");
}

void test_task_get_count_after_operations(void) {
    printf("\n--- TEST: task_get_count_after_operations ---\n");
    
    task_manager_init();
    ASSERT_EQUAL(task_get_count(), 0, "test_task_get_count_after_operations", 
                 "Initial count should be 0");
    
    task_create(1, "Task1", 5, 1024);
    ASSERT_EQUAL(task_get_count(), 1, "test_task_get_count_after_operations", 
                 "Count should be 1 after creating first task");
    
    task_create(2, "Task2", 3, 2048);
    ASSERT_EQUAL(task_get_count(), 2, "test_task_get_count_after_operations", 
                 "Count should be 2 after creating second task");
    
    task_delete(1);
    ASSERT_EQUAL(task_get_count(), 1, "test_task_get_count_after_operations", 
                 "Count should be 1 after deleting one task");
}

// ============================================================================
// RUN ALL TESTS
// ============================================================================

int main(void) {
    printf("========================================\n");
    printf("    TASK MANAGER UNIT TEST SUITE\n");
    printf("========================================\n");
    
    // Initialization tests
    test_task_manager_init();
    
    // Create tests
    test_task_create_single_task();
    test_task_create_multiple_tasks();
    test_task_create_max_tasks();
    test_task_create_duplicate_id();
    test_task_create_null_name();
    test_task_create_name_truncation();
    test_task_create_state_initialized();
    
    // Get tests
    test_task_get_existing_task();
    test_task_get_non_existing_task();
    test_task_get_empty_list();
    
    // Delete tests
    test_task_delete_existing_task();
    test_task_delete_non_existing_task();
    test_task_delete_from_middle();
    test_task_delete_multiple_tasks();
    
    // Set state tests
    test_task_set_state_valid();
    test_task_set_state_multiple_changes();
    test_task_set_state_non_existing_task();
    
    // Count tests
    test_task_get_count_empty();
    test_task_get_count_after_operations();
    
    // Print summary
    printf("\n========================================\n");
    printf("          TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Total Tests:  %d\n", tests_passed + tests_failed);
    printf("========================================\n");
    
    return (tests_failed == 0) ? 0 : 1;
}