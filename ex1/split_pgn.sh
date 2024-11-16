#!/bin/bash

# Argument Validation 
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <source_pgn_file> <destination_directory>"
  exit 1
fi


# File and Directory Existence Check
file="$1"
dir="$2"

if [ ! -f "$file" ]; then
    echo "Error: File '$file' does not exist."
    exit 1
elif  [ ! -d "$dir" ]; then
    mkdir -p "$dir"
    echo "Created directory '$dir'."
fi

