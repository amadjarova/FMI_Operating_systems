#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
        echo "One arg needed" >&2
        exit 1
fi

if [[ ! -f "./bakefile" ]] ; then
        echo "Bakefile not found" >&2
        exit 3
fi

function bake() {
    if grep -qE "^${1}:" "./bakefile" ; then
            local newer=false
            for dep in $(grep -E "^${1}:" "./bakefile" | awk -F ':' '{ print $2 }') ; do
                    bake "${dep}"
                            if [[ "${dep}" -nt "${1}" ]] ; then
                                newer=true
                        fi

            done

            if [[ "${newer}" == "true" ]] ;then
                    command="$( grep -E "^${1}:" "./bakefile" | awk -F ':' '{ print $3 }')"
                    eval "${command}"
            fi

    else
            if [[ ! -f "${1}" ]] ; then
                    echo "File: ${1} does not exist" >&2
                    exit 4
            fi
    fi
}

bake "${1}"