#!/bin/bash

hex_values=(
  "756e505234376848"
  "45414a3561733951"
  "377a7143574e6758"
  "354a35686e475873"
  "48336750664b394d"
)

for hex in "${hex_values[@]}"; do
    echo "$hex" \
    | sed 's/../& /g' \
    | tac -rs ' ' \
    | tr -d '\n ' \
    | xxd -r -p
done

echo