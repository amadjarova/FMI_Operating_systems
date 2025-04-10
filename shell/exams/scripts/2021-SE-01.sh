#!/bin/bash

if [[ "$(whoami)" != "grid" && "$(whoami)" != "oracle" ]] ; then
        echo "User not grid ot oracle" >&2
        exit 1
fi

if [[ -z "${ORACLE_HOME}" ]] ; then
        echo "Env not set" >&2
        exit 2
fi

if [[ ! -d "${ORACLE_HOME}" ]] ; then
        echo "Env var not dir" >&2
        exit 3
fi

adrci="${ORACLE_HOME}/bin/adrci"

if [[ ! -x "${adrci}" ]] ; then
        echo "Adrci not executable file" >&2
        exit 4
fi

out="$("${adrci}" 'exec="show homes"')"

if [[ "${out}" == "No ADR homes are set" ]] ; then
        echo "${out}"
        exit 0
fi

while read home ; do
        du -sm "/u01/app/$(whoami)/${home}"
done< <(echo "${out}" | tail -n +2 | sed 's/^ *//')