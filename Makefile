CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -I$(SRC_DIR) -I$(UNITY_DIR)
LDFLAGS  =

# Turn on debug flags with: make DEBUG=1
ifeq ($(DEBUG),1)
CFLAGS  += -g -O0 -DDEBUG
endif

SRC_DIR  = src
BIN_DIR  = bin
OBJ_DIR  = $(BIN_DIR)/obj

TEST_DIR      = test
TEST_BIN_DIR  = $(BIN_DIR)/tests
TEST_OBJ_DIR  = $(OBJ_DIR)/tests

SRCS  = $(SRC_DIR)/main.c $(SRC_DIR)/6502.c
OBJS  = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS  = $(OBJS:.o=.d)

TARGET = $(BIN_DIR)/main

# --- Unity auto-fetch ---
UNITY_DIR      = deps/unity
UNITY_VER      = v2.6.0
UNITY_URL_BASE = https://raw.githubusercontent.com/ThrowTheSwitch/Unity/$(UNITY_VER)/src

UNITY_SRC      = $(UNITY_DIR)/unity.c
UNITY_HDR      = $(UNITY_DIR)/unity.h
UNITY_INT_HDR  = $(UNITY_DIR)/unity_internals.h
UNITY_OBJ      = $(OBJ_DIR)/unity.o
UNITY_FILES    = $(UNITY_SRC) $(UNITY_HDR) $(UNITY_INT_HDR)

# A small "if" downloader: prefer curl, then wget, then powershell (Windows)
define GET
	@set -e; \
	if command -v curl >/dev/null 2>&1; then \
		curl -sSL "$(1)" -o "$(2)"; \
	elif command -v wget >/dev/null 2>&1; then \
		wget -qO "$(2)" "$(1)"; \
	elif command -v powershell.exe >/dev/null 2>&1; then \
		powershell -NoProfile -Command "Invoke-WebRequest -Uri '$(1)' -OutFile '$(2)'"; \
	else \
		echo "No downloader (curl/wget/powershell) found"; exit 1; \
	fi
endef

.PHONY: all clean test unity update-unity run-tests

# -------- builds --------
all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile each src/*.c to bin/obj/*.o with auto-deps
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# -------- unity fetch + build --------
unity: $(UNITY_FILES)

$(UNITY_DIR)/unity.h: | $(UNITY_DIR)
	$(call GET,$(UNITY_URL_BASE)/unity.h,$@)

$(UNITY_DIR)/unity_internals.h: | $(UNITY_DIR)
	$(call GET,$(UNITY_URL_BASE)/unity_internals.h,$@)

$(UNITY_DIR)/unity.c: | $(UNITY_DIR)
	$(call GET,$(UNITY_URL_BASE)/unity.c,$@)

$(UNITY_OBJ): $(UNITY_SRC) $(UNITY_HDR) $(UNITY_INT_HDR) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(UNITY_DIR) -MMD -MP -c $< -o $@
	 
update-unity:
	@$(MAKE) -s unity

# -------- tests --------
SRCS_NO_MAIN   = $(filter-out $(SRC_DIR)/main.c,$(SRCS))
OBJS_NO_MAIN   = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(TEST_BIN_DIR)/%,$(TEST_SRCS))

test: unity $(TEST_BINS)
	@$(MAKE) -s run-tests

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_OBJ_DIR) $(UNITY_DIR)
	$(CC) $(CFLAGS) -I$(UNITY_DIR) -MMD -MP -c $< -o $@

$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(OBJS_NO_MAIN) $(UNITY_OBJ) | $(TEST_BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run-tests:
	@set -e; \
	for t in $(TEST_BINS); do \
		echo "🧪Starting test @ $$t"; "$$t"; \
	done

# -------- dirs & clean --------
$(BIN_DIR) $(OBJ_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR) $(UNITY_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(UNITY_DIR)

-include $(DEPS) $(UNITY_DEP) $(TEST_OBJS:.o=.d)

