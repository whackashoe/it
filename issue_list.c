#include "issue_list.h"
#include <stdlib.h>

int issue_list_init(struct issue_list * list)
{
    list->next = NULL;
    return 0;
}

struct issue_list * issue_list_add(struct issue_list * list, struct issue_list_item list_item)
{
    list->item = list_item;
    list->next = malloc(sizeof(struct issue_list));
    list->next->next = NULL;

    return list->next;
}

struct issue_list_item * issue_list_search(struct issue_list * ilist, char * id)
{
    struct issue_list * ilist_it = ilist;

    do {
        if(strcmp(ilist_it->item.id, id) == 0) {
            return &ilist_it->item;
        }

        ilist_it = ilist_it->next;
    } while(ilist_it->next != NULL);

    return NULL;
}
