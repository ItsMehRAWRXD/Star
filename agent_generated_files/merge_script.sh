#!/bin/bash

# Agent Generated Files Merge Script
# This script helps merge files from the agent_generated_files directory into the main project

set -e

echo "=== Agent Generated Files Merge Script ==="
echo "This script will help you merge files from agent_generated_files into your main git repository"
echo ""

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "Error: Not in a git repository. Please run this script from the root of your git repo."
    exit 1
fi

# Create backup of current state
echo "Creating backup of current state..."
git stash push -m "Backup before merging agent-generated files" || true

# Function to copy files with confirmation
copy_files() {
    local source_dir="$1"
    local target_dir="$2"
    local file_pattern="$3"
    
    if [ -d "$source_dir" ]; then
        echo "Found files in $source_dir:"
        find "$source_dir" -name "$file_pattern" -type f | while read -r file; do
            echo "  - $file"
        done
        
        read -p "Copy these files to $target_dir? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            mkdir -p "$target_dir"
            cp -r "$source_dir"/* "$target_dir/" 2>/dev/null || true
            echo "Files copied to $target_dir"
        fi
    fi
}

# Check for files in each subdirectory
echo "Checking for files to merge..."

# Source files
copy_files "agent_generated_files/src" "src" "*.py"
copy_files "agent_generated_files/src" "src" "*.cpp"
copy_files "agent_generated_files/src" "src" "*.h"

# Test files
copy_files "agent_generated_files/tests" "tests" "*"

# Documentation
copy_files "agent_generated_files/docs" "docs" "*"

# Configuration
copy_files "agent_generated_files/config" "config" "*"

echo ""
echo "=== Merge Summary ==="
echo "Files have been copied to their respective directories."
echo ""
echo "Next steps:"
echo "1. Review the copied files"
echo "2. Run 'git status' to see what files are new/modified"
echo "3. Add files to git: 'git add .'"
echo "4. Commit changes: 'git commit -m \"Add agent-generated files\"'"
echo "5. Push to remote: 'git push'"
echo ""
echo "To restore previous state: 'git stash pop'"