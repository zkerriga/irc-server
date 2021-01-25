#!/bin/zsh

echo "\033[32m[+] Copy script file!\033[0m"

src="ngircd.conf"
tmp="$src.tmp"
dest="$HOME/.brew/etc/ngircd.conf"

iam=$(whoami)
str=CHANGE_TO_USERNAME

sed "s/$str/$iam/g" $src > $tmp
cp "$dest" "$dest.bk"
cp $tmp "$dest"
rm -f $tmp

echo "\033[32m[+] Success!\033[0m"
