CC=g++
FLAGS=-Wall
LIBS=-lSDL2 -lSDL2_image

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/CutThePower
OBJ_DEFAULT=$(OBJDIR)/main.o $(OBJDIR)/map.o

CutThePower: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/game $(OBJ_DEFAULT) $(LIBS)

run: CutThePower
	$(BINDIR)/CutThePower

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

debug: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -g -o $(BINDIR)/game $(OBJ_DEFAULT) $(LIBS)
	gdb -tui $(BINDIR)/CutThePower


$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/main.o $(SRCDIR)/main.cpp

$(OBJDIR)/map.o: $(SRCDIR)/map.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/map.o $(SRCDIR)/map.cpp

