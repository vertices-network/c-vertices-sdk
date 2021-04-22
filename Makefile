PROJ_DIR := .
OUTPUT_DIRECTORY := build

.PHONY: tests

$(OUTPUT_DIRECTORY):
	mkdir $(OUTPUT_DIRECTORY)

tests:
	make -C tests all

clean:
	make -C tests clean
	rm -rf build

all: clean $(OUTPUT_DIRECTORY)
	cd $(OUTPUT_DIRECTORY) && cmake ..
	make -C $(OUTPUT_DIRECTORY) vertices
	make -C $(OUTPUT_DIRECTORY) unix_example