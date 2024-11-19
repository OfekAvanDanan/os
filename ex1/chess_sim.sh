#!/bin/bash

# Ofek Avan Danan | avandao | 211824727

# Global Variables
#======================
moves_array=()
moves_length=0
currMove=0

# Create an empty board
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
fileExistence() {
    local file="$1"

    if [ ! -f "$file" ]; then
        echo "Error: File '$file' does not exist."
        exit 1
    fi
}

# argumentValidation - Ensures that exactly 1 argument is provided to the script
argumentValidation() {
    echo "Usage: $0 <source_pgn_file>"
    exit 1
}

displayStart() {
    local file="$1"
    local line
    local found_moves=0

    # Read the file line by line
    while IFS= read -r line; do
        if [[ "$line" =~ ^1\. ]]; then
            found_moves=1
            break
        fi
        echo "$line"
    done < "$file"

    # Ensure blank lines are preserved before moves
    if [ $found_moves -eq 0 ]; then
        echo "No moves found in the file."
        exit 1
    fi
}

getMoves() {
    local file="$1"
    local line
    local moves=""

    # Read the file line by line and collect moves
    while IFS= read -r line; do
        if [[ "$line" =~ ^1\. ]]; then
            moves+="$line "
        elif [[ -n "$moves" ]]; then
            moves+="$line "
        fi
    done < "$file"

    # Run parse_moves.py with the extracted moves
    moves_array=($(python3 parse_moves.py "$moves")) # the () turns it into an array
    moves_length=${#moves_array[@]}
}

displayMoves() {
    echo "Move $currMove/$moves_length"
}

displayBoard() {
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

# Validate the file existence
input_file="$1"
fileExistence "$input_file"

# Display start
displayStart "$input_file"

# Extract moves from the PGN file
getMoves "$input_file"

# Display moves
displayMoves

# Display the initial board
displayBoard
