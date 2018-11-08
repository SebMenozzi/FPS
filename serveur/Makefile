.PHONY : clean, fclean, re
.SUFFIXES :

NAME = serveur
SRC =	src/main.cpp src/fonctionsUtiles.cpp src/serveurUDP.cpp src/horloge.cpp

CC = g++

OBJ = $(SRC:.cpp=.o)

COMPILER_FLAGS = -w

all : bin/Debug/$(NAME)

bin/Debug/$(NAME) : $(OBJ)
	$(CC) $(OBJ) $(COMPILER_FLAGS) -o $@

%.o : %.cpp
	$(CC) -c $^ $(COMPILER_FLAGS) -o $@

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f  bin/Debug/$(NAME)

re : fclean all
