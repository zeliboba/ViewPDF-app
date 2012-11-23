# NOTES.txt
# CHANGES
# README
# help.html
# viewpdf.1
#DEFINES	     += DEBUG
DEFINES	     += POPPLER_ANNOTATION_BOUNDARY_BUG
#DEFINES	     += SHOW_DELETED
CONFIG	     += console
HEADERS	     += common.hpp
HEADERS	     += textbox.hpp
HEADERS	     += annotation.hpp
HEADERS	     += mainwindow.hpp
SOURCES      += mainwindow.cpp
HEADERS	     += helpform.hpp
SOURCES      += helpform.cpp
HEADERS	     += aboutform.hpp
SOURCES      += aboutform.cpp
SOURCES      += main.cpp
RESOURCES    += resources.qrc
LIBS	     += -lpoppler-qt4 -lenchant
exists($(HOME)/opt/poppler018/) {
    message(Using locally built Poppler library)
    INCLUDEPATH += $(HOME)/opt/poppler018/include/poppler/cpp
    INCLUDEPATH += $(HOME)/opt/poppler018/include/poppler/qt4
    LIBS += -Wl,-rpath -Wl,$(HOME)/opt/poppler018/lib -Wl,-L$(HOME)/opt/poppler018/lib
} else {
    exists(/usr/include/poppler/qt4) {
	INCLUDEPATH += /usr/include/poppler/cpp
	INCLUDEPATH += /usr/include/poppler/qt4
    } else {
	INCLUDEPATH += /usr/local/include/poppler/cpp
	INCLUDEPATH += /usr/local/include/poppler/qt4
    }
}
INCLUDEPATH += /usr/include/enchant
