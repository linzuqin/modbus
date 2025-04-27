/**
 * @file list_status.c
 * @brief Implementation of a doubly linked list for managing status objects with enter, exit, and handle callbacks.
 *
 * This file provides functions to create, destroy, add, remove, and poll status objects in a doubly linked list.
 * Each status object contains a name and function pointers for enter, exit, and handle operations.
 */
#include "list_status.h"

list_status_t* list_status_create(char *name, void (*enter)(void), void (*exit)(void), void (*handle)(void))
{
    list_status_t *status = (list_status_t *)malloc(sizeof(list_status_t));
    if (status == NULL)
    {
        return NULL;
    }
    memset(status, 0, sizeof(list_status_t));
    strncpy(status->name, name, sizeof(status->name) - 1);
    status->enter = enter;
    status->exit = exit;
    status->handle = handle;
    return status;
}

void list_status_destroy(list_status_t **status)
{
    if ((*status) != NULL)
    {
        free(*status);
    }
}

void list_status_add(list_status_t **head, list_status_t *new_status)
{
    if ((*head) == NULL || new_status == NULL)
    {
        return;
    }
    new_status->next = (*head)->next;
    new_status->prev = (*head);
    (*head)->next = new_status;
    if (new_status->next != NULL)
    {
        new_status->next->prev = new_status;
    }
}

void list_status_remove(list_status_t *status)
{
    if (status == NULL)
    {
        return;
    }
    if (status->prev != NULL)
    {
        status->prev->next = status->next;
    }
    if (status->next != NULL)
    {
        status->next->prev = status->prev;
    }

    // Preserve the name member
    char preserved_name[sizeof(status->name)];
    strncpy(preserved_name, status->name, sizeof(preserved_name) - 1);
    free(status);

    // Optionally, you can use preserved_name here if needed
    strncpy(status->name, preserved_name, sizeof(status->name) - 1);    
}

void list_poll(list_status_t *head , list_status_t **current)
{
    if (head == NULL)
    {
        return;
    }
    if (*current == NULL)
    {
        *current = head;
    }
    else
    {
        if ((*current)->handle != NULL)
        {
            (*current)->handle();
        }

        if ((*current)->exit != NULL)
        {
                (*current)->exit();
        }

        if ((*current)->next != NULL)
        {   
            if ((*current)->next->enter != NULL)
            {
                (*current)->next->enter();
            }
            (*current) = (*current)->next;
        }else
        {
                (*current) = head; // Reset to head if at the end of the list
        }
    }

}
