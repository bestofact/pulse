#!/bin/bash

# Pulse Engine - Project Generation Script
# This script configures the CMake build system and generates the build files
# It handles different build types and generators intelligently

set -e  # Exit on any error

# Get the script's directory (the scripts folder)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Navigate to project root (one level up from scripts)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Default values
BUILD_TYPE="Release"
GENERATOR=""
BUILD_DIR=""  # Will be auto-determined based on BUILD_TYPE if not specified
CLEAN=false
AUTO_BUILD_DIR=true  # Flag to track if we're using auto-naming

# Color codes for pretty output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored messages
print_info() {
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

# Display usage information
show_usage() {
    cat << EOF
Pulse Engine - Project Generation Script

Usage: $0 [OPTIONS]

Options:
    -t, --type TYPE         Build type: Debug, Report, Release, Publish
                           (default: Release)
    -g, --generator GEN     CMake generator to use (e.g., "Unix Makefiles", "Ninja")
                           If not specified, CMake will choose the default
    -b, --build-dir DIR     Build directory path
                           If not specified, uses build-{type} (e.g., build-debug)
    -c, --clean            Clean the build directory before generating
    -h, --help             Show this help message

Examples:
    $0                                  # Generate Release in build-release/
    $0 -t Debug                        # Generate Debug in build-debug/
    $0 -t Release -g Ninja             # Generate Release with Ninja in build-release/
    $0 -c -t Debug                     # Clean and generate Debug in build-debug/
    $0 -b custom-build -t Release      # Use custom directory name

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -g|--generator)
            GENERATOR="$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            AUTO_BUILD_DIR=false  # User specified custom dir
            shift 2
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Validate build type
case $BUILD_TYPE in
    Debug|Report|Release|Publish)
        ;;
    *)
        print_error "Invalid build type: $BUILD_TYPE"
        print_info "Valid types: Debug, Report, Release, Publish"
        exit 1
        ;;
esac

# Auto-determine build directory if not specified by user
if [ "$AUTO_BUILD_DIR" = true ]; then
    # Convert build type to lowercase for directory name
    BUILD_TYPE_LOWER=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')
    BUILD_DIR="$PROJECT_ROOT/build/build-${BUILD_TYPE_LOWER}"
    print_info "Using automatic build directory for $BUILD_TYPE configuration"
fi

# Print configuration
echo ""
print_info "================================================"
print_info "Pulse Engine - Configuration"
print_info "================================================"
print_info "Project root:  $PROJECT_ROOT"
print_info "Build type:    $BUILD_TYPE"
print_info "Build dir:     $BUILD_DIR"
if [ -n "$GENERATOR" ]; then
    print_info "Generator:     $GENERATOR"
else
    print_info "Generator:     (default)"
fi
print_info "================================================"
echo ""

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    if [ -d "$BUILD_DIR" ]; then
        print_warning "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    else
        print_info "Build directory doesn't exist, nothing to clean"
    fi
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    print_info "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR"

# Prepare CMake command
CMAKE_CMD="cmake"
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

# Add generator if specified
if [ -n "$GENERATOR" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -G \"$GENERATOR\""
fi

# Add source directory
CMAKE_ARGS="$CMAKE_ARGS \"$PROJECT_ROOT\""

# Execute CMake
print_info "Running CMake..."
print_info "Command: $CMAKE_CMD $CMAKE_ARGS"
echo ""

# Use eval to properly handle quoted arguments
if eval $CMAKE_CMD $CMAKE_ARGS; then
    echo ""
    print_success "================================================"
    print_success "Project generation completed successfully!"
    print_success "================================================"
    print_info "Build directory: $BUILD_DIR"
    print_info ""
    print_info "To build the project, run:"
    print_info "  ./scripts/build.sh -b $BUILD_DIR"
    print_info ""
    print_info "Or manually:"
    print_info "  cd $BUILD_DIR"
    print_info "  cmake --build ."
    print_info ""
    print_info "Tip: You can have multiple configurations built simultaneously:"
    print_info "  ./scripts/generate.sh -t Debug"
    print_info "  ./scripts/generate.sh -t Release"
    print_info "  ./scripts/build.sh -t Debug    # Builds Debug"
    print_info "  ./scripts/build.sh -t Release  # Builds Release"
    print_success "================================================"
    exit 0
else
    echo ""
    print_error "================================================"
    print_error "CMake configuration failed!"
    print_error "================================================"
    print_error "Please check the error messages above"
    print_error "================================================"
    exit 1
fi
