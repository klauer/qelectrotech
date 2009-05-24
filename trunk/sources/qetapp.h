/*
	Copyright 2006-2009 Xavier Guerrin
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QET_APP_H
#define QET_APP_H
#include "qetsingleapplication.h"
#include <QTranslator>
#include <QtGui>
#include "elementslocation.h"
#include "qetarguments.h"
class QETDiagramEditor;
class QETElementEditor;
class ElementsCollection;
class ElementsCollectionItem;
class FileElementsCollection;
class ElementsCategory;
class ElementDefinition;
class QETProject;
class RecentFiles;
/**
	Cette classe represente l'application QElectroTech.
	
*/
class QETApp : public QETSingleApplication {
	Q_OBJECT
	// constructeurs, destructeur
	public:
	QETApp(int &, char **);
	virtual ~QETApp();
	
	private:
	QETApp(const QETApp &);
	
	// methodes
	public:
	static QETApp *instance();
	void setLanguage(const QString &);
	static void printHelp();
	static void printVersion();
	static void printLicense();
	
	static ElementsCollectionItem *collectionItem(const ElementsLocation &, bool = true);
	static ElementsCategory *createCategory(const ElementsLocation &);
	static ElementDefinition *createElement(const ElementsLocation &);
	static ElementsCollection *commonElementsCollection();
	static ElementsCollection *customElementsCollection();
	static QList<ElementsCollection *> availableCollections();
	
	static QString userName();
	static QString commonElementsDir();
	static QString customElementsDir();
	static bool registerProject(QETProject *);
	static bool unregisterProject(QETProject *);
	static QMap<uint, QETProject *> registeredProjects();
	static QETProject *project(const uint &);
	static int projectId(const QETProject *);
	static QString configDir();
	static QSettings &settings();
	static QString languagesPath();
	static QString realPath(const QString &);
	static QString symbolicPath(const QString &);
	static RecentFiles *projectsRecentFiles();
	static RecentFiles *elementsRecentFiles();
#ifdef QET_ALLOW_OVERRIDE_CED_OPTION
	public:
	static void overrideCommonElementsDir(const QString &);
	private:
	static QString common_elements_dir; ///< Dossier contenant la collection d'elements commune
#endif
#ifdef QET_ALLOW_OVERRIDE_CD_OPTION
	public:
	static void overrideConfigDir(const QString &);
	private:
	static QString config_dir; ///< Dossier contenant la configuration et la collection d'elements de l'utilisateur
#endif
	public:
	static void overrideLangDir(const QString &);
	static QString lang_dir; ///< Dossier contenant les fichiers de langue
	static QFont diagramTextsFont(int = -1);
	static QETDiagramEditor *diagramEditorForFile(const QString &);
	static QList<QETDiagramEditor *> diagramEditors();
	static QList<QETElementEditor *> elementEditors();
	static QList<QETElementEditor *> elementEditors(QETProject *);
	
	protected:
#ifdef Q_OS_DARWIN
	bool event(QEvent *);
#endif
	
	// attributs
	private:
	QTranslator qtTranslator;
	QTranslator qetTranslator;
	QSystemTrayIcon *qsti;
	QSplashScreen *splash_screen_;
	QMenu *menu_systray;
	QAction *quitter_qet;
	QAction *reduce_appli;
	QAction *restore_appli;
	QAction *reduce_diagrams;
	QAction *restore_diagrams;
	QAction *reduce_elements;
	QAction *restore_elements;
	QAction *new_diagram;
	QAction *new_element;
	QHash<QMainWindow *, QByteArray> window_geometries;
	QHash<QMainWindow *, QByteArray> window_states;
	bool every_editor_reduced;
	bool every_diagram_reduced;
	bool every_diagram_visible;
	bool every_element_reduced;
	bool every_element_visible;
	QSignalMapper signal_map;
	QSettings *qet_settings;
	QETArguments qet_arguments_;        ///< Analyseur d'arguments
	bool non_interactive_execution_;    ///< booleen indiquant si l'application va se terminer immediatement apres un court traitement
	QPalette initial_palette_;          ///< Palette des couleurs systeme
	
	static FileElementsCollection *common_collection;
	static FileElementsCollection *custom_collection;
	static QMap<uint, QETProject *> registered_projects_;
	static uint next_project_id;
	static RecentFiles *projects_recent_files_;
	static RecentFiles *elements_recent_files_;
	
	public slots:
	void systray(QSystemTrayIcon::ActivationReason);
	void reduceEveryEditor();
	void restoreEveryEditor();
	void reduceDiagramEditors();
	void restoreDiagramEditors();
	void reduceElementEditors();
	void restoreElementEditors();
	void newDiagramEditor();
	void newElementEditor();
	bool closeEveryEditor();
	void setMainWindowVisible(QMainWindow *, bool);
	void invertMainWindowVisibility(QWidget *);
	void useSystemPalette(bool);
	void quitQET();
	void checkRemainingWindows();
	void messageReceived(const QString &);
	void openFiles(const QETArguments &);
	void openProjectFiles(const QStringList &);
	void openElementFiles(const QStringList &);
	
	// methodes privees
	private slots:
	void cleanup();
	
	private:
	QList<QETDiagramEditor *> detectDiagramEditors() const;
	QList<QETElementEditor *> detectElementEditors() const;
	QList<QWidget *> floatingToolbarsAndDocksForMainWindow(QMainWindow *) const;
	void parseArguments();
	void initSplashScreen();
	void setSplashScreenStep(const QString & = QString());
	void initLanguage();
	void initStyle();
	void initConfiguration();
	void initSystemTray();
	void buildSystemTrayMenu();
	void fetchWindowStats(const QList<QETDiagramEditor *> &diagrams, const QList<QETElementEditor *> &elements);
};

/**
	Cette classe represente le style de QElectroTech
	Il s'agit de modifications simples appliquees aux styles communs de Qt
*/
class QETStyle : public QPlastiqueStyle {
	Q_OBJECT
	public:
	QETStyle();
	virtual ~QETStyle();
	virtual int styleHint(StyleHint hint, const QStyleOption * = 0, const QWidget * = 0, QStyleHintReturn * = 0) const;
	
	protected slots:
	virtual QIcon standardIconImplementation(StandardPixmap, const QStyleOption * = 0, const QWidget * = 0) const;
};
#endif
