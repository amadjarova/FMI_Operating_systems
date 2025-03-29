#!/bin/bash

read -p "Enter dir name: " dirName

if [[ ! -d "${dirName}" ]] ; then
    echo "Dir not found" >&2
    exit 1
fi

files="$(find ${dirName} -type f 2>&1 | wc -l)"
dirs="$(find ${dirName} -mindepth 1 -type d 2>&1 | wc -l)"

echo "Files: ${files} Dirs: ${dirs}"
