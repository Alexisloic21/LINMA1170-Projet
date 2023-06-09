CC := gcc
CFLAGS=-Wall -g -std=gnu99 -O2 #Add -Werror

GMSH_DIR := ../gmsh-sdk
BLAS_DIR := ../OpenBlas

INC_DIR := -I $(GMSH_DIR)/include -I $(BLAS_DIR)/include -I /usr/local/include
LIB_DIR := -L $(GMSH_DIR)/lib -L $(BLAS_DIR)/lib -L /usr/local/lib

CC_SDL=-lSDL `sdl-config --cflags --libs`


LDFLAGS := -Wl,-rpath,$(GMSH_DIR)/lib,-rpath,$(BLAS_DIR)/lib
LDLIBS := -lgmsh -llapack -llapacke -lm -lopenblas

SRC_DIRS = src
OUTDIR=build

SRCFILES=$(SRC_DIRS)/*.c

PROG = project




all: build


build: $(SRCFILES)
	@echo "Building project..."
	@mkdir -p $(OUTDIR)
	@$(CC) $(INC_DIR) $(CFLAGS) -o $(OUTDIR)/$(PROG) $^ $(LIB_DIR) $(LDLIBS) $(CC_SDL) $(LDFLAGS)
	@echo "Finished building project"


clean:
	@echo "Removing $(OUTDIR) directory..."
	@rm -rf $(OUTDIR)
	@echo "Finished cleaning"


.PHONY: clean
