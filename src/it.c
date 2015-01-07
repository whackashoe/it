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
#include "print_help.h"
#include "issue_status.h"
#include "it.h"


int get_it_dir(char * out, char * dir)
{
    char it_dir[1024];
    char indir[1024];
    if(it_search_recursive_descent(it_dir) != 0) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

    sprintf(indir, "%s/%s", it_dir, dir);
    strncpy(out, indir, sizeof(indir));

    return 0;
}

int get_issues_dir(char * out)
{
    return get_it_dir(out, "issues");
}

int get_archives_dir(char * out)
{
    return get_it_dir(out, "archive");
}

int it_search_recursive_descent(char * out)
{
    char cwd[1024];
    char it_dir[1024 + (128 * 3)];
    int depth = 0;
    int maxdepth = 128;
    
    if(getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "error: current working directory unable to be resolved\n");
        return 1;
    }

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
        return 1;
    }

    sprintf(out, "%s", it_dir);
    return 0;
}

int print_version()
{
    printf("it version %s\n", VERSION);
    return 0;
}

int print_help()
{
    printf(
        "usage: it [--version] [--help] <command> [<args>]\n"
        "\n"
        "it commands:\n"
        "   init               Initialize issue tracker\n"
        "   new TITLE          Add a new issue\n"
        "   list [archive]     List all issues\n"
        "   close ID           Close an issue\n"
        "   reopen ID          Reopen an issue\n"
        /*"   rename ID TITLE    Rename an issue\n"*/
        "   edit ID            Edit an issue\n"
        "   help COMMAND       Shows help page describing command\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int init_it()
{
    char it_dir[1024];
    struct stat st = {0};

    if(it_search_recursive_descent(it_dir) == 0) {
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
    char editor[128];
    int title_len;
    char * filepath;
    char it_dir[1024];
    
    get_editor(editor);
    title_len = strlen(title);
    filepath = malloc(sizeof(char) * (title_len + 128));

    if(it_search_recursive_descent(it_dir) != 0) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

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
        sprintf(filepath, "%s/issues/%s.md", it_dir, filename);
    }
    
    {
        FILE * fp;
        char filedata[1024];
        time_t now;
        char * time_str = malloc(sizeof(char) * DATEFORMAT_LEN);

        time(&now);

        strftime(time_str, DATEFORMAT_LEN, "%Y-%m-%d %H:%M", gmtime(&now));
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
        
        if(system(cmd) != 0) {
            fprintf(stderr, "error: '%s' had non zero return code\n", cmd);
            return 1;
        }
    }

    return 0;
}

int get_issue_list(struct issue_list * ilist, char * issues_dir)
{
    DIR * d;
    struct dirent *dir;
    struct issue_list * ilist_it;

    d = opendir(issues_dir);
    
    issue_list_init(ilist);
    
    if (d) {    
        ilist_it = ilist;

        while ((dir = readdir(d)) != NULL) {
            if(endswith(dir->d_name, ".md")) {
                char filepath[1024];
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
                    strncpy(list_item.filepath, filepath, sizeof(filepath));
                    strncpy(list_item.filename, dir->d_name, sizeof(dir->d_name));

                    read = getline(&list_item.title, &len, fp);
                    if(read == -1) {
                        fprintf(stderr, "error: reading first line of %s", list_item.filepath);
                    }
                    read = getline(&list_item.datetime, &len, fp);
                    if(read == -1) {
                        fprintf(stderr, "error: reading second line of %s", list_item.filepath);
                    }

                    strtok(list_item.title, "\n");
                    strtok(list_item.datetime, "\n");


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


int list_issues(enum issue_status type)
{
    char issues_dir[1024];
    struct issue_list ilist;
    int result;

    if(type == IT_OPEN) {
        result = get_issues_dir(issues_dir);
    } else if(type == IT_ARCHIVE) {
        result = get_archives_dir(issues_dir);
    } else {
        result = 1;
    }

    if(result != 0) {
        return 1;
    }

    if(get_issue_list(&ilist, issues_dir) != 0) {
        fprintf(stderr, "error: issue list could not be retrieved\n");
        return 1;
    }

    {
        struct issue_list * ilist_it = &ilist;
        do {
            printf("\t%s\n[%s]\t%s\n", ilist_it->item.title, ilist_it->item.id, ilist_it->item.datetime);
            ilist_it = ilist_it->next;
        } while(ilist_it->next != NULL);
    }

    return 0;
}

int list_open_issues()
{
    return list_issues(IT_OPEN);
}

int list_archived_issues()
{
    return list_issues(IT_ARCHIVE);
}

int switch_issue_directory(char * id, enum issue_status fromtype, enum issue_status totype, char * success_message)
{
    char it_dir[1024];
    char from_dir[1024], to_dir[1024];
    struct issue_list ilist;
    int fresult, tresult;

    if(fromtype == IT_OPEN) {
        fresult = get_issues_dir(from_dir);
    } else if(fromtype == IT_ARCHIVE) {
        fresult = get_archives_dir(from_dir);
    } else {
        fresult = 1;
    }

    if(fresult != 0) {
        return 1;
    }

    if(totype == IT_OPEN) {
        tresult = get_issues_dir(to_dir);
    } else if(totype == IT_ARCHIVE) {
        tresult = get_archives_dir(to_dir);
    } else {
        tresult = 1;
    }

    if(tresult != 0) {
        return 1;
    }

    if(it_search_recursive_descent(it_dir) != 0) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

    if(get_issue_list(&ilist, from_dir) != 0) {
        fprintf(stderr, "error: issue list could not be retrieved\n");
        return 1;
    }


    {
        struct issue_list_item * item = issue_list_search(&ilist, id);
        char newname[1024];

        if(item == NULL) {
            fprintf(stderr, "error: item id not found\n");
            return 1;
        }

        sprintf(newname, "%s/%s", to_dir, item->filename);
        
        if(rename(item->filepath, newname) != 0) {
            fprintf(stderr, "error: rename from %s to %s failed\n", item->filepath, newname);
            return 1;
        }

        printf(success_message, item->title);
    }

    return 0;
}

int close_issue(char * id)
{
    return switch_issue_directory(id, IT_OPEN, IT_ARCHIVE, "%s archived\n");
}

int reopen_issue(char * id)
{
    return switch_issue_directory(id, IT_ARCHIVE, IT_OPEN, "%s reopened\n");
}

int rename_issue(char * id, char * title)
{
    return 0;
}

int edit_issue(char * id)
{
    char it_dir[1024];
    char issues_dir[1024];
    struct issue_list ilist;

    if(get_issues_dir(issues_dir) != 0) {
        return 1;
    }

    if(it_search_recursive_descent(it_dir) != 0) {
        fprintf(stderr, "error: it doesn't exist in this or any parent directories\n");
        return 1;
    }

    if(get_issue_list(&ilist, issues_dir) != 0) {
        fprintf(stderr, "error: issue list could not be retrieved\n");
        return 1;
    }


    {
        struct issue_list_item * item = issue_list_search(&ilist, id);

        if(item == NULL) {
            fprintf(stderr, "error: item id not found\n");
            return 1;
        }

        {
            char editor[128];
            char cmd[512];

            get_editor(editor);
            sprintf(cmd, "%s %s", editor, item->filepath);
            
            if(system(cmd) != 0) {
                fprintf(stderr, "error: '%s' had non zero return code\n", cmd);
                return 1;
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    int i;

    if(argc > 1) {
        i = 1;

        if(strcmp("--version", argv[i]) == 0) {
            return print_version();
        }
        
        else if(strcmp("--help", argv[i]) == 0) {
            return print_help();
        }
        
        else if(strcmp("help", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "it: 'help' requires a command. See 'it --help'.\n");
                return 1;
            } 
            else if(strcmp("init", argv[i+1]) == 0) {
                return print_help_init();
            }
            else if(strcmp("new", argv[i+1]) == 0) {
                return print_help_new();
            } 
            else if(strcmp("list", argv[i+1]) == 0) {
                return print_help_list();
            }
            else if(strcmp("close", argv[i+1]) == 0) {
                return print_help_close();
            }
            else if(strcmp("reopen", argv[i+1]) == 0) {
                return print_help_reopen();
            }
            else if(strcmp("rename", argv[i+1]) == 0) {
                return print_help_rename();
            }
            else if(strcmp("edit", argv[i+1]) == 0) {
                return print_help_edit();
            } 
            else {
                fprintf(stderr, "it: 'help %s' is not an it command. See 'it --help'.\n", argv[i+1]);
                return 1;
            }
        }
        
        else if(strcmp("init", argv[i]) == 0) {
            return init_it();
        }
        
        else if(strcmp("new", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "it: 'new' requires a title\n");
                return 1;
            }
            
            return new_issue(argv[i+1]);
        }
        
        else if(strcmp("list", argv[i]) == 0) {
            if(argc < 3) {
                return list_open_issues();
            } else {
                /*
                if(strcmp("new", argv[i+1]) == 0) {
                }
                if(strcmp("old", argv[i+1]) == 0) {
                }
                */
                if(strcmp("archive", argv[i+1]) == 0) {
                    return list_archived_issues();
                }
            }
        }
        
        else if(strcmp("close", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "it: 'close' requires an id\n");
                return 1;
            }
            return close_issue(argv[i+1]);
        }
        
        else if(strcmp("reopen", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "it: 'reopen' requires an id\n");
                return 1;
            }
            return reopen_issue(argv[i+1]);
        }

        else if(strcmp("rename", argv[i]) == 0) {
            if(argc < 4) {
                fprintf(stderr, "it: 'rename' requires an id and a title\n");
                return 1;
            }
            return rename_issue(argv[i+1], argv[i+2]);
        }

        else if(strcmp("edit", argv[i]) == 0) {
            if(argc < 3) {
                fprintf(stderr, "it: 'edit' requires an id\n");
                return 1;
            }
            return edit_issue(argv[i+1]);
        }
        
        fprintf(stderr, "it: '%s' is not an it command. See 'it --help'.\n", argv[i]);
    }
    
    return print_help();
}
