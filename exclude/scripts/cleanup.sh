#!/bin/bash

# Your netid
NETID=nxm020100
# name of the directory when the project is located
PROJECT_DIRECTORY="$HOME/CS 6378/Project 1"
# name of the configuration file
CONFIG_FILE="$PROJECT_DIRECTORY/config 1.txt"
# name of the program to be killed (provided as an input argument)
# if no input argument is provided, then all processes belonging to the user are killed
PROGRAM_FILE=$1

# initialize iteration variable
i=0
# read the configuration file
# replace any phrase starting with "#" with an empty string and then delete any empty lines
cat "$CONFIG_FILE" | sed -e "s/#.*//" | sed "/^$/d" |
{	
	# read the number of nodes
	read n
	echo "system contains" $n "nodes"
	# read the location of each node one by one
	while [[ $i -lt $n ]] 
	do
		# read a line
		read line
		# echo $line
		# extract the node identifier
		IDENTIFIER=$( echo $line | awk '{ print $1 }' )
		# extract the machine name
		HOST=$( echo $line | awk '{ print $2 }' )
		echo "cleaning up processess on machine" $HOST
		# construct the string specifying the program to be run by the ssh command
		if [[ "$PROGRAM_FILE" == "" ]]; then
     		   ARGUMENTS="killall -u $NETID"
		else
  		   ARGUMENTS="killall -u $NETID -r \"$PROGRAM_FILE\""
		fi
		echo $ARGUMENTS
		# kill the relevant processes running on the machine
		# any error message will be stored in the log files
		xterm -e "ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no $NETID@$HOST '$ARGUMENTS' 2> log.cleanup.$IDENTIFIER" &
		i=$((i+1)) 
	done
}
