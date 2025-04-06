#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Two arguments needed" >&2
    exit 1
fi

if [[ ! "${1}" =~ ^[0-9]+$ ]] ; then
    echo "Arg 1 should be a number" >&2
    exit 2
fi

if [[ ! "${2}" =~ ^[0-9]+$ ]] ; then
    echo "Arg 2 should be a number" >&2
    exit 3
fi

if [[ "${1}" -gt "${2}" ]] ; then
    echo "Wrong order of args" >&2
    exit 4
fi

if [[ ! -z "$(find . -mindepth 1 -type d)" ]] ; then
    echo "Dir not empty" >&2
    exit 4
fi

mkdir a b c

while read file ; do
    rowsCount="$(cat "${file}" | wc -l )"
    
    if [[ "${rowsCount}" -lt "${1}" ]] ; then
        mv "${file}" a/
    elif [[ "${rowsCount}" -le "${2}" ]] ; then
        mv "${file}" b/
    else
        mv "${file}" c/
    fi
done< <(find . -maxdepth 1 -type f )
