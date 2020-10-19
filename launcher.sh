#!/bin/bash

# Change this to your netid
netid=wxz190003

# Directory where the config file is located on your local system
CONFIGLOCAL=/Users/t.t./Desktop/study/AOS/Project1/config.txt
# CONFIGLOCAL=$HOME/AOS/Project1/config.txt


n=0

cat $CONFIGLOCAL | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    echo $i
    while [[ $n -lt $i ]]
    do
    	read line
    	p=$( echo $line | awk '{ print $1 }' )
        host=$( echo $line | awk '{ print $2 }' )
	
	    # gnome-terminal -e "ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no $netid@$host java -cp $BINDIR $PROG $p; exec bash" &

        # g++ compile and link the $HOME project1.cpp file to the name $host, then run the project
        osascript -e 'tell app "Terminal"
        do script "ssh '$netid@$host' g++ project1.cpp -o '$host' -pthread -std=c++11 && ssh '$netid@$host' ./'$host'; '$SHELL'"
        end tell'

        # osascript -e 'tell app "Terminal"
        # do script "cd '$PROJDIR' && ./a.out; '$SHELL'"
        # end tell'
        # g++  main.cpp -o main.out -pthread -std=c++11

        n=$(( n + 1 ))
    done
)
