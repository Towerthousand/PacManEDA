TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Utils.cc \
    SecPacMan.cc \
    SecGame.cc \
    Registry.cc \
    PosDir.cc \
    Player.cc \
    PacMan.cc \
    Game.cc \
    Board.cc \
    BackTrace.cc \
    Action.cc \
    itsshowtime.cc \
    Demo.cc

HEADERS += \
    Utils.hh \
    SecGame.hh \
    Registry.hh \
    PosDir.hh \
    Player.hh \
    Game.hh \
    Board.hh \
    BackTrace.hh \
    Action.hh

OTHER_FILES += \
    upc.gam \
    terror.gam \
    smile.gam \
    small.gam \
    manicomi.gam \
    Makefile.deps \
    Makefile \
    joseki.gam \
    horta.gam \
    EDA2.pro.user \
    demo.gam \
    crises.gam \
    cotolengo.gam
