CC := gcc
# CFLAGS = -std=c17 -Wall -Wextra -pedantic
LDFLAGS := -lm
EXEC := codec
LIB := libqtc.so

CFLAGS = -Wall -pedantic -finline-functions -funroll-loops -Wextra -g

ADVANCED_CFLAGS = 

# book of GCC flags
# https://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc.pdf

# usefull GCC flags
# https://linux.die.net/man/1/gcc

# if user wants to check with `-ansi` norm
ifeq ($(ANSI), 1)
	CFLAGS += -ansi
	else 
	CFLAGS += -std=c17
endif

ifeq ($(ADV), 1)
	ADVANCED_CFLAGS += -Wfatal-errors -Werror -Wfloat-equal -Wstrict-prototypes -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wundef -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
endif

LIB_MTRACK = 
ifeq ($(MTRACK), 1)
	LIB_MTRACK += $(shell echo $$MTRACK)
endif
# for internal tests of memory leaks
# to take them
# -ansi -std=standard -fgnu89-inline -aux-info filename -fno-asm -fno-builtin -fno-builtin-function -fhosted -ffreestanding -fopenmp -fms-extensions -trigraphs -no-integrated-cpp -traditional -traditional-cpp -fallow-single-precision -fcond-mismatch -flax-vector-conversions -fsigned-bitfields -fsigned-char -funsigned-bitfields -funsigned-char

#  -Werror -Wfatal-errors 
OPT := -O2

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib
APP_DIR := app
DOCS_DIR := docs

QTC_DIR := QTC
PGM_DIR := PGM


# for creating shared object we don't need main.o
OBJ = $(OBJ_DIR)/option.o $(OBJ_DIR)/qtree.o $(OBJ_DIR)/main.o
OBJ += $(OBJ_DIR)/pixmap.o $(OBJ_DIR)/bits_operations.o $(OBJ_DIR)/grid.o


# DATE = $(shell date +%Y-%m-%d__%H-%M)
NAME = L3.2024.ProgC-MUNAITPASOV.BENEVISTE


QTC_LIB = -lqtc -I$(CURDIR)/$(INC_DIR) -L$(CURDIR)/$(LIB_DIR) -Wl,-rpath,$(CURDIR)/$(LIB_DIR)


all: $(LIB_DIR)/$(LIB) $(BIN_DIR)/$(EXEC)


# for creating the executable file
$(BIN_DIR)/$(EXEC): $(OBJ_DIR)/main.o $(LIB_DIR)/$(LIB)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(CFLAGS) $(QTC_LIB) -O2

# compiling main file
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INC_DIR) $(ADVANCED_CFLAGS) $(LDFLAGS)

# for library:
# remove the object file after creating the library
$(LIB_DIR)/$(LIB): $(OBJ_DIR)/option.o $(OBJ_DIR)/qtree.o $(OBJ_DIR)/pixmap.o $(OBJ_DIR)/bits_operations.o $(OBJ_DIR)/grid.o
	@mkdir -p $(@D)
	$(CC) $^ -shared -o $@ $(LDFLAGS)
#	$(CC) -shared -o $@ $^ $(CFLAGS) -I$(INC_DIR) $(ADVANCED_CFLAGS)

# create the library
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -fPIC $< -o $@ $(CFLAGS) -I$(INC_DIR) $(ADVANCED_CFLAGS) $(LDFLAGS)

# # for executable 
# $(BIN_DIR)/$(EXEC): $(OBJ)
# 	@mkdir -p $(@D)
# 	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# # create object files from source files
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	@mkdir -p $(@D)
# 	$(CC) -c -o $@ $< $(CFLAGS) -I$(INC_DIR) $(ADVANCED_CFLAGS) $(LDFLAGS)


help:
	@echo "make lib/libqtc.so: to compile the library"
	@echo "make all: to compile the project"
	@echo "make clean: to clean the project"
	@echo "make docs: to generate the documentation"
	@echo "make ansi: to compile the project with the -ansi flag"


# set JAVADOC_BANNER to YES, GENERATE_HTML to YES, GENERATE_LATEX to NO
docs: $(DOCS_DIR)/index.html

$(DOCS_DIR)/index.html: Doxyfile $(SRC_DIR)/*.c $(INC_DIR)/*.h
	@doxygen Doxyfile
	@echo "Documentation generated in $(DOCS_DIR)/index.html"

encode:
	make -B
	./$(BIN_DIR)/$(EXEC) -c -i $(PGM_DIR)/boat.512.pgm -o $(QTC_DIR)/boat.512.qtc -a 1.4

decode:
	make -B
	./$(BIN_DIR)/$(EXEC) -u -i $(QTC_DIR)/boat.512.qtc -o $(PGM_DIR)/my_boat.512.pgm

ansi:
	make -B ANSI=1 ADV=1

valgrind:
	make -B ANSI=1 ADV=1
	valgrind --leak-check=full -s ./$(BIN_DIR)/$(EXEC) -c -i $(PGM_DIR)/boat.512.pgm -g -a 1.8

memory:
	make -B ADV=1 MTRACK=1
	valgrind --leak-check=full -s ./$(BIN_DIR)/$(EXEC) -c -i $(PGM_DIR)/boat.512.pgm -g -a 1.0



# compress the project with the `zip` command
zip: clean
	@echo "cleaning all PGM and QTC directories"
	@echo "archivage sources, includes > $(NAME).zip"
	@mkdir -p $(NAME)
	@cp -R README.md makefile $(SRC_DIR) $(INC_DIR) $(OBJ_DIR) $(BIN_DIR) $(DOCS_DIR) $(QTC_DIR) $(PGM_DIR) Doxyfile $(NAME)
	@zip -r $(NAME).zip $(NAME)
	@$(RM)r $(NAME)

# compress the project with the `tar` command
tar: clean
	@echo "cleaning all PGM and QTC directories"
	@echo "archivage sources, includes > $(NAME).tar.gz"
	@mkdir -p $(NAME)
	@cp -R README.md makefile $(SRC_DIR) $(INC_DIR) $(OBJ_DIR) $(BIN_DIR) $(DOCS_DIR) $(QTC_DIR) $(PGM_DIR) Doxyfile $(NAME)
	@tar -zcf $(NAME).tar.gz $(NAME)
#	@tar -czvf $(NAME).tgz $(NAME)
	@$(RM)r $(NAME)


clean:
	$(RM) $(OBJ_DIR)/*.o
	$(RM) $(BIN_DIR)/$(EXEC)
	$(RM) $(QTC_DIR)/*.qtc
	$(RM) $(PGM_DIR)/*.pgm
	@$(RM)r $(NAME) $(NAME)*.zip $(NAME)*.tar.gz

mrproper: clean
	$(RM)r $(BIN_DIR)
	$(RM)r $(OBJ_DIR)



.PHONY: mrproper all run

# end of makefile
