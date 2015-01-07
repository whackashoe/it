#ifndef ISSUE_LIST_ITEM_H
#define ISSUE_LIST_ITEM_H

#include <string.h>
#include "util.h"

struct issue_list_item {
    char * id;
    char * title;
    char * datetime;
};

int issue_list_item_init(struct issue_list_item * list_item);
int issue_list_item_gen_id(struct issue_list_item * list_item);
int issue_list_item_print(struct issue_list_item * list_item);
int issue_list_item_destroy(struct issue_list_item * list_item);

#endif
