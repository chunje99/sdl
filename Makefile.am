bin_PROGRAMS = CApp playTest

CApp_SOURCES = CApp.cpp
CApp_SOURCES += CSurface.cpp
CApp_SOURCES += CEvent.cpp
CApp_SOURCES += Decode.cpp

CApp_LDADD = -lpthread 
CApp_LDADD += $(SDL2_LIBS)
CApp_LDADD += $(FFMPEG_LIBS)
CApp_CXXFLAGS = $(SDL2_CFLAGS)
CApp_CXXFLAGS += $(FFMPEG_CFLAGS)
CApp_LDFLAGS = $(SDL2_LDFLAGS)
CApp_LDFLAGS += $(FFMPEG_LDFLAGS) 


playTest_SOURCES = playTest.cpp

playTest_LDADD = -lpthread 
playTest_LDADD += $(SDL2_LIBS)
playTest_LDADD += $(FFMPEG_LIBS)
playTest_CXXFLAGS = $(SDL2_CFLAGS)
playTest_CXXFLAGS += $(FFMPEG_CFLAGS)
playTest_LDFLAGS = $(SDL2_LDFLAGS)
playTest_LDFLAGS += $(FFMPEG_LDFLAGS) 
