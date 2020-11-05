TARGET_EXEC ?= a.out

BUILD_DIR ?= ./build
SRC_DIR ?= ./src
INC_DIR ?= ./inc

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LDFLAGS:= -pthread

FLAGS ?= -I$(INC_DIR) -Wall -Wextra -g 

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c $(BUILD_DIR)/src
	$(CC) $(FLAGS) -c $< -o $@


$(BUILD_DIR)/src:
	mkdir -p $(BUILD_DIR)/src

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
