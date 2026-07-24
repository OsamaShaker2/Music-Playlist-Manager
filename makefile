# Detect operating system
ifeq ($(OS),Windows_NT)
    OBJ = obj
    TARGET = playlist.exe
    RM = del /Q
	SEP = \
else
    OBJ = o
    TARGET = playlist
    RM = rm -f
	SEP = /
endif

main.$(OBJ): src/main.c include/Music.h include/UI.h
	gcc -c src/main.c -o build/main.$(OBJ)

Music.$(OBJ): src/Music.c include/Music.h
	gcc -c src/Music.c -o build/Music.$(OBJ)

UI.$(OBJ): src/UI.c include/UI.h include/Music.h
	gcc -c src/UI.c -o build/UI.$(OBJ)

all: main.$(OBJ) Music.$(OBJ) UI.$(OBJ)
	gcc build/main.$(OBJ) build/Music.$(OBJ) build/UI.$(OBJ) -o build/$(TARGET)

clean:
	-$(RM) build$(SEP)*.$(OBJ) build$(SEP)$(TARGET)