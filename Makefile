PROJ_DIR := .
OUTPUT_DIRECTORY := build

VERTICES_SRC_FILES := \
        $(PROJ_DIR)/src/vertices_errors.c \
        $(PROJ_DIR)/src/vertices.c \
        $(PROJ_DIR)/src/account.c \

VERTICES_INC_FOLDERS := \
	-I$(PROJ_DIR)/inc \
	-I$(PROJ_DIR)/mdk \
	-I$(PROJ_DIR)/config \

UNIX_SRC_FILES := \
        $(PROJ_DIR)/src/tcp/tcp_unix.c \
        $(PROJ_DIR)/examples/unix/main.c \

CFLAGS := -Wall -g

SRCS := $(VERTICES_SRC_FILES) $(UNIX_SRC_FILES)

clean:
	rm -rf build/

$(OUTPUT_DIRECTORY):
	mkdir $(OUTPUT_DIRECTORY)

# Building unix example
unix: $(OUTPUT_DIRECTORY)
	$(CC) $(CFLAGS) $(VERTICES_INC_FOLDERS) -o $(OUTPUT_DIRECTORY)/$@ $(SRCS)
