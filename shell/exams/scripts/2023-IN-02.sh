#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Wrong args count" >&2
    exit 1
fi

if [[ ! -d "${1}" ]] ; then
    echo "Arg 1 should be a directory" >&2
    exit 2
fi

hash_sums="$(mktemp -d)"

while read inode file ; do
    sum="$(md5sum "${file}" | cut -d ' ' -f 1)"
    echo "${inode} ${file}" >> "${hash_sums}/${sum}"
done< <(find "${1}" -type f -printf '%i %p\n')

inodes="$(mktemp)"

while read group_files ; do
    cut -d ' ' -f 1 "${group_files}" | sort | uniq > "${inodes}"

    inodes_count="$(cat "${inodes}" | wc -l)"
    files_count="$(cat "${group_files}" | wc -l)"

    if [[ "${inodes_count}" -eq "${files_count}" ]] ; then
        tail -n +2 "${group_files}" | cut -d ' ' -f 2-
    else
        while read inode ; do
            count="$(grep -E "^${inode} " "${group_files}" | wc -l)"

            if [[ "${count}" -eq 1 ]] ; then
                grep -E "^${inode} " "${group_files}" | cut -d ' ' -f 2-
            else
                grep -E "^${inode} " "${group_files}" | head -n 1 | cut -d ' ' -f 2-
            fi
        done < "${inodes}"
    fi
done< <(find "${hash_sums}" -type f)

rm -rf "${hash_sums}"
rm "${inodes}"