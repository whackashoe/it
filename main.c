#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"
#include "issue_list_item.h"
#include "issue_list.h"
#include "constants.h"

int print_version();
int print_help();
int init_it();
int new_issue(char * title);
int list_issues();
int close_issue(char * id);
int reopen_issue(char * id);
int rename_issue(char * id, char * title);
int main(int argc, char **argv);



int print_version()
{
    printf("0.1\n");
    return 0;
}

int print_help()
{
    printf("help\n");
    return 0;
}

int init_it()
{
    struct stat st = {0};

    if(stat(".it", &st) == -1) {
        char cwd[1024];

        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            mkdir(".it", 0775);
            mkdir(".it/issues", 0775);
            mkdir(".it/archive", 0775);
            printf("Initialized it in %s/.it\n", cwd);
        } else {
            fprintf(stderr, "error: current working directory not accessible\n");
            return 1;
        }
    } else {
        fprintf(stderr, "error: it has already been installed\n");
        return 1;
    }
    
    return 0;
}

int new_issue(char * title)
{
    char * editor = "vim";
    int title_len = strlen(title);
    char * filepath = malloc(sizeof(char) * (title_len + 128));

    /*
    char * editor = getenv("EDITOR");
    if(editor == NULL) {
        fprintf(stderr, "error: EDITOR environment variable not set");
        return 1;
    }*/

    {
        char * filename = malloc(sizeof(char) * title_len);
        char c;
        int i, j;
        for(i=0, j=0; i<title_len; ++i) {
            c = tolower(title[i]);

            if(c >= 'a' && c <= 'z') {
                filename[j] = c;
                ++j;
            } else if(c == ' ' || c == '_' || c == '-') {
                filename[j] = '-';
                ++j;
            }
        }

        filename[j] = '\0';
        sprintf(filepath, ".it/issues/%s.it", filename);
    }
    
    {
        FILE * fp;
        char filedata[1024];
        time_t now;
        char * time_str = malloc(sizeof(char) * ISO8601_LEN);

        time(&now);

        strftime(time_str, ISO8601_LEN, "%FT%TZ", gmtime(&now));
        sprintf(filedata, "%s\n%s\n========\n\nstart writing your issue here\n", title, time_str); 

        fp = fopen(filepath, "a+");
        if(fp == NULL) {
            fprintf(stderr, "error: %s unable to be created/opened\n", filepath);
            fclose(fp);
            return 1;
        }

        fwrite(filedata, sizeof(char), strlen(filedata), fp);
        fclose(fp);
    }
    
    {
        char cmd[512];
        sprintf(cmd, "%s %s", editor, filepath);
        system(cmd);
    }

    return 0;
}

int list_issues()
{
    DIR * d = opendir(".it/issues");
    struct dirent *dir;

    if (d) {
        struct issue_list ilist;
        struct issue_list * ilist_it;
        issue_list_init(&ilist);
        ilist_it = &ilist;

        while ((dir = readdir(d)) != NULL) {
            if(endswith(dir->d_name, ".it")) {
                char filepath[256];
                FILE * fp;
                sprintf(filepath, ".it/issues/%s", dir->d_name);
                fp = fopen(filepath, "r");

                if(fp == NULL) {
                    fprintf(stderr, "error: %s could not be read\n", filepath);
                    fclose(fp);
                    continue;
                }

                {
                    struct issue_list_item list_item;
                    ssize_t read;
                    size_t len = 0;

                    issue_list_item_init(&list_item);

                    read = getline(&list_item.title, &len, fp);
                    read = getline(&list_item.datetime, &len, fp);

                    issue_list_item_gen_id(&list_item);

                    ilist_it = issue_list_add(ilist_it, list_item);
                }

                fclose(fp);
            }
        }

        {
            struct issue_list * ilist_it = &ilist;
            do {
                issue_list_item_print(&ilist_it->item);
                ilist_it = ilist_it->next;
            } while(ilist_it->next != NULL);
        }
    
        closedir(d);
    }

    return 0;
}

int close_issue(char * id)
{
    return 0;
}

int reopen_issue(char * id)
{
    return 0;
}

int rename_issue(char * id, char * title)
{
    return 0;
}

int main(int argc, char **argv)
{
    int i;

    if(argc > 1) {
        i = 1;

        if(strcmp("version", argv[i]) == 0) {
            return print_version();
        }
        if(strcmp("help", argv[i]) == 0) {
            return print_help();
        }
        if(strcmp("init", argv[i]) == 0) {
            return init_it();
        }
        if(strcmp("new", argv[i]) == 0) {
            if(argc < 2) {
                fprintf(stderr, "error: new requires a title\n");
                return 1;
            }
            
            return new_issue(argv[i+1]);
        }
        if(strcmp("list", argv[i]) == 0) {
            return list_issues();
        }
        if(strcmp("close", argv[i]) == 0) {
            if(argc < 2) {
                fprintf(stderr, "error: close requires an id\n");
                return 1;
            }
            return close_issue(argv[i+1]);
        }
        if(strcmp("reopen", argv[i]) == 0) {
            if(argc < 2) {
                fprintf(stderr, "error: reopen requires an id\n");
                return 1;
            }
            return reopen_issue(argv[i+1]);
        }
        if(strcmp("rename", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "error: rename requires an id and a title\n");
                return 1;
            }
            return rename_issue(argv[i+1], argv[i+2]);
        }
        
        printf("try `it help`\n");
    }
    
    return 0;
}
