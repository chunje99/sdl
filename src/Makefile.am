noinst_LTLIBRARIES = libsdltool.la

SOURCES = CApp.cpp
SOURCES += CSurface.cpp
SOURCES += CEvent.cpp
SOURCES += Decode.cpp
SOURCES += CController.cpp
SOURCES += CButton.cpp
SOURCES += CText.cpp
SOURCES += ECS.h

libsdltool_la_SOURCES = $(SOURCES)

libsdltool_la_CXXFLAGS = $(SDL2_CFLAGS)
libsdltool_la_CXXFLAGS += $(FFMPEG_CFLAGS)
libsdltool_la_CXXFLAGS += $(GFLAGS_CFLAGS)
libsdltool_la_CXXFLAGS += $(GLOG_CFLAGS)

libsdltool_la_LDFLAGS = $(SDL2_LDFLAGS)
libsdltool_la_LDFLAGS += $(FFMPEG_LDFLAGS) 
libsdltool_la_LDFLAGS += $(GFLAGS_LDFLAGS) 
libsdltool_la_LDFLAGS += $(GLOG_LDFLAGS) 

bin_PROGRAMS = Player

Player_SOURCES = Player.cpp

Player_LDADD = libsdltool.la
Player_LDADD += $(SDL2_LIBS)
Player_LDADD += $(FFMPEG_LIBS)
Player_LDADD += $(GFLAGS_LIBS)
Player_LDADD += $(GLOG_LIBS)

Player_CXXFLAGS = $(SDL2_CFLAGS)
Player_CXXFLAGS += $(FFMPEG_CFLAGS)
Player_CXXFLAGS += $(GFLAGS_CFLAGS)
Player_CXXFLAGS += $(GLOG_CFLAGS)