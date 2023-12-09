#!/bin/bash

# verificare argumente
if [ "$#" -ne 1 ]; then
    echo "rulare gresita"
    exit 1
fi

char="$1"
counter=0

# citire linii pana la ctrl+d
while IFS= read -r line || [[ -n "$line" ]]; do
    # verificare conditii linie
    if grep -q "^[[:upper:]].*[[:upper:][:digit:][:space:],.!?]$char" <<< "$line" && ! grep -q ",\sand[[:space:]]" <<< "$line"; then
        ((counter++))
    fi
done

# afisare
echo "$counter"
