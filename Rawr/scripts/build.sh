#!/bin/bash

# Rawr Build Script
# Advanced Cybersecurity Learning Platform

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    local missing_deps=()
    
    # Check for required tools
    if ! command_exists cmake; then
        missing_deps+=("cmake")
    fi
    
    if ! command_exists make; then
        missing_deps+=("make")
    fi
    
    if ! command_exists g++; then
        missing_deps+=("g++")
    fi
    
    if ! command_exists git; then
        missing_deps+=("git")
    fi
    
    # Report missing dependencies
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing required dependencies: ${missing_deps[*]}"
        print_status "Please install the missing dependencies and try again."
        exit 1
    fi
    
    print_success "All prerequisites satisfied"
}

# Function to create build directory
create_build_dir() {
    print_status "Creating build directory..."
    
    if [ -d "build" ]; then
        print_warning "Build directory already exists, cleaning..."
        rm -rf build
    fi
    
    mkdir -p build
    print_success "Build directory created"
}

# Function to configure build
configure_build() {
    print_status "Configuring build..."
    
    cd build
    
    # Configure with CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    
    if [ $? -eq 0 ]; then
        print_success "Build configured successfully"
    else
        print_error "Build configuration failed"
        exit 1
    fi
    
    cd ..
}

# Function to compile project
compile_project() {
    print_status "Compiling project..."
    
    cd build
    
    # Get number of CPU cores for parallel compilation
    local num_cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    print_status "Using $num_cores cores for compilation"
    
    # Compile with parallel jobs
    make -j$num_cores
    
    if [ $? -eq 0 ]; then
        print_success "Compilation completed successfully"
    else
        print_error "Compilation failed"
        exit 1
    fi
    
    cd ..
}

# Function to run tests
run_tests() {
    print_status "Running tests..."
    
    cd build
    
    # Run tests if they exist
    if [ -f "Makefile" ] && grep -q "test:" Makefile; then
        make test
        if [ $? -eq 0 ]; then
            print_success "All tests passed"
        else
            print_warning "Some tests failed"
        fi
    else
        print_warning "No tests found"
    fi
    
    cd ..
}

# Function to install project
install_project() {
    print_status "Installing project..."
    
    cd build
    
    # Install if requested
    if [ "$INSTALL" = "true" ]; then
        sudo make install
        if [ $? -eq 0 ]; then
            print_success "Installation completed"
        else
            print_error "Installation failed"
            exit 1
        fi
    fi
    
    cd ..
}

# Function to create package
create_package() {
    print_status "Creating package..."
    
    cd build
    
    # Create package if requested
    if [ "$PACKAGE" = "true" ]; then
        make package
        if [ $? -eq 0 ]; then
            print_success "Package created successfully"
        else
            print_error "Package creation failed"
            exit 1
        fi
    fi
    
    cd ..
}

# Function to show help
show_help() {
    echo "Rawr Build Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, --clean         Clean build directory before building"
    echo "  -d, --debug         Build in debug mode"
    echo "  -i, --install       Install after building"
    echo "  -p, --package       Create package after building"
    echo "  -t, --test          Run tests after building"
    echo "  -v, --verbose       Enable verbose output"
    echo ""
    echo "Examples:"
    echo "  $0                  # Build in release mode"
    echo "  $0 -d               # Build in debug mode"
    echo "  $0 -i               # Build and install"
    echo "  $0 -c -d -t         # Clean, debug build, and test"
    echo ""
}

# Parse command line arguments
CLEAN=false
DEBUG=false
INSTALL=false
PACKAGE=false
TEST=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -d|--debug)
            DEBUG=true
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -p|--package)
            PACKAGE=true
            shift
            ;;
        -t|--test)
            TEST=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Enable verbose output if requested
if [ "$VERBOSE" = "true" ]; then
    set -x
fi

# Main build process
main() {
    print_status "Starting Rawr build process..."
    
    # Check prerequisites
    check_prerequisites
    
    # Clean if requested
    if [ "$CLEAN" = "true" ]; then
        print_status "Cleaning build directory..."
        rm -rf build
    fi
    
    # Create build directory
    create_build_dir
    
    # Configure build
    configure_build
    
    # Compile project
    compile_project
    
    # Run tests if requested
    if [ "$TEST" = "true" ]; then
        run_tests
    fi
    
    # Install if requested
    if [ "$INSTALL" = "true" ]; then
        install_project
    fi
    
    # Create package if requested
    if [ "$PACKAGE" = "true" ]; then
        create_package
    fi
    
    print_success "Build process completed successfully!"
    print_status "Executable location: build/bin/rawr"
}

# Run main function
main "$@"