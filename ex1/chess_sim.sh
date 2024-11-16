#!/bin/bash

# Ofek Avan Danan | avandao | 211824727

# FUNCTIONS - 
#======================
# fileExistence - Checks if the input file exists
fileExistence(){
    local file="$1"

    if [ ! -f "$file" ]; then
        echo "Error: File '$file' does not exist."
        exit 1
    fi
}

# argumentValidation - Ensures that exactly 2 arguments are provided to the script
argumentValidation() {
    echo "Usage: $0 <source_pgn_file>"
    exit 1
}


displayStart(){
    local file="$1"

    # Read the first 11 lines of the file and display them
    head -n 11 "$file"
}

displayMoves(){
    local c=$1
    local e=
    echo "Move 2/114"
}

displayBoard(){
    local b=$1
    echo "  a b c d e f g h"
    for ((i=0; i<8; i++)); do
    echo "$((8-i)) ${board[i]} $((8-i))"
    done
    echo "  a b c d e f g h"
}


# Main script flow
#======================
# Validate the number of arguments
if [ "$#" -ne 1 ]; then
    argumentValidation
fi

# Validate the file exisitence 
input_file="$1"
fileExistence "$input_file"

# create an empty board
board=(
  "r n b q k b n r"  
  "p p p p p p p p"  
  ". . . . . . . ." 
  ". . . . . . . ."  
  ". . . . . . . ."  
  ". . . . . . . ."  
  "P P P P P P P P" 
  "R N B Q K B N R" 
)

# display start
displayStart "$input_file"

moves=0

displayBoard "$board" 



