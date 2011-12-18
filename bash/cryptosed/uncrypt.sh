#!/bin/bash

filename="tex2html-test.tar.7z"
st="no"

while [[ $st = "no" ]]; do
      	for ((a=0;a<10;a++)); do 
	      	for ((b=0;b<10;b++)); do 
      			for ((c=0;c<10;c++)); do 
			      	for ((d=0;d<10;d++)); do
				  7za e $filename -p$a$b$c$d -y &>/dev/null
				  if [ $? != 0 ]; then
					st="no"
					echo "Trying password: $a$b$c$d"
			          else
				        st="yes"
					echo "$a$b$c$d"
				        exit 0
			          fi
				done
			done
		done
	done
done
