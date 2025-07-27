#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_node {
    struct list_node *prev, *next;
} list_node;

typedef struct list {
    int len;
    list_node *first, *last;
} list;

#define container_of(ptr, sample, member) (typeof(sample))((char *)(ptr) - offsetof(typeof(*sample), member))

#define list_for_each(iter, list) for(list_node *iter = (list)->first; iter != NULL; iter = iter->next)

#define list_for_each_safe(iter, list)                                                                 \
    for(list_node *iter = (list)->first, *iter##_tmp = iter == NULL ? NULL : iter->next; iter != NULL; \
            iter = iter##_tmp, iter##_tmp = iter == NULL ? NULL : iter->next)

#define list_for_each_reverse(iter, list) for(list_node *iter = (list)->last; iter != NULL; iter = iter->prev)

#define list_for_each_reverse_safe(iter, list)                                                        \
    for(list_node *iter = (list)->last, *iter##_tmp = iter == NULL ? NULL : iter->prev; iter != NULL; \
            iter = iter##_tmp, iter##_tmp = iter == NULL ? NULL : iter->prev)

void
list_push_back(list *list, list_node *node);

void
list_push_front(list *list, list_node *node);

void
list_insert_before(list *list, list_node *before, list_node *node);

void
list_insert_after(list *list, list_node *after, list_node *node);

void
list_remove(list *list, list_node *node);

#endif

#ifdef LIST_IMPLEMENTATION

void
list_push_back(list *list, list_node *node) {
    if(list->first == NULL) {
        list->first = list->last = node;
        node->prev = node->next = NULL;
    } else {
        list->last->next = node;
        node->prev = list->last;
        node->next = NULL;
        list->last = node;
    }

    list->len++;
}

void
list_push_front(list *list, list_node *node) {
    if(list->first == NULL) {
        list->first = list->last = node;
        node->prev = node->next = NULL;
    } else {
        list->first->prev = node;
        node->next = list->first;
        node->prev = NULL;
        list->first = node;
    }

    list->len++;
}

void
list_insert_before(list *list, list_node *before, list_node *node) {
    if(before == list->first) {
        list_push_front(list, node);
    } else {
        node->prev = before->prev;
        node->next = before;
        before->prev->next = node;
        before->prev = node;
        list->len++;
    }
}

void
list_insert_after(list *list, list_node *after, list_node *node) {
    if(after == list->last) {
        list_push_back(list, node);
    } else {
        node->next = after->next;
        node->prev = after;
        after->next->prev = node;
        after->next = node;
        list->len++;
    }
}

void
list_remove(list *list, list_node *node) {
    if(node->prev == NULL) {
        list->first = node->next;
    } else {
        node->prev->next = node->next;
    }

    if(node->next == NULL) {
        list->last = node->prev;
    } else {
        node->next->prev = node->prev;
    }

    list->len--;

    node->prev = node->next = NULL;
}

#endif
