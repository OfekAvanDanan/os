#!/bin/bash

# Ofek Avan Danan | avandao | 211824727


# FUNCTIONS - 
#======================

# argumentValidation - Ensures that exactly 2 arguments are provided to the script
argumentValidation() {
    echo "Usage: $0 <source_pgn_file> <destination_directory>"
    exit 1
}

# fileExistence - Checks if the input file exists
fileExistence(){
    local file="$1"

    if [ ! -f "$file" ]; then
        echo "Error: File '$file' does not exist."
        exit 1
    fi
}

# directoryExistence - Checks if the output directory exists; creates it if necessary
directoryExistence(){
    local dir="$1"

    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        echo "Created directory '$dir'."
    fi
}

# splitPGNFile - Splits the PGN file into individual games and saves them as separate files
splitPGNFile(){
    # Local variables
    local file="$1"
    local dir="$2"

    local gameNum=1
    local currGameContent=""

    # Read the input file line by line
    while IFS= read -r line || [ -n "$line" ]; do
        # Check for the start of a new game by looking for "[Event " at the beginning of the line
        if [[ "$(echo "$line" | head -c 7)" == "[Event " ]]; then
            # If a new game starts, save the previous game's content (if it exists)
            if [ -n "$currGameContent" ]; then
                createAPGNFile "$dir" "$gameNum" "$currGameContent"
                gameNum=$((gameNum + 1))
            fi
            # Start a new game's content
            currGameContent="$line"
        else
            # Append the current line to the ongoing game's content
            currGameContent+=$'\n'"$line"
        fi
    done < "$file"

    # Save the last game's content (if it exists)
    if [ -n "$currGameContent" ]; then
        createAPGNFile "$dir" "$gameNum" "$currGameContent"
    fi

    echo "All games have been split and saved to '$dir'."
}

# createAPGNFile - Creates a PGN file for the current game
createAPGNFile(){
    local dir="$1"
    local gameNum="$2"
    local gameContent="$3"
    local currGameFile

    currGameFile="$dir/game${gameNum}.pgn"
    echo "$gameContent" > "$currGameFile"
    echo "Saved game $gameNum to '$currGameFile'."
}

# Main script flow
#======================

# Validate the number of arguments
if [ "$#" -ne 2 ]; then
    argumentValidation
fi

# Input arguments
input_file="$1"
input_dir="$2"

# Ensure the input file and output directory are valid
fileExistence "$input_file"
directoryExistence "$input_dir"

# Process the PGN file to split it into individual games
splitPGNFile "$input_file" "$input_dir"