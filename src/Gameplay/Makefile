CC=g++
FLAGS=-Wall
LIBS=-lSDL2

BINDIR=bin
OBJDIR=obj

BIN_DEFAULT=$(BINDIR)/test_program
OBJ_DEFAULT=$(OBJDIR)/collision_system.o $(OBJDIR)/main.o $(OBJDIR)/input_system.o $(OBJDIR)/movement_system.o $(OBJDIR)/render_system.o $(OBJDIR)/world.o

test_program: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/test_program $(OBJ_DEFAULT) $(LIBS)

run: test_program
	$(BINDIR)/test_program

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

$(OBJDIR)/collision_system.o: collision_system.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/collision_system.o collision_system.cpp

$(OBJDIR)/main.o: main.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/main.o main.cpp

$(OBJDIR)/input_system.o: input_system.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/input_system.o input_system.cpp

$(OBJDIR)/movement_system.o: movement_system.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/movement_system.o movement_system.cpp
	
$(OBJDIR)/render_system.o: render_system.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/render_system.o render_system.cpp

$(OBJDIR)/world.o: world.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/world.o world.cpp

