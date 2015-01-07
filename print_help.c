#include <stdio.h>
#include "print_help.h"

int print_help_init()
{
    printf(
        "NAME\n"
        "\tit init - Initializes issue tracker in a directory\n"
        "\n"
        "SYNOPSIS\n"
        "\tit init\n"
        "\n"
        "DESCRIPTION:\n"
        "\tThis command initializes an it tracker in the directory it is run in. "
        "Any child directories can then use this issue tracker. It is best to run "
        "this in the base folder of your project.\n"
        "\n"
        "EXAMPLE:\n"
        "\tit init\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int print_help_new()
{
    printf(
        "NAME\n"
        "\tit new - Adds a new issue to the tracker\n"
        "\n"
        "SYNOPSIS\n"
        "\tit new TITLE\n"
        "\n"
        "DESCRIPTION:\n"
        "\tJust enter in a title and your $EDITOR will open allowing you to edit your "
        "issue to add any relevant information. "
        "\n"
        "EXAMPLE:\n"
        "\tit new \"segfault encountered only when I make a phone call in the bathroom\"\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int print_help_list()
{
    printf(
        "NAME\n"
        "\tit list - Lists open or archived issues\n"
        "\n"
        "SYNOPSIS\n"
        "\tit list [archive]\n"
        "\n"
        "DESCRIPTION:\n"
        "\tUse 'it list' to see all open issues. They will be displayed in this format:\n"
        "\n"
        "\t#ID\tCREATION_TIME\n"
        "\t\tTITLE\n"
        "\n"
        "\tYou may use 'it list archive' to show archived issues as well."
        "\n"
        "EXAMPLE:\n"
        "\tit list\n"
        "\tit list archive\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int print_help_close()
{
    printf(
        "NAME\n"
        "\tit close - Closes an issue, moving it to the archive\n"
        "\n"
        "SYNOPSIS\n"
        "\tit close ID\n"
        "\n"
        "DESCRIPTION:\n"
        "\tMove an issue to the archive, removing it from the open issues list. "
        "You must provide the 5 character id displayed during 'it list'\n"
        "\n"
        "EXAMPLE:\n"
        "\tit close adhaw\n"
        "\tit close mweis\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int print_help_reopen()
{
    printf(
        "NAME\n"
        "\tit reopen - Reopens an issue, moving it to the open issues\n"
        "\n"
        "SYNOPSIS\n"
        "\tit reopen ID\n"
        "\n"
        "DESCRIPTION:\n"
        "\tMove an issue to the open issues list, removing it from the archive. "
        "You must provide the 5 character id displayed during 'it list'\n"
        "\n"
        "EXAMPLE:\n"
        "\tit reopen adhaw\n"
        "\tit reopen mweis\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}

int print_help_rename()
{
    printf(
        "NAME\n"
        "\tit rename - Rename an issue\n"
        "\n"
        "SYNOPSIS\n"
        "\tit rename ID TITLE\n"
        "\n"
        "DESCRIPTION:\n"
        "\tRenames an issue. Useful for those cases when you learn more about the "
        "problem and it is different than you had thought initially. "
        "You must provide the 5 character id displayed during 'it list' and also a new title.\n"
        "\n"
        "EXAMPLE:\n"
        "\tit rename adhaw \"Everytime you click the main button the cpu goes to 100%%\"\n"
        "\tit rename mweis \"Eating while programming is an issue for the keyboard\"\n"
        "\n"
        "See 'it help <command>' to read about a specific subcommand\n"
    );
    return 0;
}
