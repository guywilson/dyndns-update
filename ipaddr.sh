#!/bin/bash
#
# Script to update the dynamic ip address, taken from www.ipchicken.com
# to the dynu.com DDNS server.
#
homedir=$1
username=$2
password=$3

cachelocation=$homedir/.ipaddr
cachefile=${cachelocation}/ipcache

#username=guywilson
#password=e04b0ee27040705f4123b90aaa3fbfc081554269561180341ae51f9f9fb6c0d3

ipdiscoveryurl=https://www.ipchicken.com

ipregex="(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"

printf "Cached file: $cachefile\n"

updateip=no

#
# Find our public ip address...
#
publicip=$(curl -s ${ipdiscoveryurl} | grep -E -o ${ipregex})
#publicip="192.168.0.65"

#
# If the cache file does not exist, create it and upload the new ip
# else if the ip address has changed update the cache and upload the new ip
if test -f "$cachefile"; then
    cachedip=$(cat ${cachefile})

    printf "Cached ip address: $cachedip\n"
    printf "Public ip from $ipdiscoveryurl is $publicip\n"

    if [ "$cachedip" != "$publicip" ]; then
        printf "$publicip\n" > $cachefile

        printf "Updated cached ip to $publicip\n"

        updateip=yes
    fi
else
    mkdir -p "$cachelocation"
    printf "$publicip\n" > $cachefile

    updateip=yes
fi

if [ "$updateip" = "yes" ]; then
    updateurl="https://api.dynu.com/nic/update?username=${username}&myip=${publicip}&password=${password}"
    echo "$updateurl"

    response=$(curl -s "$updateurl")

    printf "ip update service responded with: $response\n"
fi