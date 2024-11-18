#!/bin/bash

# Ofek Avan Danan | avandao | 211824727

# Global Varibles
#======================
moves_array=[]
moves_length=0
currMove=0

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
    awk '!/^1\./ {print; next} {exit}' "$file"
}

getMoves(){
    local file="$1"
    moves=$(awk '/^1\./ {flag=1} flag' "$file")

    # Run parse_moves.py with the extracted moves
    moves_array=($(python3 parse_moves.py "$moves")) # the () turns it into an arrey
    moves_length=${#moves_array[@]}
}

displayMoves(){
    echo "Move $currMove/$moves_length"
}

displayBoard(){
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


# display start
displayStart "$input_file"

# Extract moves from the PGN file
getMoves "$input_file"
echo "$moves_array"
displayMoves
# Display the initial board
displayBoard

