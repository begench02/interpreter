CC = g++
FLAGS = -g

res: *.cpp
	${CC} ${FLAGS} $^ -o $@
