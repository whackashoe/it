it: the issue tracker
===

it is a simple commandline issue tracker written in c. it stores all issues in flat files so you can easily edit them/view them/share however you want. it lends itself quite well to being integrated into a project under textual version control.

it is designed to be fast to enter in issues while working on more important stuff. it was made because I hate having to switch mental contexts to file a bug report or a simple todo later sort of thing. designed for lazy people you could say?! (or just designed for me.)

how to use
===

```
usage: it [--version] [--help] <command> [<args>]

it commands:
   init               Initialize issue tracker
   new TITLE          Add a new issue
   list [archive]     List all issues
   close ID           Close an issue
   reopen ID          Reopen an issue
   edit ID            Edit an issue
   help COMMAND       Shows help page describing command

See 'it help <command>' to read about a specific subcommand
```


installation
===

it just requires that you run something that is posix, has a reasonable c compiler, make, and vim. only tested on one specific linux. 

```
git clone https://github.com/whackashoe/it.git
cd it/build
make
sudo make install
```

you're done.


contributing
===

there are a ton of bugs, security problems, optimization problems, bullshit code, etc etc. if you feel like improving any of this, please feel free to send a pull request
