CC=g++
FLAGS=-Wall -std=c++0x -O0 -g -fpermissive 
LIBS=-lrt -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_mixer -lpthread -lSDL2_ttf

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/CutThePower
OBJ_DEFAULT=$(OBJDIR)/Gameplay/collision_system.o $(OBJDIR)/Gameplay/powerups.o $(OBJDIR)/Gameplay/movement_system.o $(OBJDIR)/Graphics/render_system.o $(OBJDIR)/Graphics/animation_system.o $(OBJDIR)/Graphics/map.o $(OBJDIR)/Graphics/fog_of_war_system.o $(OBJDIR)/Input/keyinputsystem.o $(OBJDIR)/Input/mouseinputsystem.o $(OBJDIR)/Input/menu.o $(OBJDIR)/main.o $(OBJDIR)/sound.o $(OBJDIR)/world.o $(OBJDIR)/triggered.o $(OBJDIR)/Graphics/text.o $(OBJDIR)/Network/GameplayCommunication.o $(OBJDIR)/Network/ServerCommunication.o $(OBJDIR)/Network/PipeUtils.o $(OBJDIR)/Network/NetworkRouter.o $(OBJDIR)/Network/ClientUpdateSystem.o $(OBJDIR)/Network/SendSystem.o $(OBJDIR)/Network/packet_min_utils.o $(OBJDIR)/Input/chat.o $(OBJDIR)/Graphics/cutscene_system.o

CutThePower: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/CutThePower $(OBJ_DEFAULT) $(LIBS)

run: CutThePower
	$(BINDIR)/CutThePower

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

debug: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -g -o $(BINDIR)/CutThePower $(OBJ_DEFAULT) $(LIBS)
	gdb $(BINDIR)/CutThePower

$(OBJDIR)/Network/packet_min_utils.o: $(SRCDIR)/Network/packet_min_utils.c
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/packet_min_utils.o $(SRCDIR)/Network/packet_min_utils.c
	
$(OBJDIR)/Gameplay/powerups.o: $(SRCDIR)/Gameplay/powerups.cpp
	test -d $(OBJDIR)/Gameplay || mkdir -p $(OBJDIR)/Gameplay
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Gameplay/powerups.o $(SRCDIR)/Gameplay/powerups.cpp

$(OBJDIR)/Gameplay/collision_system.o: $(SRCDIR)/Gameplay/collision_system.cpp
	test -d $(OBJDIR)/Gameplay || mkdir -p $(OBJDIR)/Gameplay
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Gameplay/collision_system.o $(SRCDIR)/Gameplay/collision_system.cpp

$(OBJDIR)/Gameplay/movement_system.o: $(SRCDIR)/Gameplay/movement_system.cpp
	test -d $(OBJDIR)/Gameplay || mkdir -p $(OBJDIR)/Gameplay
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Gameplay/movement_system.o $(SRCDIR)/Gameplay/movement_system.cpp

$(OBJDIR)/Graphics/render_system.o: $(SRCDIR)/Graphics/render_system.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/render_system.o $(SRCDIR)/Graphics/render_system.cpp

$(OBJDIR)/Graphics/animation_system.o: $(SRCDIR)/Graphics/animation_system.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/animation_system.o $(SRCDIR)/Graphics/animation_system.cpp

$(OBJDIR)/Graphics/text.o: $(SRCDIR)/Graphics/text.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/text.o $(SRCDIR)/Graphics/text.cpp

$(OBJDIR)/Graphics/cutscene_system.o: $(SRCDIR)/Graphics/cutscene_system.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/cutscene_system.o $(SRCDIR)/Graphics/cutscene_system.cpp
	
$(OBJDIR)/Graphics/map.o: $(SRCDIR)/Graphics/map.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/map.o $(SRCDIR)/Graphics/map.cpp

$(OBJDIR)/Graphics/fog_of_war_system.o: $(SRCDIR)/Graphics/fog_of_war_system.cpp
	test -d $(OBJDIR)/Graphics || mkdir -p $(OBJDIR)/Graphics
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Graphics/fog_of_war_system.o $(SRCDIR)/Graphics/fog_of_war_system.cpp

$(OBJDIR)/Input/keyinputsystem.o: $(SRCDIR)/Input/keyinputsystem.cpp
	test -d $(OBJDIR)/Input || mkdir -p $(OBJDIR)/Input
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Input/keyinputsystem.o $(SRCDIR)/Input/keyinputsystem.cpp

$(OBJDIR)/Input/mouseinputsystem.o: $(SRCDIR)/Input/mouseinputsystem.cpp
	test -d $(OBJDIR)/Input || mkdir -p $(OBJDIR)/Input
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Input/mouseinputsystem.o $(SRCDIR)/Input/mouseinputsystem.cpp

$(OBJDIR)/Input/menu.o: $(SRCDIR)/Input/menu.cpp
	test -d $(OBJDIR)/Input || mkdir -p $(OBJDIR)/Input
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Input/menu.o $(SRCDIR)/Input/menu.cpp
	
$(OBJDIR)/Input/chat.o: $(SRCDIR)/Input/chat.cpp
	test -d $(OBJDIR)/Input || mkdir -p $(OBJDIR)/Input
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Input/chat.o $(SRCDIR)/Input/chat.cpp

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/main.o $(SRCDIR)/main.cpp
	
$(OBJDIR)/triggered.o: $(SRCDIR)/triggered.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/triggered.o $(SRCDIR)/triggered.cpp

$(OBJDIR)/sound.o: $(SRCDIR)/sound.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/sound.o $(SRCDIR)/sound.cpp

$(OBJDIR)/Network/GameplayCommunication.o: $(SRCDIR)/Network/GameplayCommunication.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/GameplayCommunication.o $(SRCDIR)/Network/GameplayCommunication.cpp

$(OBJDIR)/Network/ServerCommunication.o: $(SRCDIR)/Network/ServerCommunication.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/ServerCommunication.o $(SRCDIR)/Network/ServerCommunication.cpp

$(OBJDIR)/Network/PipeUtils.o: $(SRCDIR)/Network/PipeUtils.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/PipeUtils.o $(SRCDIR)/Network/PipeUtils.cpp

$(OBJDIR)/Network/NetworkRouter.o: $(SRCDIR)/Network/NetworkRouter.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/NetworkRouter.o $(SRCDIR)/Network/NetworkRouter.cpp

$(OBJDIR)/Network/ClientUpdateSystem.o: $(SRCDIR)/Network/ClientUpdateSystem.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/ClientUpdateSystem.o $(SRCDIR)/Network/ClientUpdateSystem.cpp

$(OBJDIR)/Network/SendSystem.o: $(SRCDIR)/Network/SendSystem.cpp
	test -d $(OBJDIR)/Network || mkdir -p $(OBJDIR)/Network
	$(CC) $(FLAGS) -c -o $(OBJDIR)/Network/SendSystem.o $(SRCDIR)/Network/SendSystem.cpp

$(OBJDIR)/world.o: $(SRCDIR)/world.cpp
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c -o $(OBJDIR)/world.o $(SRCDIR)/world.cpp

