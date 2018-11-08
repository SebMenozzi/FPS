.PHONY : clean, fclean, re
.SUFFIXES :

NAME = client
SRC =	src/gui/bouton.cpp src/gui/caseACocher.cpp src/carte.cpp src/gui/champSaisieNumerique.cpp \
	src/conteneurTextures.cpp src/gui/etiquette.cpp src/gui/etiquetteOverlay.cpp      \
	src/fonctionsUtiles.cpp src/historiqueDesPositions.cpp src/horloge.cpp    \
	src/gui/imageOverlay.cpp src/init.cpp src/main.cpp src/gui/menu.cpp src/objet3DStatique.cpp   \
	src/gui/overlay.cpp src/personnage.cpp src/scene.cpp src/clientUDP.cpp src/gui/widget.cpp    \
	src/gui/widgetOverlay.cpp src/gui/zoneTexte.cpp src/jeu.cpp src/fichierINI.cpp src/objet3D.cpp \
	src/traitementChemin.cpp src/conteneurMateriaux.cpp src/materiau.cpp

CC = g++

OBJ = $(SRC:.cpp=.o)

OS = $(shell uname)
ifeq ($(OS), Darwin)
	LINKER_FLAGS = -framework OpenGL -F/Library/Frameworks -framework SDL2 -framework SDL2_image -framework SDL2_TTF

  IFLAGS = -I/Library/Frameworks/SDL2.framework/Headers/ -I/Library/Frameworks/SDL2_image.framework/Headers/ -I/Library/Frameworks/SDL2_TTF.framework/Headers/
else
  LINKER_FLAGS = -lGL -lGLU -lSDL2 -lSDL2_image -lSDL2_ttf
endif

COMPILER_FLAGS = -w

all : bin/Debug/$(NAME)

bin/Debug/$(NAME) : $(OBJ)
	$(CC) $(OBJ) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

%.o : %.cpp
	$(CC) -c $^ $(COMPILER_FLAGS) $(IFLAGS) -o $@

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f  bin/Debug/$(NAME)

re : fclean all
