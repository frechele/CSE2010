#! /bin/sh

logfile=memcheck.log

valgrind --log-file=$logfile --leak-check=full --error-limit=no --show-reachable=yes -v $1
less $logfile
rm $logfile
