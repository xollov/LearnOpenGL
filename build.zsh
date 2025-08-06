#!/bin/zsh

# Compiler
CC="gcc"

# Flags as an array
FLAGS=(-Wall -std=c11)
LIBS=(-lglfw -lGL -lm -lassimp)

# Source files
SRCS=(main.c glad.c Graphics/shader.c common/model.c)

# Output binary
TARGET="w.exe"

# Build mode (default: release)
MODE="${1:-release}"

# Add debug flag if requested
if [[ "$MODE" == "debug" ]]; then
  FLAGS+=(-g)
fi

# Run the build
echo "🔧 Building [$MODE]..."
echo "$CC ${FLAGS[@]} -o $TARGET ${SRCS[@]} ${LIBS[@]}"
$CC "${FLAGS[@]}" -o "$TARGET" "${SRCS[@]}" "${LIBS[@]}"

# Check success
if [[ $? -eq 0 ]]; then
  echo "✅ Build succeeded: $TARGET"
else
  echo "❌ Build failed"
  exit 1
fi
