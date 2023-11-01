# totp cracking

cracking TOTP codes is the same as any other secret / msg pair

hashcat has the mode 18100 for HMAC-SHA1, which is what RFC-6238 uses to create tokens

the issue is that 6 digits is a very small output, with a very large amount of possible hashes - more than one token / timestamp pair is needed to produce the correct secret, and three or more should be used to ensure no collisions. Hashcat 18100 only produces a list of hashes for a pair, the matching needs to be done by yourself.
