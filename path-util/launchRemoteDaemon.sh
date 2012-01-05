#!/bin/bash
# Launch a remote daemon on another machine

function kill_daemon {
    if [ ! -z "$KILLED" ]; then #prevent multipe calls... 
	return
    fi
    KILLED=TRUE
    #end the remote screen session
    echo -e "\nKilling screen session $SESSION_NAME on host $REMOTE_HOST"
    ssh -q -t -o BatchMode=yes $REMOTE_HOST "screen -Rd -S $SESSION_NAME -X kill"
    echo "kill-ssh exited with code $?" 
}

function usage {
	# Display usage message on standard error
	echo "Usage:"
	echo "   $(basename $0) host remote_workingdir daemon [args]" 1>&2
	exit 1
}

if [ $# -lt 3 ]; then
	usage
fi
SESSION_NAME=session_$RANDOM
REMOTE_HOST=$1
WORKING_DIR=$2
DAEMON=$3
shift;shift;shift
DAEMON_ARGS="$@"

#trap signals so we can cleanup later
trap kill_daemon SIGHUP SIGINT SIGTERM

#launch the daemon on the remote host
echo "Launching $DAEMON from $WORKING_DIR on remote host: $REMOTE_HOST"
echo "Using screen session name $SESSION_NAME"
autossh -M 0 -- -t -o BatchMode=yes -o ServerAliveInterval=5 -o serverAliveCountMax=3 $REMOTE_HOST "screen -q -Rd -S $SESSION_NAME bash -c \"echo && cd $WORKING_DIR && ./$DAEMON $DAEMON_ARGS\" " 

#todo: use ssh directly, and keep trying to reconnect on exit code 255?
#ssh -t -o BatchMode=yes -o ServerAliveInterval=5 -o serverAliveCountMax=3 $REMOTE_HOST "screen -q -Rd -S $SESSION_NAME bash -c \"echo && cd $WORKING_DIR && ./$DAEMON $DAEMON_ARGS\" " 

echo "ssh ended in main with exit code $?"
