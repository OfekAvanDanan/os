#!/bin/bash

# Ofek Avan Danan | avandao | 211824727

# Global Variables
#======================
moves_array=()
moves_length=0
currMove=0

# Create an empty board
emptyBoard=(
  "r n b q k b n r"
  "p p p p p p p p"
  ". . . . . . . ."
  ". . . . . . . ."
  ". . . . . . . ."
  ". . . . . . . ."
  "P P P P P P P P"
  "R N B Q K B N R"
)

boardHistorty=()


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

getAllMoves() {
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
    moves_array=($(python3 parse_moves.py "$moves"))
    moves_length=${#moves_array[@]}
}

# Generate all board states
getAllMoves() {
    # Add the initial board to the history
    board=("${emptyBoard[@]}")
    boardHistory+=("$(printf "%s\n" "${board[@]}")")

    for ((i=0; i<moves_length; i++)); do
        m="${moves_array[i]}"

        # Extract the source and destination coordinates
        x1=$(( $(printf '%d' "'${m:0:1}") - 97 )) 
        y1=$((8 - ${m:1:1}))                     
        x2=$(( $(printf '%d' "'${m:2:1}") - 97 ))
        y2=$((8 - ${m:3:1}))

        # Move the piece
        piece=$(echo "${board[y1]}" | cut -d' ' -f$((x1 + 1)))

        # Update the source position
        read -a lineArray1 <<< "${board[y1]}"
        lineArray1[$x1]='.'
        board[y1]="${lineArray1[*]}"

        # Update the destination position
        read -a lineArray2 <<< "${board[y2]}"
        lineArray2[$x2]="$piece"
        board[y2]="${lineArray2[*]}"

        # Append the updated board to the history
        boardHistory+=("$(printf "%s\n" "${board[@]}")")
    done
    
}


update_cell() {
    local row=$1
    local col=$2
    local value=$3
    local row_content="${board[row]}"
    board[row]=$(echo "$row_content" | awk -v col=$col -v val="$value" '{
        for (i = 1; i <= NF; i++) {
            if (i == col) $i = val;
        }
        print $0;
    }')
}


displayBoard() {
    local stage="$1"
    echo "Move $stage/$moves_length"

    # Extract the board state for the given stage
    local board_state=("${boardHistorty[@]:$((stage * 8)):8}") # Extract 8 lines per stage

    echo "  a b c d e f g h"
    for ((i=0; i<8; i++)); do
        echo "$((8-i)) ${board_state[i]} $((8-i))"
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
getAllMoves

# Display the initial board
displayBoard 0