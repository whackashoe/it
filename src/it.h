#ifndef IT_H
#define IT_H

#include "issue_status.h"
#include "issue_list.h"

int print_version();
int print_help();
int init_it();
int new_issue(char * title);
int it_search_recursive_descent(char * out);
int get_it_dir(char * out, char * dir);
int get_issues_dir(char * out);
int get_archives_dir(char * out);
int get_issue_list(struct issue_list * ilist, char * issues_dir);
int list_issues(enum issue_status type);
int list_open_issues();
int list_archived_issues();
int switch_issue_directory(char * id, enum issue_status fromtype, enum issue_status totype, char * success_message);
int close_issue(char * id);
int reopen_issue(char * id);
int rename_issue(char * id, char * title);

#endif
