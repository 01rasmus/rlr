# directories
SRC_DIR  	  := ./src
LIB_DIR		  := ./lib
OUT_DIR  	  := ./bin/$(TARGET)
OBJ_DIR  	  := $(OUT_DIR)/obj

LIB_INCLUDE	  := $(LIB_DIR)/

SRC	           := $(shell find $(SRC_DIR) -name '*.c')

# compiler arguments
FLAGS    	   := -Wall -O3 -MMD -MP
LINK_FLAS      := 
INCLUDES 	   := -I$(SRC_DIR) -I$(LIB_INCLUDE)
LIBS     	   := -lm -lX11 -lGL -lXrandr

BINARY   	   := $(OUT_DIR)/rl_render
OBJECTS  	   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
DEPS  	 	   := $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRC))

.PHONY: clean

$(BINARY): $(OBJECTS)
	@echo Building Executable
	@mkdir -p $(OUT_DIR)
	@$(CC) -o $@ $^ $(LIBS) $(LINK_FLAS) main.c -I$(INCLUDES)

$(OBJ_DIR)/%.o: %.c
	@echo Compiling $<
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(INCLUDES) $(FLAGS)

build: $(BINARY)

run: build
	./$(BINARY)

clean:
	rm -rf bin

-include $(DEPS)