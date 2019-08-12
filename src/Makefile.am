noinst_LTLIBRARIES = libsdltool.la

SOURCES = CApp.cpp
SOURCES += CGame.cpp
SOURCES += CSurface.cpp
SOURCES += CEvent.cpp
SOURCES += Decode.cpp
SOURCES += CController.cpp
SOURCES += CButton.cpp
SOURCES += CText.cpp
SOURCES += Vector2D.cpp
SOURCES += Collision.cpp

libsdltool_la_SOURCES = $(SOURCES)

libsdltool_la_CXXFLAGS = $(SDL2_CFLAGS)
libsdltool_la_CXXFLAGS += $(FFMPEG_CFLAGS)
libsdltool_la_CXXFLAGS += $(GFLAGS_CFLAGS)
libsdltool_la_CXXFLAGS += $(GLOG_CFLAGS)

libsdltool_la_LDFLAGS = $(SDL2_LDFLAGS)
libsdltool_la_LDFLAGS += $(FFMPEG_LDFLAGS) 
libsdltool_la_LDFLAGS += $(GFLAGS_LDFLAGS) 
libsdltool_la_LDFLAGS += $(GLOG_LDFLAGS) 

bin_PROGRAMS = Player Game

Player_SOURCES = Player.cpp

Player_LDADD = libsdltool.la
Player_LDADD += $(SDL2_LIBS)
Player_LDADD += $(FFMPEG_LIBS)
Player_LDADD += $(GFLAGS_LIBS)
Player_LDADD += $(GLOG_LIBS)
Player_LDADD += -lpthread

Player_CXXFLAGS = $(SDL2_CFLAGS)
Player_CXXFLAGS += $(FFMPEG_CFLAGS)
Player_CXXFLAGS += $(GFLAGS_CFLAGS)
Player_CXXFLAGS += $(GLOG_CFLAGS)

Game_SOURCES = Game.cpp

Game_LDADD = libsdltool.la
Game_LDADD += $(SDL2_LIBS)
Game_LDADD += $(GFLAGS_LIBS)
Game_LDADD += $(GLOG_LIBS)
Game_LDADD += -lpthread

Game_CXXFLAGS = $(SDL2_CFLAGS)
Game_CXXFLAGS += $(GFLAGS_CFLAGS)
Game_CXXFLAGS += $(GLOG_CFLAGS)