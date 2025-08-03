#!/bin/bash
echo "=== Large Test Executable ==="
echo "This is a more complex executable with multiple functions"
echo "Testing various shell features..."
for i in {1..5}; do
    echo "Loop iteration $i"
done
echo "Testing command substitution: $(whoami)@$(hostname)"
echo "Testing arithmetic: $((2 + 3 * 4))"
echo "Testing file operations..."
echo "Current files:"
ls -la | head -5
echo "=== Test completed successfully! ==="
