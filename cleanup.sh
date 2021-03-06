#!/bin/bash


# Change this to your netid
netid=wxz190003


#
# Directory where the config file is located on your local system
CONFIGLOCAL=config.txt

n=0

cat $CONFIGLOCAL | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    echo $i
    while [[ $n -lt $i ]]
    do
    	read line
        host=$( echo $line | awk '{ print $2 }' )

        echo $host
        # gnome-terminal -e "ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no $netid@$host killall -u $netid" &
        # sleep 1
        osascript -e 'tell app "Terminal"
        do script "ssh '$netid@$host' rm -rf '$host' && ssh '$netid@$host' rm -rf 'config-$n.dat' && ssh '$netid@$host' killall -u '$netid'; '$SHELL'"
        end tell'

        n=$(( n + 1 ))
    done
   
)