#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalid args count" >&2
    exit 2
fi

if [[ ! -f "${1}" ]] ; then
    echo "Arg not file" >&2
    exit 1
fi

cat "${1}" | sed -E "s/^[0-9]+ г. - //" | awk -v OFS=". " '{print NR,$0}' | sort -t '"' -k 2 | head -n 4
