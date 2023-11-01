#!/bin/bash
cut -d: -f3 1.pot > decoded_wordlist.b32

while IFS= read -r line
do
	echo "$line" | base32 -d >> decoded_wordlist.txt
	echo "" >> decoded_wordlist.txt
done < decoded_wordlist.b32

rm decoded_wordlist.b32