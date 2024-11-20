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

boardHistory=()  # Corrected variable name

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

    echo "Metadata from PGN file:"

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

# Generate all board states
getAllMoves() {
    # Initialize the board
    board=("${emptyBoard[@]}")

    # Add the initial board to the history
    boardHistory+=("${board[@]}")

    for ((i=0; i<moves_length; i++)); do
        m="${moves_array[i]}"

        # Extract the source and destination coordinates
        x1=$(( $(printf '%d' "'${m:0:1}") - 97 )) 
        y1=$((8 - ${m:1:1}))                     
        x2=$(( $(printf '%d' "'${m:2:1}") - 97 ))
        y2=$((8 - ${m:3:1}))

        # Move the piece
        piece=$(echo "${board[y1]}" | cut -d' ' -f$((x1 + 1)))

        # Special Moves
        # Castling
        if [[ "$piece" = "K" && "$x1" -eq 4 && "$y1" -eq 7 ]]; then
            if [[ "$x2" -eq 2 ]]; then
                read -a lineArray <<< "${board[7]}"
                lineArray[0]='.'
                lineArray[3]='R'
                board[7]="${lineArray[*]}"
            elif [[ "$x2" -eq 6 ]]; then
                read -a lineArray <<< "${board[7]}"
                lineArray[7]='.'
                lineArray[5]='R'
                board[7]="${lineArray[*]}"
            fi
        elif [[ "$piece" = "k" && "$x1" -eq 4 && "$y1" -eq 0 ]]; then
            if [[ "$x2" -eq 2 ]]; then
                read -a lineArray <<< "${board[0]}"
                lineArray[0]='.'
                lineArray[3]='r'
                board[0]="${lineArray[*]}"
            elif [[ "$x2" -eq 6 ]]; then
                read -a lineArray <<< "${board[0]}"
                lineArray[7]='.'
                lineArray[5]='r'
                board[0]="${lineArray[*]}"
            fi
        fi

        # Promotion
        if [[ "$piece" = "P" && "$y2" -eq 0 ]]; then
            lineArray2[$x2]='Q'
        elif [[ "$piece" = "p" && "$y2" -eq 7 ]]; then
            lineArray2[$x2]='q'
        fi

        # En Passant
        if [[ "$piece" = "P" && "$y1" -eq 3 && "$y2" -eq 2 && "${board[3]}" =~ "p" && "$x2" -ne "$x1" ]]; then
            read -a lineArray <<< "${board[3]}"
            lineArray[$x2]='.'
            board[3]="${lineArray[*]}"
        elif [[ "$piece" = "p" && "$y1" -eq 4 && "$y2" -eq 5 && "${board[4]}" =~ "P" && "$x2" -ne "$x1" ]]; then
            read -a lineArray <<< "${board[4]}"
            lineArray[$x2]='.'
            board[4]="${lineArray[*]}"
        fi

        # Update the source position
        read -a lineArray1 <<< "${board[y1]}"
        lineArray1[$x1]='.'
        board[y1]="${lineArray1[*]}"

        # Update the destination position
        read -a lineArray2 <<< "${board[y2]}"
        lineArray2[$x2]="$piece"
        board[y2]="${lineArray2[*]}"

        # Append the updated board to the history
        boardHistory+=("${board[@]}")
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
    local board_state=("${boardHistory[@]:$((stage * 8)):8}") # Extract 8 lines per stage

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

# Generate all board states
getAllMoves

# Interactive navigation of board states
ans=""
displayBoard $currMove


while true; do
    # Display the current board state
    echo -n "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit:"
    read -r ans
    echo #Skip line

    case $ans in
    "d")
        # 'd' to move forward
        if [ $currMove -lt $moves_length ]; then
            ((currMove++))
            displayBoard $currMove
        else
            echo "No more moves available."
        fi
        ;;
    "a")
        if [ $currMove -gt 0 ]; then
            ((currMove--))
        fi
        displayBoard $currMove
        ;;
    "w")
        currMove=0 
        displayBoard $currMove
        ;;
    "s")
        currMove=$moves_length  # Jump to the end
        displayBoard $currMove
        ;;
    "q")
        echo "Exiting."
        break
        ;;
    *)
        echo "Invalid key pressed: $ans" 
        ;;
    esac
done
echo "End of game."
