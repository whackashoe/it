#include "issue_list.h"
#include <stdlib.h>

int issue_list_init(struct issue_list * list)
{
    list->prev = NULL;
    list->next = NULL;

    return 0;
}

struct issue_list * issue_list_add(struct issue_list * list, struct issue_list_item list_item)
{
    list->item = list_item;
    list->next = malloc(sizeof(struct issue_list));
    list->next->prev = list;
    list->next->next = NULL;

    return list->next;
}
