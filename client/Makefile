.PHONY : clean, fclean, re
.SUFFIXES :

NAME = client
SRC =	src/gui/menu.cpp src/gui/widget.cpp src/gui/textInput.cpp \
	src/gui/numericInput.cpp src/gui/checkbox.cpp src/gui/label.cpp  \
	src/gui/button.cpp src/texturesContainer.cpp src/utils.cpp src/clock.cpp  \
	src/init.cpp src/main.cpp src/udpClient.cpp src/game.cpp \
 	src/player.cpp src/objet3DStatique.cpp src/scene.cpp src/map.cpp \
  src/heightmap.cpp src/iniFile.cpp

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

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(OBJ) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

%.o : %.cpp
	$(CC) -c $^ $(COMPILER_FLAGS) $(IFLAGS) -o $@

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f  $(NAME)

re : fclean all
