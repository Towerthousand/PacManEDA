# Add your players here with .o extension.

PLAYERS = itsshowtime.o Demo.o dummy/AIDummy.o

# Do not modify past this point.

EXE = PacMan SecPacMan

LDLIBS = -lm

#Debug
#CXXFLAGS = -O0 -Wall
#LDFLAGS = -g -rdynamic

#Optimize
CXXFLAGS = -O2 -Wall
LDFLAGS =



all: $(EXE)

clean:
	rm -f $(EXE) *.o Makefile.deps


PacMan: BackTrace.o Utils.o PosDir.o Board.o Action.o Player.o Registry.o Game.o PacMan.o $(PLAYERS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

SecPacMan: BackTrace.o Utils.o PosDir.o Board.o Action.o Player.o Registry.o SecGame.o SecPacMan.o $(PLAYERS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

Makefile.deps:
	$(CXX) $(CPPFLAGS) -MM *.cc > Makefile.deps

include Makefile.deps
