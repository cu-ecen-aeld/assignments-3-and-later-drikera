#!/bin/sh

# finder.sh
# arg1: path to a directory on the filesystem [filesdir]
# arg2: text string which will be searched within these files [searchstr]
# res:  Prints message "The number of files are X and the number of matching files are Y"
# 	X is the number of files in the directory and all subdirectories
#	Y is the number of matching lines found in respective files, matching seachstr
# Example Invocation:
# 	finder.sh /tmp/aesd/assignment1 linux

# Check number of arguments passed
if [ $# -eq 2 ] # right number of args provided
then
   if [ -d $1 ]
   then
      nfiles=$(ls -r $1 | grep "" -c)
      #nfiles=$(find $1 | grep "./" -c)
      nlines=$(grep -r "$2" $1/* | grep "" -c)
      echo "The number of files are ${nfiles} and the number of matching lines are ${nlines}."
   else # directory path provided is not directory or directory does not exist
      echo "The directory provided is either:"
      echo "   1) nonexistent"
      echo "   2) not a directory"
      exit 1
   fi
	
else # wrong number of args provided
   echo "Incorrect number of arguments provided."
   echo "Usage: ./finder.sh Arg1 Arg2"
   echo "   Arg1: Path to a directory on the filesystem"
   echo "   Arg2: Text string which will be searched within the files in the directory"
   exit 1
fi

