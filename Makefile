CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = clox

# Directories
BUILD_DIR = build
TEST_DIR = test

# Source files
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OBJS_NO_MAIN = $(filter-out main.o, $(OBJS))

# CMocka configuration
CMOCKA_CFLAGS = $(shell pkg-config --cflags cmocka 2>/dev/null || echo "")
CMOCKA_LIBS = $(shell pkg-config --libs cmocka 2>/dev/null || echo "-lcmocka")

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/unit/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/unit/%.c=$(BUILD_DIR)/test/%)

# Main target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test targets
test: test-unit test-integration

test-unit: $(TEST_BINS)
	@echo "Running unit tests..."
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo ""
	@echo "All unit tests passed!"

$(BUILD_DIR)/test/%: $(TEST_DIR)/unit/%.c $(OBJS_NO_MAIN) | $(BUILD_DIR)/test
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) $(CMOCKA_CFLAGS) -I. $^ $(CMOCKA_LIBS) -o $@

$(BUILD_DIR)/test:
	@mkdir -p $(BUILD_DIR)/test

test-integration: $(TARGET)
	@echo "Running integration tests..."
	@python3 $(TEST_DIR)/run_tests.py

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(BUILD_DIR)

.PHONY: all clean test test-unit test-integration
