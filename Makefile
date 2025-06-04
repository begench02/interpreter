CC = g++
FLAGS = -g

main: *.cpp
	${CC} ${FLAGS} $^ -o $@
