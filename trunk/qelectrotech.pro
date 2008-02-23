######################################################################
#                   Fichier projet de QElectroTech                   #
######################################################################

# Chemins utilises pour la compilation et l'installation de QET
!win32 {
	# Chemins UNIX
	COMPIL_PREFIX              = '/usr/local/'
	INSTALL_PREFIX             = '/usr/local/'
	QET_BINARY_PATH            = 'bin/'
	QET_COMMON_COLLECTION_PATH = 'share/qelectrotech/elements/'
	QET_LANG_PATH              = 'share/qelectrotech/lang/'
	QET_LICENSE_PATH           = 'doc/qelectrotech/'
	QET_DESKTOP_PATH           = 'share/applications/'
	QET_ICONS_PATH             = 'share/icons/'
} else {
	# Chemins Windows
	COMPIL_PREFIX              = './'
	INSTALL_PREFIX             = './'
	QET_BINARY_PATH            = './'
	QET_COMMON_COLLECTION_PATH = 'elements/'
	QET_LANG_PATH              = 'lang/'
	QET_LICENSE_PATH           = './'
}

# Commenter la ligne ci-dessous pour desactiver l'option --common-elements-dir
DEFINES += QET_ALLOW_OVERRIDE_CED_OPTION

# Commenter la ligne ci-dessous pour desactiver l'option --config-dir
DEFINES += QET_ALLOW_OVERRIDE_CD_OPTION

######################################################################

TEMPLATE = app
DEPENDPATH += . editor lang
INCLUDEPATH += . editor

# Fichiers sources
HEADERS += aboutqet.h \
           borderinset.h \
           conductor.h \
           conductorprofile.h \
           conductorproperties.h \
           conductorpropertieswidget.h \
           conductorsegment.h \
           conductorsegmentprofile.h \
           configdialog.h \
           configpages.h \
           customelement.h \
           diagram.h \
           diagramcommands.h \
           diagramcontent.h \
           diagramview.h \
           diagramtextitem.h \
           element.h \
           elementdeleter.h \
           elementscategorieslist.h \
           elementscategorieswidget.h \
           elementscategory.h \
           elementscategorydeleter.h \
           elementscategoryeditor.h \
           elementspanel.h \
           elementspanelwidget.h \
           elementtextitem.h \
           exportdialog.h \
           fixedelement.h \
           hotspoteditor.h \
           insetproperties.h \
           insetpropertieswidget.h \
           nameslist.h \
           nameslistwidget.h \
           newelementwizard.h \
           orientationset.h \
           orientationsetwidget.h \
           qet.h \
           qetapp.h \
           qetdiagrameditor.h \
           qgimanager.h \
           terminal.h \
           editor/arceditor.h \
           editor/circleeditor.h \
           editor/customelementgraphicpart.h \
           editor/customelementpart.h \
           editor/editorcommands.h \
           editor/elementitemeditor.h \
           editor/elementscene.h \
           editor/elementview.h \
           editor/ellipseeditor.h \
           editor/lineeditor.h \
           editor/partarc.h \
           editor/partcircle.h \
           editor/partellipse.h \
           editor/partline.h \
           editor/partpolygon.h \
           editor/partterminal.h \
           editor/parttext.h \
           editor/parttextfield.h \
           editor/polygoneditor.h \
           editor/qetelementeditor.h \
           editor/styleeditor.h \
           editor/terminaleditor.h \
           editor/texteditor.h \
           editor/textfieldeditor.h
SOURCES += aboutqet.cpp \
           borderinset.cpp \
           conductor.cpp \
           conductorprofile.cpp \
           conductorproperties.cpp \
           conductorpropertieswidget.cpp \
           conductorsegment.cpp \
           customelement.cpp \
           configdialog.cpp \
           configpages.cpp \
           diagram.cpp \
           diagramcommands.cpp \
           diagramcontent.cpp \
           diagramtextitem.cpp \
           diagramview.cpp \
           element.cpp \
           elementdeleter.cpp \
           elementscategorieslist.cpp \
           elementscategorieswidget.cpp \
           elementscategory.cpp \
           elementscategorydeleter.cpp \
           elementscategoryeditor.cpp \
           elementspanel.cpp \
           elementspanelwidget.cpp \
           elementtextitem.cpp \
           exportdialog.cpp \
           fixedelement.cpp \
           hotspoteditor.cpp \
           insetproperties.cpp \
           insetpropertieswidget.cpp \
           main.cpp \
           nameslist.cpp \
           nameslistwidget.cpp \
           newelementwizard.cpp \
           orientationset.cpp \
           orientationsetwidget.cpp \
           qet.cpp \
           qetapp.cpp \
           qetdiagrameditor.cpp \
           qgimanager.cpp \
           terminal.cpp \
           editor/arceditor.cpp \
           editor/circleeditor.cpp \
           editor/customelementgraphicpart.cpp \
           editor/customelementpart.cpp \
           editor/editorcommands.cpp \
           editor/elementitemeditor.cpp \
           editor/elementscene.cpp \
           editor/elementview.cpp \
           editor/ellipseeditor.cpp \
           editor/lineeditor.cpp \
           editor/partarc.cpp \
           editor/partcircle.cpp \
           editor/partellipse.cpp \
           editor/partline.cpp \
           editor/partpolygon.cpp \
           editor/partterminal.cpp \
           editor/parttext.cpp \
           editor/parttextfield.cpp \
           editor/polygoneditor.cpp \
           editor/qetelementeditor.cpp \
           editor/styleeditor.cpp \
           editor/terminaleditor.cpp \
           editor/texteditor.cpp \
           editor/textfieldeditor.cpp
RESOURCES += qelectrotech.qrc
TRANSLATIONS += lang/qet_en.ts lang/qt_fr.ts
RC_FILE = ico/windows_icon/application_icon/qelectrotech.rc
QT += xml svg
CONFIG += debug_and_release warn_on
TARGET = qelectrotech

# Description de l'installation
target.path     = $$join(INSTALL_PREFIX,,,$${QET_BINARY_PATH})

elements.path   = $$join(INSTALL_PREFIX,,,$${QET_COMMON_COLLECTION_PATH})
elements.files  = elements/*

lang.path       = $$join(INSTALL_PREFIX,,,$${QET_LANG_PATH})
lang.files      = $$replace(TRANSLATIONS, '.ts', '.qm')

copyright.path  = $$join(INSTALL_PREFIX,,,$${QET_LICENSE_PATH})
copyright.files = LICENSE \
                  CREDIT \
                  README

desktop.path   = $$join(INSTALL_PREFIX,,,$${QET_DESKTOP_PATH})
desktop.files  = misc/qelectrotech.desktop

icons.path     = $$join(INSTALL_PREFIX,,,$${QET_ICONS_PATH})
icons.files    = ico/qet.png

# L'installation comprend la copie du binaire, des elements, des fichiers de langue et du fichier LICENSE
INSTALLS += target elements lang copyright
# Sous Unix, on installe egalement l'icone et un fichier .desktop
unix {
	INSTALLS += desktop icons
}

# Options de compilation
# Chemin des fichiers de traduction ; par defaut : lang/ dans le repertoire d'execution
DEFINES += QET_LANG_PATH=$$join(COMPIL_PREFIX,,,$${QET_LANG_PATH})

# Chemin de la collection commune ; par defaut : elements/ dans le repertoire d'execution
DEFINES += QET_COMMON_COLLECTION_PATH=$$join(COMPIL_PREFIX,,,$${QET_COMMON_COLLECTION_PATH})
