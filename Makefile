___HOST_OS    := $(shell uname)
ifeq ($(findstring Windows,$(___HOST_OS)),Windows)
	LIBS      := -lopengl32 -lgdi32
else ifeq ($(findstring Linux,$(___HOST_OS)),Linux)
	LIBS      := -lm -lX11 -lGL -lXrandr
else ifeq ($(findstring Darwin,$(___HOST_OS)),Darwin)
	LIBS      := -framework Cocoa -framework CoreVideo -framework OpenGL -framework IOKit
endif

# directories
SRC_DIR  	  := ./src
LIB_DIR		  := ./lib
OUT_DIR  	  := ./bin/$(TARGET)
OBJ_DIR  	  := $(OUT_DIR)/obj

LIB_INCLUDE	  := $(LIB_DIR)/

SRC	           := $(shell find $(SRC_DIR) -name '*.c') $(LIB_DIR)/lib.c main.c

# compiler arguments
FLAGS    	   := -Wall -O3 -MMD -MP
LINK_FLAS      := 
INCLUDES 	   := -I$(SRC_DIR) -I$(LIB_INCLUDE)

BINARY   	   := $(OUT_DIR)/rl_render
OBJECTS  	   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
DEPS  	 	   := $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRC))

.PHONY: clean

$(BINARY): $(OBJECTS)
	@echo Building Executable
	@mkdir -p $(OUT_DIR)
	@$(CC) -o $@ $^ $(LIBS) $(LINK_FLAS) -I$(INCLUDES)

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