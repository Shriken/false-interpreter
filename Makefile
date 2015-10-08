CXX = g++

CXXFLAGS  = -Wall -Werror -g -Wno-c++11-extensions
LDFLAGS   = -lm -lGLEW `pkg-config --libs opencv`

PLATFORM_FLAGS =

SRCDIR = src
OBJDIR = obj

BIN = faint
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

ifeq ($(shell uname -s), Darwin)
	CXX = clang++
	PLATFORM_FLAGS = -mmacosx-version-min=10.7 -D__MAC__
	LDFLAGS += -framework GLUT -framework OpenGL
endif

CXXFLAGS += $(PLATFORM_FLAGS)
CXXFLAGS += `pkg-config --cflags opencv`

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(PLATFORM_FLAGS) $(LDFLAGS) $^ -o $@

debug: $(SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $(BIN)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(PLATFORM_FLAGS) $(CXXFLAGS) $^ -c -o $@

.PHONY: clean run

clean:
	rm -rf $(wildcard src/*.o) $(BIN) $(OBJDIR)

run: $(BIN)
	./$(BIN)
