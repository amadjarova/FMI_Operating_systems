#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "One arg needed" >&2
    exit 1
fi

if [[ ! "${1}" =~ [0-9]+ ]] ; then
    echo "Arg 1 should be a number" >&2
    exit 2
fi

if [[ "$(whoami)" != "root" ]] ; then
    echo "User not root"
    exit 3
fi

snapshot="$(mktemp)"
ps -e -o uid=,pid=,rss= > "${snapshot}"

while read user ; do
    rss="$(cat "${snapshot}" | grep -E "^ *${user} " | awk '{ sum += $3 } END { print sum }')"
    if [[ "${rss}" -gt "${1}" ]] ; then
        maxRssPid="$(cat "${snapshot}" | grep -E "^ *${user} " | awk '{ print $2,$3 }' | sort -t ' ' -k 2 -n -r | head -n 1 | awk '{ print $1 }' )"
        kill "${maxRssPid}"
        sleep 2
        kill -KILL "${maxRssPid}"
    fi
done< <(cat "${snapshot}" | awk '{print $1}' | sort | uniq)

rm "${snapshot}"