#include <stdlib.h>
#include <stdio.h>
#include "issue_list_item.h"
#include "util.h"
#include "constants.h"


int issue_list_item_init(struct issue_list_item * list_item)
{
    list_item->id = malloc(sizeof(char) * 6);
    list_item->title = malloc(sizeof(char) * 256);
    list_item->datetime = malloc(sizeof(char) * ISO8601_LEN);
    list_item->filepath = malloc(sizeof(char) * 1024);
    list_item->filename = malloc(sizeof(char) * 1024);
    return 0;
}

int issue_list_item_gen_id(struct issue_list_item * list_item)
{
    strncpy(list_item->id, djb2tos(djb2(list_item->title)), 6);
    return 0;
}

int issue_list_item_destroy(struct issue_list_item * list_item)
{
    free(list_item->id);
    free(list_item->title);
    free(list_item->datetime);
    free(list_item->filepath);
    free(list_item->filename);
    return 0;
}
