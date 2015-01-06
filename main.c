#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>

int endswith(char * str, char * suffix);
int print_version();
int print_help();
int init_it();
int new_issue(char * title);
int list_issues();
int main(int argc, char **argv);

//http://stackoverflow.com/a/744822/778858
int endswith(char *str, char *suffix)
{
    if (!str || !suffix) {
        return 0;
    }
    
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    
    if (lensuffix >  lenstr) {
        return 0;
    }
    
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
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
    char filepath[title_len + 128];

    /*
    char * editor = getenv("EDITOR");
    if(editor == NULL) {
        fprintf(stderr, "error: EDITOR environment variable not set");
        return 1;
    }*/

    {
        char filename[title_len];
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
        time(&now);
        char time_str[sizeof "2011-10-08T07:07:09Z"];
        strftime(time_str, sizeof time_str, "%FT%TZ", gmtime(&now));
        sprintf(filedata, "%s\n%s\n========\n\n#start writing your issue here", title, time_str); 

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
                    char * line;
                    ssize_t read;
                    size_t len = 0;
                    int lcnt = 0;
                 
                    while((read = getline(&line, &len, fp)) != 1 && lcnt++ < 2) {
                        printf("%s", line);
                    }
                }

                fclose(fp);
            }
        }
    
        closedir(d);
    }

    return 0;
}

int main(int argc, char **argv)
{
    int i; //index

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
        
        printf("try `it help`\n");
    }
    
    return 0;
}
