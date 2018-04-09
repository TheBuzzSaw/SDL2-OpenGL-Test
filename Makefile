CXXFLAGS += -std=c++14
DEBUG_CXXFLAGS += -g
ifeq ($(OS),Windows_NT)
	TARGET = sdl2-opengl-test.exe
	CXXFLAGS += -I/mingw64/include/SDL2
	LDLIBS += -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32
else
	DEBUG_CXXFLAGS += -Wall -Werror
	TARGET = sdl2-opengl-test.bin
	CXXFLAGS += -I/usr/include/SDL2
	DEBUG_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
	DEBUG_LDFLAGS += -fsanitize=address
	LDLIBS += -lSDL2main -lSDL2 -lGL -lGLEW -ldl -pthread
endif

OBJECTS = \
	main.o

all : debug

debug : CXXFLAGS += $(DEBUG_CXXFLAGS)
debug : CFLAGS += $(DEBUG_CXXFLAGS)
debug : LDFLAGS += $(DEBUG_LDFLAGS)
debug : $(TARGET)

release : CXXFLAGS += -O2
release : $(TARGET)

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

clean :
	rm -f -v *.o *.bin
