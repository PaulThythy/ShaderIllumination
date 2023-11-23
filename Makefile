EXT = cpp
CXX=g++
CFLAGS=-Wall -I/usr/local/include
#LDFLAGS= -lGL -lGLEW -lGLU -lglut  -larmadillo
#LDFLAGS= -lGL -lGLU -lGLEW -lglut -lm 
LDFLAGS= -lGL -lGLU -lGLEW -lglut

SRCDIR = src
BINDIR = bin

SOURCES=$(wildcard $(SRCDIR)/*.$(EXT))
# $(wildcard ./utilstexture/*.$(EXT))
OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SOURCES))

DEBUBFLAG=-g

EXEC=$(BINDIR)/ToreVBOShader

all: $(EXEC)

$(EXEC): $(OBJECTS)
		$(CXX) -g -o $@ $^ $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
		@mkdir -p $(BINDIR)
		$(CXX) -g -o $@ -c $< $(CFLAGS)


clean:
		rm -rf $(BINDIR)

mrproper: clean
		rm -rf $(EXEC)

