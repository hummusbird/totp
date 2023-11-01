# totp cracking

cracking TOTP codes is the same as any other secret / msg pair

hashcat has the mode 18100 for HMAC-SHA1, which is what RFC-6238 uses to create tokens

the issue is that 6 digits isn't enough to work with alone, there are a very large amount of possible hashes - more than one token / timestamp pair is needed to produce the correct secret, and three or more should be used to ensure no collisions. Hashcat 18100 only produces a list of hashes for a pair, the matching needs to be done by yourself.

## generate_pairs

this is a simple c# program based on the c# totp implementation, designed to generate totp:timestamp pairs for cracking. you can input the secret, the initial unix timestamp, and the amount needed.


## example
 
### (so i remember)

this example is faster than generating hashes for all tokens then checking for dupes.
if collisions occur, just run a third token through the results of 2.pot

```
// generate first potfile (single hash pair used, 10 digits in this example)
hashcat -m18100 -a3 -o 1.pot hash_1.hash "?d?d?d?d?d?d?d?d?d?d" --keep-guessing -n1 -T1 --force

// take secrets, base32 decode, wrap newline, save to new file
cut -d: -f3 1.pot | base32 -d | fold -w 10 > 1_wordlist.txt

// run second hash using wordlist
hashcat -m18100 -a0 -o 2.pot hash_2.hash 1_wordlist.txt --keep-guessing -n1 -T1 --force

// decode b32 to secret (if hash exists)
cut -d: -f3 2.pot | base32 -d >> secret.txt
```

this method is simpler but must more computationally expensive:

```
// generate potfile (any amount of pairs in the hash file, 10 digits in this example)
hashcat -m18100 -a3 -o totp.pot hashes.hash"?d?d?d?d?d?d?d?d?d?d" --keep-guessing -n1 -T1 --force

// find matches in pot
cut -d: -f3 totp.pot | sort | uniq -c | sort -nr | head -n1 
```
