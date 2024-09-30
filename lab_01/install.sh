#!/bin/bash

# Script name for installation
SCRIPT_NAME="testify"

# Location where the script is stored
SOURCE_SCRIPT="generate_tests.sh" # Change this to the actual script name if different

# Default installation directory
DEFAULT_INSTALL_DIR="/usr/local/bin"

# Function to install the script
install_script() {
    if [ ! -f "$SOURCE_SCRIPT" ]; then
        echo -e "${RED}Error: Source script '$SOURCE_SCRIPT' not found.${NC}"
        exit 1
    fi

    # Check if the script already exists in the installation directory
    if [ -f "$INSTALL_DIR/$SCRIPT_NAME" ]; then
        echo -e "${YELLOW}Warning: '$SCRIPT_NAME' already exists in $INSTALL_DIR. Overwriting...${NC}"
    fi

    # Create the directory if it does not exist
    sudo mkdir -p "$INSTALL_DIR"

    # Move and make executable
    sudo mv "$SOURCE_SCRIPT" "$INSTALL_DIR/$SCRIPT_NAME"
    sudo chmod +x "$INSTALL_DIR/$SCRIPT_NAME"

    echo -e "${GREEN}Script installed successfully as '$SCRIPT_NAME'. You can now call it from anywhere.${NC}"
}

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Prompt for installation directory
echo -e "${YELLOW}Enter the installation directory (default is $DEFAULT_INSTALL_DIR):${NC}"
read INSTALL_DIR

# Use default directory if none provided
INSTALL_DIR=${INSTALL_DIR:-$DEFAULT_INSTALL_DIR}

# Run the installation function
install_script
