#!/bin/bash

# FUNCTIONS - 
# Argument Validation - makes sure thate there are 2 arguments
argumentValidation() {
    echo "Usage: $0 <source_pgn_file> <destination_directory>"
    exit 1
}

fileExistence(){
    local file="$1"

    if [ ! -f "$file" ]; then
        echo "Error: File '$file' does not exist."
        exit 1
    fi
}

directoryExistence(){
    local dir="$1"

    if  [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        echo "Created directory '$dir'."
    fi
}

splitPGNFile(){
    local file="$1"
    local dir="$2"

    local gameNum=1
    local currGameFile

    echo "'$file' '$dir' '$gameNum'."
}

# Main Function flow
#======================
if [ "$#" -ne 2 ]; then
    argumentValidation
fi

input_file="$1"
input_dir="$2"

fileExistence "$input_file"
directoryExistence "$input_dir"

splitPGNFile "$input_file" "$input_dir"




# Sp