#!/bin/bash
#
# Script to update the dynamic ip address, taken from www.ipchicken.com
# to the dynu.com DDNS server. Should be configured to run as a cron job
# regularly (every 15 minutes is a good start).
#
homedir=$1

dyndnshome=$homedir/.ddnsupd
cachefile=${dyndnshome}/ipcache

usernameprop=${dyndnshome}/username.prop
passwordprop=${dyndnshome}/password.prop

ipdiscoveryurl=https://www.ipchicken.com

ipregex="(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
updateip=no

now="[$(date +'%Y-%m-%d %T')]"

#
# Find our public ip address...
#
publicip=$(curl -s ${ipdiscoveryurl} | grep -E -o ${ipregex})

#
# If the cache file does not exist, create it and upload the new ip
# else if the ip address has changed update the cache and upload the new ip
if test -f "$cachefile"; then
    cachedip=$(cat ${cachefile})

    printf "%s Cached ip address: $cachedip\n" "$now"
    printf "%s Public ip from $ipdiscoveryurl is $publicip\n" "$now"

    if [ "$cachedip" != "$publicip" ]; then
        printf "$publicip\n" > $cachefile

        printf "%s Updated cached ip to $publicip\n" "$now"

        updateip=yes
    fi
else
    mkdir -p "$dyndnshome"
    printf "$publicip\n" > $cachefile

    updateip=yes
fi

if [ "$updateip" = "yes" ]; then
    username=$(cat ${usernameprop})
    password=$(cat ${passwordprop})

    updateurl="https://api.dynu.com/nic/update?username=${username}&myip=${publicip}&password=${password}"
    response=$(curl -s "$updateurl")

    printf "%s ip update service responded with: $response\n" "$now"
fi
