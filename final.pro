QT += opengl network

TARGET = final
TEMPLATE = app
unix:!macx {
    LIBS += -lGLU
    QMAKE_CXXFLAGS += -std=c++11
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    QMAKE_MAC_SDK = macosx10.9
}

SOURCES += \
    main.cpp \
    lib/ResourceLoader.cpp \
    ui/mainwindow.cpp \
    scene/view.cpp \
    scene/openglscene.cpp \
    scene/scene.cpp \
    camera/camera.cpp \
    shapes/shape.cpp \
    shapes/musicshape.cpp \
    networking/udphandler.cpp \
    shapes/grid.cpp \
    shapes/room.cpp \
    shapes/cubemap.cpp \
    shapes/filtershape.cpp

HEADERS += \
    lib/Common.h \
    lib/ResourceLoader.h \
    lib/SceneData.h \
    ui/mainwindow.h \
    scene/view.h \
    scene/openglscene.h \
    scene/scene.h \
    camera/camera.h \
    shapes/shape.h \
    shapes/musicshape.h \
    networking/udphandler.h \
    shapes/grid.h \
    shapes/room.h \
    shapes/cubemap.h \
    shapes/filtershape.h

FORMS += ui/mainwindow.ui

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g.
INCLUDEPATH += glm lib ui scene camera shapes networking
DEPENDPATH += glm lib ui scene camera shapes networking
DEFINES += TIXML_USE_STL
OTHER_FILES += \
    # default shader
    shaders/shader.frag shaders/shader.vert\
    # water shader
    shaders/water.frag shaders/water.vert\
    # lighting shaders
    shaders/orb.frag shaders/orb.vert\
    shader/shader.gsh shaders/cube.gsh\
    shaders/bolt.frag shaders/bolt.vert\
    shaders/bolt.gsh shaders/boltcube.gsh\
    # cube map shaders
    shaders/cubemap.frag shader/cubemap.vert\
    # images for cube maps
    images/front.png images/back.png images/top.png\
    images/bottom.png images/left.png images/right.png\
    images/posx.jpg images/negx.jpg images/posy.jpg\
    images/negy.jpg images/posz.jpg images/negz.jpg\
    images/posx.bmp images/negx.bmp images/posy.bmp\
    images/negy.bmp images/posz.bmp images/negz.bmp

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra
# QMAKE_CXX_FLAGS_WARN_ON += -Wunknown-pragmas -Wunused-function -Wmain

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}

#TODO (Windows): If you are setting up local development on Windows (NOT Mac), comment out the following lines
win32:CONFIG(release, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/release/ -lGLEW
else:win32:CONFIG(debug, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/debug/ -lGLEW
else:unix: LIBS += -L/usr/local/Cellar/glew/1.11.0/lib/ -lGLEW

#TODO (Windows or Mac): If you are setting up local development on Windows OR Mac, fill in the correct path to your glew and uncomment the following lines:
INCLUDEPATH+=/usr/local/Cellar/glew/1.11.0/include
DEPENDPATH+=/usr/local/Cellar/glew/1.11.0/include

RESOURCES += \
    resources.qrc
