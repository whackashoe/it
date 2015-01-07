#ifndef ISSUE_LIST_H
#define ISSUE_LIST_H

#include "issue_list_item.h"

struct issue_list {
    struct issue_list * next;
    struct issue_list_item item;
};

int issue_list_init(struct issue_list * list);
struct issue_list * issue_list_add(struct issue_list * list, struct issue_list_item list_item);
struct issue_list_item * issue_list_search(struct issue_list * ilist, char * id);

#endif
