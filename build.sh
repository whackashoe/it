#!/bin/sh

clang -Wall -Werror -pedantic main.c util.c issue_list_item.c issue_list.c -o it
