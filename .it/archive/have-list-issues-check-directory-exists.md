have list issues check directory exists
===
2015-01-06T18:28:56Z

and lets do recursive descent if it doesnt.

ie..

/home/code/something
/home/code/something/.it
/home/code/something/pictures

cd /home/code/something/pictures
it list
    check /home/code/something/pictures/.it (not exists)
     ...  /home/code/something/.it (exists)

