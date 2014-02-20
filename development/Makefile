CC=g++
FLAGS=-Wall
LIBS=-lSDL2

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/input_demo
OBJ_DEFAULT=$(OBJDIR)/keyinputsystem.o $(OBJDIR)/main.o $(OBJDIR)/mouseinputsystem.o $(OBJDIR)/world.o

input_demo: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/input_demo $(OBJ_DEFAULT) $(LIBS)

run: input_demo
	$(BINDIR)/input_demo

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

debug: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -g -o $(BINDIR)/input_demo $(OBJ_DEFAULT) $(LIBS)
	gdb -tui $(BINDIR)/input_demo


$(OBJDIR)/keyinputsystem.o: $(SRCDIR)/keyinputsystem.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/keyinputsystem.o $(SRCDIR)/keyinputsystem.cpp

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/main.o $(SRCDIR)/main.cpp

$(OBJDIR)/mouseinputsystem.o: $(SRCDIR)/mouseinputsystem.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/mouseinputsystem.o $(SRCDIR)/mouseinputsystem.cpp

$(OBJDIR)/world.o: $(SRCDIR)/world.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/world.o $(SRCDIR)/world.cpp

