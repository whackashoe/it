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

int it_search_recursive_descent(char * out)
{
    char cwd[1024];
    size_t cwdlen;
    char it_dir[1024 + (128 * 3)];
    int depth = 0;
    int maxdepth = 128;
    
    if(getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "error: current working directory unable to be resolved\n");
        return 1;
    }
    cwdlen = strlen(cwd);

    {
        struct stat st = {0};
        sprintf(it_dir, "%s/.it", cwd);

        while((stat(it_dir, &st) == -1) && depth < maxdepth) {
            char append[128*3];
            int i;

            for(i=0; i<depth; ++i) {
                append[i*3 + 0] = '/';
                append[i*3 + 1] = '.';
                append[i*3 + 2] = '.';
            }
            append[depth * 3] = '\0';
            
            sprintf(it_dir, "%s%s/.it", cwd, append);
            ++depth;
        }
    }

    if(depth == maxdepth) {
        sprintf(out, "");        
        return 1;
    }

    sprintf(out, "%s", it_dir);
    return 0;
}

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
    char it_dir[1024];
    struct stat st = {0};

    if(!it_search_recursive_descent(it_dir)) {
        fprintf(stderr, "error: it has already been installed in a parent directory: %s\n", it_dir);
        return 1;
    }

    if(stat(it_dir, &st) == -1) {
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
    char it_dir[1024];
    if(it_search_recursive_descent(it_dir)) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

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
        sprintf(filepath, "%s/issues/%s.it", it_dir, filename);
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

int get_issue_list(struct issue_list * ilist)
{
    char it_dir[1024];
    char issues_dir[1024];
    DIR * d;
    struct dirent *dir;
    struct issue_list * ilist_it;

    if(it_search_recursive_descent(it_dir)) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

    sprintf(issues_dir, "%s/issues", it_dir);
    d = opendir(issues_dir);
    
    issue_list_init(ilist);
    
    if (d) {    
        ilist_it = ilist;

        while ((dir = readdir(d)) != NULL) {
            if(endswith(dir->d_name, ".it")) {
                char filepath[256];
                FILE * fp;
                sprintf(filepath, "%s/%s", issues_dir, dir->d_name);
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
    }

    closedir(d);

    return 0;
}

int list_issues()
{
    struct issue_list ilist;
    if(get_issue_list(&ilist)) {
        fprintf(stderr, "error: issue list could not be retrieved\n");
        return 1;
    }

    {
        struct issue_list * ilist_it = &ilist;
        do {
            issue_list_item_print(&ilist_it->item);
            ilist_it = ilist_it->next;
        } while(ilist_it->next != NULL);
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
            if(argc < 3) {
                fprintf(stderr, "error: new requires a title\n");
                return 1;
            }
            
            return new_issue(argv[i+1]);
        }
        if(strcmp("list", argv[i]) == 0) {
            return list_issues();
        }
        if(strcmp("close", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "error: close requires an id\n");
                return 1;
            }
            return close_issue(argv[i+1]);
        }
        if(strcmp("reopen", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "error: reopen requires an id\n");
                return 1;
            }
            return reopen_issue(argv[i+1]);
        }
        if(strcmp("rename", argv[i]) == 0) {
            if(argc < 4) {
                fprintf(stderr, "error: rename requires an id and a title\n");
                return 1;
            }
            return rename_issue(argv[i+1], argv[i+2]);
        }
        
        printf("try `it help`\n");
    }
    
    return 0;
}
