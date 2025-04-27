#ifndef _LIST_STATUS_H
#define _LIST_STATUS_H

#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define LIST_IDEL       0
#define LIST_RUN        1

typedef struct list_status
{
    /* data */
    char name[50];
    struct list_status *next;
    struct list_status *prev;
    void (*enter)(void); // 进入状态时的动作
    void (*exit)(void); // 离开状态时的动作
    void (*handle)(void); // 处理状态内部事件的动作
}list_status_t;

/**
 * @brief Creates a new status object.
 *
 * Allocates memory for a new status object, initializes its members, and assigns the provided callbacks.
 *
 * @param name The name of the status.
 * @param enter Pointer to the function to be called when entering the status.
 * @param exit Pointer to the function to be called when exiting the status.
 * @param handle Pointer to the function to be called to handle the status.
 * @return Pointer to the newly created status object, or NULL if memory allocation fails.
 */
list_status_t* list_status_create(char *name, void (*enter)(void), void (*exit)(void), void (*handle)(void));

/**
 * @brief Destroys a status object.
 *
 * Frees the memory allocated for the status object and sets the pointer to NULL.
 *
 * @param status Double pointer to the status object to be destroyed.
 */
void list_status_destroy(list_status_t **status);

/**
 * @brief Adds a new status object to the list.
 *
 * Inserts a new status object after the head of the list. Updates the next and previous pointers accordingly.
 *
 * @param head Pointer to the head of the list.
 * @param new_status Pointer to the new status object to be added.
 */
void list_status_add(list_status_t **head, list_status_t *new_status);

/**
 * @brief Removes a status object from the list.
 *
 * Removes the specified status object from the list by updating the next and previous pointers of its neighbors.
 * Frees the memory allocated for the status object.
 *
 * @param status Pointer to the status object to be removed.
 */
void list_status_remove(list_status_t *status);

/**
 * @brief Polls the status list and transitions between statuses.
 *
 * Executes the handle function of the current status, calls the exit function if transitioning out,
 * and calls the enter function of the next status. If the end of the list is reached, it resets to the head.
 *
 * @param head Pointer to the head of the list.
 * @param current Double pointer to the current status object being polled.
 */
void list_poll(list_status_t *head, list_status_t **current);

#endif /* _LIST_STATUS_H */
