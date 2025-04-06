#!/bin/bash

if [[ "${#}" -ne 1 && "${#}" -ne 2 ]] ; then
        echo "One or two args needed" >&2
        exit 1
fi

if [[ ! -d "${1}" ]] ; then
        echo "Arg 1 should be a directory" >&2
        exit 2
fi

out="/dev/stdout"

if [[ "${#}" -eq 2 ]] ; then
    if [[ ! -f "${2}" ]] ; then
        echo "Arg 2 should be a file" >&2
        exit 3
    else
        out="${2}"
    fi
fi

broken=0

while read file; do
        if [[ -e "${file}" ]] ; then
                echo "$(basename "${file}") -> $(readlink "${file}")" >> "${out}"
        else
                broken=$((broken + 1))
        fi
done < <(find "${1}" -type l 2>/dev/null)

echo "Broken symlinks: ${broken}" >> "${out}"

