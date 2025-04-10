#!/bin/bash

files="$(mktemp)"
words="$(mktemp)"

for arg in "${@}" ; do
        if echo "${arg}" | grep -qE "^-R.*=.*$" ; then
                echo "${arg}" | sed -E "s/-R(.*)/\1/" >>"${words}"
        else
                if [[ ! -f "${arg}" ]] ; then
                        echo "Invalid argument: ${arg}" >&2
                        exit 2
                else
                        echo "${arg}" >>"${files}"
                fi
        fi
done

pwgens="$(mktemp)"

while read file ; do
        while read word ; do
            pw="$(pwgen)"
            secondWord="$(echo "${word}" | cut -d '=' -f 2)"
            firstWord="$(echo "${word}" | cut -d '=' -f 1)"
            sed -i -E "/^#/! s/\b${firstWord}\b/${pw}${secondWord}/g" "${file}"
            echo "${pw}${secondWord}=${secondWord}" >>"${pwgens}"
        done< "${words}"
done< "${files}"

while read file ; do
        while read word ; do
                old="$(echo "${word}" | cut -d '=' -f 1)"
                new="$(echo "${word}" | cut -d '=' -f 2)"
                sed -i -E "s/\b${old}\b/${new}/g" "${file}"
        done< "${pwgens}"
done< "${files}"

rm  "${files}"
rm "${words}"
rm  "${pwgens}"
