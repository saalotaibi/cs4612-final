# Makefile for serial genome statistics program

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = genome_stats

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): genome_stats.c
	$(CC) $(CFLAGS) -o $(TARGET) genome_stats.c

# Clean build files
clean:
	rm -f $(TARGET)

# Test with sample data if available
test: $(TARGET)
	@echo "Testing with sample FASTA file..."
	@if [ -f "../data/sample.fasta" ]; then \
		./$(TARGET) ../data/sample.fasta; \
	else \
		echo "No test file found at ../data/sample.fasta"; \
	fi

# Phony targets
.PHONY: all clean test

