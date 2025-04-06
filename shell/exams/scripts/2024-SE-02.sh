#!/bin/bash

occPath="$(dirname "$0")/occ"
etc="/etc/passwd"

if [[ -n "${PASSWD}" ]] ; then
    etc="${PASSWD}"
fi

occUsernames="$(mktemp)"
etcUsernames="$(mktemp)"

awk -F: '$3 >= 1000 { print $1 }' "${etc}" | sort > "${etcUsernames}"
"${occPath}" user:list | sed -E "s/^ *- *(.*):.*$/\1/" | sort > "${occUsernames}"

while read username ; do
    "${occPath}" user:add "${username}"
done< <(comm -2 -3 "${etcUsernames}" "${occUsernames}")

while read username ; do
    if [[ "$( "${occPath}" user:info "${username}" | grep -E "^ *- *enabled:.*$" | cut -d ':' -f 2 | sed -E "s/ *(.*)/\1/")" == "false" ]] ; then
            "${occPath}" user:enable "${username}"
    fi
done< <(comm -1 -2 "${etcUsernames}" "${occUsernames}")

while read username ; do
     if [[ "$( "${occPath}" user:info "${username}" | grep -E "^ *- *enabled:.*$" | cut -d ':' -f 2 | sed -E "s/ *(.*)/\1/")" == "true" ]] ; then
          "${occPath}" user:disable "${username}"
     fi
done< <(comm -1 -3 "${etcUsernames}" "${occUsernames}")

rm "${occUsernames}"
rm "${etcUsernames}"