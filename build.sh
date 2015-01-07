#!/bin/sh

clang -Wall -Werror -pedantic it.c util.c issue_list_item.c issue_list.c print_help.c -o it
