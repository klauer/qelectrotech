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
#include "qetdiagrameditor.h"
#include "qetapp.h"
#include "diagramview.h"
#include "diagram.h"
#include "elementspanelwidget.h"
#include "conductorpropertieswidget.h"
#include "qetproject.h"
#include "projectview.h"
#include "recentfiles.h"
#include "qeticons.h"
#include "qetelementeditor.h"

/**
	constructeur
	@param files Liste de fichiers a ouvrir
	@param parent le widget parent de la fenetre principale
 */
QETDiagramEditor::QETDiagramEditor(const QStringList &files, QWidget *parent) :
	QMainWindow(parent),
	open_dialog_dir(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)),
	can_update_actions(true)
{
	// mise en place de l'interface MDI au centre de l'application
	setCentralWidget(&workspace);
	
	// nomme l'objet afin qu'il soit reperable par les feuilles de style
	workspace.setBackground(QBrush(Qt::NoBrush));
	workspace.setObjectName("mdiarea");
	
	// mise en place du signalmapper
	connect(&windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(activateWidget(QWidget *)));
	
	// titre de la fenetre
	setWindowTitle(tr("QElectroTech", "window title"));
	
	// icone de la fenetre
	setWindowIcon(QET::Icons::QETLogo);
	
	// barre de statut de la fenetre
	statusBar() -> showMessage(tr("QElectroTech", "status bar message"));
	
	// ajout du panel d'Elements en tant que QDockWidget
	qdw_pa = new QDockWidget(tr("Panel d'\351l\351ments", "dock title"), this);
	qdw_pa -> setObjectName("elements panel");
	qdw_pa -> setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	qdw_pa -> setFeatures(QDockWidget::AllDockWidgetFeatures);
	qdw_pa -> setMinimumWidth(160);
	qdw_pa -> setWidget(pa = new ElementsPanelWidget(qdw_pa));
	connect(&(pa -> elementsPanel()), SIGNAL(requestForDiagram(Diagram *)),    this, SLOT(activateDiagram(Diagram *)));
	connect(&(pa -> elementsPanel()), SIGNAL(requestForProject(QETProject *)), this, SLOT(activateProject(QETProject *)));
	
	connect(pa, SIGNAL(requestForProjectClosing(QETProject *)), this, SLOT(closeProject(QETProject *)));
	connect(pa, SIGNAL(requestForProjectPropertiesEdition(QETProject *)), this, SLOT(editProjectProperties(QETProject *)));
	connect(pa, SIGNAL(requestForDiagramPropertiesEdition(Diagram *)),    this, SLOT(editDiagramProperties(Diagram *)));
	connect(pa, SIGNAL(requestForNewDiagram(QETProject *)),     this, SLOT(addDiagramToProject(QETProject *)));
	connect(pa, SIGNAL(requestForDiagramDeletion(Diagram *)),   this, SLOT(removeDiagram(Diagram *)));
	
	qdw_undo = new QDockWidget(tr("Annulations", "dock title"));
	qdw_undo -> setObjectName("diagram_undo");
	qdw_pa -> setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	qdw_undo -> setFeatures(QDockWidget::AllDockWidgetFeatures);
	qdw_undo -> setMinimumWidth(160);
	tabifyDockWidget(qdw_pa, qdw_undo);
	QUndoView *undo_view = new QUndoView(&undo_group, this);
	undo_view -> setEmptyLabel(tr("Aucune modification"));
	qdw_undo -> setWidget(undo_view);
	
	addDockWidget(Qt::LeftDockWidgetArea, qdw_undo);
	tabifyDockWidget(qdw_undo, qdw_pa);
	
	// mise en place des actions
	actions();
	
	// mise en place de la barre d'outils
	toolbar();
	
	// mise en place des menus
	menus();
	
	// la fenetre est maximisee par defaut
	setMinimumSize(QSize(500, 350));
	setWindowState(Qt::WindowMaximized);
	
	// connexions signaux / slots pour une interface sensee
	connect(&workspace,                SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(slot_updateWindowsMenu()));
	connect(&workspace,                SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(slot_updateActions()));
	connect(QApplication::clipboard(), SIGNAL(dataChanged()),              this, SLOT(slot_updatePasteAction()));
	
	// lecture des parametres
	readSettings();
	
	// affichage
	show();
	
	// si des chemins de fichiers valides sont passes en arguments
	uint opened_projects = 0;
	if (files.count()) {
		// alors on ouvre ces fichiers
		foreach(QString file, files) {
			bool project_opening = openAndAddProject(file, false);
			if (project_opening) {
				++ opened_projects;
			}
		}
	}
	
	// si aucun schema n'a ete ouvert jusqu'a maintenant, on ouvre un nouveau schema
	if (!opened_projects) newProject();
}

/**
	Destructeur
*/
QETDiagramEditor::~QETDiagramEditor() {
}

/**
	Permet de quitter l'application lors de la fermeture de la fenetre principale
	@param qce Le QCloseEvent correspondant a l'evenement de fermeture
*/
void QETDiagramEditor::closeEvent(QCloseEvent *qce) {
	// quitte directement s'il n'y a aucun projet ouvert
	bool can_quit = true;
	if (openedProjects().count()) {
		// s'assure que la fenetre soit visible s'il y a des projets a fermer
		if (!isVisible() || isMinimized()) {
			if (isMaximized()) showMaximized();
			else showNormal();
		}
		// sinon demande la permission de fermer chaque projet
		foreach(ProjectView *project, openedProjects()) {
			if (!closeProject(project)) {
				can_quit = false;
				qce -> ignore();
				break;
			}
		}
	}
	if (can_quit) {
		writeSettings();
		setAttribute(Qt::WA_DeleteOnClose);
		qce -> accept();
	}
}

/**
	Fait passer la fenetre du mode plein ecran au mode normal et vice-versa
*/
void QETDiagramEditor::toggleFullScreen() {
	setWindowState(windowState() ^ Qt::WindowFullScreen);
}

/**
	Mise en place des actions
*/
void QETDiagramEditor::actions() {
	// icones et labels
	new_file          = new QAction(QET::Icons::DocumentNew,           tr("&Nouveau"),                             this);
	open_file         = new QAction(QET::Icons::DocumentOpen,          tr("&Ouvrir"),                              this);
	close_file        = new QAction(QET::Icons::DocumentClose,         tr("&Fermer"),                              this);
	save_file         = new QAction(QET::Icons::DocumentSave,          tr("&Enregistrer"),                         this);
	save_file_sous    = new QAction(QET::Icons::DocumentSaveAs,        tr("Enregistrer sous"),                     this);
	save_all          = new QAction(QET::Icons::DocumentSaveAll,       tr("&Enregistrer tous les sch\351mas"),     this);
	import_diagram    = new QAction(QET::Icons::DocumentImport,        tr("&Importer"),                            this);
	export_diagram    = new QAction(QET::Icons::DocumentExport,        tr("E&xporter"),                            this);
	print             = new QAction(QET::Icons::DocumentPrint,         tr("Imprimer"),                             this);
	quit_editor       = new QAction(QET::Icons::ApplicationExit,       tr("&Quitter"),                             this);
	
	undo = undo_group.createUndoAction(this, tr("Annuler"));
	undo -> setIcon(QET::Icons::EditUndo);
	redo = undo_group.createRedoAction(this, tr("Refaire"));
	redo -> setIcon(QET::Icons::EditRedo);
	cut               = new QAction(QET::Icons::EditCut,               tr("Co&uper"),                              this);
	copy              = new QAction(QET::Icons::EditCopy,              tr("Cop&ier"),                              this);
	paste             = new QAction(QET::Icons::EditPaste,             tr("C&oller"),                              this);
	select_all        = new QAction(QET::Icons::EditSelectAll,         tr("Tout s\351lectionner"),                 this);
	select_nothing    = new QAction(                                   tr("D\351s\351lectionner tout"),            this);
	select_invert     = new QAction(                                   tr("Inverser la s\351lection"),             this);
	delete_selection  = new QAction(QET::Icons::EditDelete,            tr("Supprimer"),                            this);
	rotate_selection  = new QAction(QET::Icons::ObjectRotateRight,     tr("Pivoter"),                              this);
	selection_prop    = new QAction(QET::Icons::DialogInformation,     tr("Propri\351t\351s de la s\351lection"),  this);
	conductor_reset   = new QAction(QET::Icons::ConductorSettings,     tr("R\351initialiser les conducteurs"),     this);
	conductor_default = new QAction(QET::Icons::DefaultConductor,      tr("Conducteurs par d\351faut"),            this);
	infos_diagram     = new QAction(QET::Icons::DialogInformation,     tr("Propri\351t\351s du sch\351ma"),        this);
	add_text          = new QAction(QET::Icons::PartTextField,         tr("Ajouter un champ de texte"),            this);
	add_column        = new QAction(QET::Icons::AddColumn,             tr("Ajouter une colonne"),                  this);
	remove_column     = new QAction(QET::Icons::RemoveColumn,          tr("Enlever une colonne"),                  this);
	add_row           = new QAction(QET::Icons::AddRow,                tr("Ajouter une ligne"),                    this);
	remove_row        = new QAction(QET::Icons::RemoveRow,             tr("Enlever une ligne"),                    this);
	
	prj_edit_prop     = new QAction(QET::Icons::DialogInformation,     tr("Propri\351t\351s du projet"),           this);
	prj_add_diagram   = new QAction(QET::Icons::DiagramAdd,            tr("Ajouter un sch\351ma"),                 this);
	prj_del_diagram   = new QAction(QET::Icons::DiagramDelete,         tr("Supprimer le sch\351ma"),               this);
	prj_clean         = new QAction(QET::Icons::EditClear,             tr("Nettoyer le projet"),                   this);
	
	zoom_in           = new QAction(QET::Icons::ZoomIn,                tr("Zoom avant"),                           this);
	zoom_out          = new QAction(QET::Icons::ZoomOut,               tr("Zoom arri\350re"),                      this);
	zoom_fit          = new QAction(QET::Icons::ZoomFitBest,           tr("Zoom adapt\351"),                       this);
	zoom_reset        = new QAction(QET::Icons::ZoomOriginal,          tr("Pas de zoom"),                          this);
	
	tabbed_view_mode  = new QAction(                                   tr("en utilisant des onglets"),             this);
	windowed_view_mode= new QAction(                                   tr("en utilisant des fen\352tres"),         this);
	
	mode_selection    = new QAction(QET::Icons::PartSelect,            tr("Mode Selection"),                       this);
	mode_visualise    = new QAction(QET::Icons::ViewMove,              tr("Mode Visualisation"),                   this);
	
	fullscreen        = new QAction(this);
	slot_updateFullScreenAction();
	configure         = new QAction(QET::Icons::Configure,             tr("&Configurer QElectroTech"),             this);
	
	tile_window        = new QAction(                                  tr("&Mosa\357que"),                         this);
	cascade_window     = new QAction(                                  tr("&Cascade"),                             this);
	next_window        = new QAction(                                  tr("Projet suivant"),                       this);
	prev_window        = new QAction(                                  tr("Projet pr\351c\351dent"),               this);
	
	about_qet         = new QAction(QET::Icons::QETLogo,               tr("\300 &propos de QElectroTech"),         this);
	about_qt          = new QAction(QET::Icons::QtLogo,                tr("\300 propos de &Qt"),                   this);
	
	// raccourcis clavier
	new_file          -> setShortcut(QKeySequence::New);
	open_file         -> setShortcut(QKeySequence::Open);
	close_file        -> setShortcut(QKeySequence::Close);
	save_file         -> setShortcut(QKeySequence::Save);
	import_diagram    -> setShortcut(QKeySequence(tr("Ctrl+Shift+I")));
	export_diagram    -> setShortcut(QKeySequence(tr("Ctrl+Shift+X")));
	print             -> setShortcut(QKeySequence(QKeySequence::Print));
	quit_editor       -> setShortcut(QKeySequence(tr("Ctrl+Q")));
	
	undo              -> setShortcut(QKeySequence::Undo);
	redo              -> setShortcut(QKeySequence::Redo);
	cut               -> setShortcut(QKeySequence::Cut);
	copy              -> setShortcut(QKeySequence::Copy);
	paste             -> setShortcut(QKeySequence::Paste);
	select_all        -> setShortcut(QKeySequence::SelectAll);
	select_nothing    -> setShortcut(QKeySequence(tr("Ctrl+Shift+A")));
	select_invert     -> setShortcut(QKeySequence(tr("Ctrl+I")));
	delete_selection  -> setShortcut(QKeySequence(tr("Suppr")));
	rotate_selection  -> setShortcut(QKeySequence(tr("Ctrl+R")));
	selection_prop    -> setShortcut(QKeySequence(tr("Ctrl+J")));
	conductor_reset   -> setShortcut(QKeySequence(tr("Ctrl+K")));
	infos_diagram     -> setShortcut(QKeySequence(tr("Ctrl+L")));
	conductor_default -> setShortcut(QKeySequence(tr("Ctrl+D")));
	
	prj_add_diagram   -> setShortcut(QKeySequence(tr("Ctrl+T")));
	
	zoom_in           -> setShortcut(QKeySequence::ZoomIn);
	zoom_out          -> setShortcut(QKeySequence::ZoomOut);
	zoom_fit          -> setShortcut(QKeySequence(tr("Ctrl+9")));
	zoom_reset        -> setShortcut(QKeySequence(tr("Ctrl+0")));
	
	fullscreen        -> setShortcut(QKeySequence(tr("Ctrl+Shift+F")));
	
	next_window       -> setShortcut(QKeySequence::NextChild);
	prev_window       -> setShortcut(QKeySequence::PreviousChild);
	
	// affichage dans la barre de statut
	new_file          -> setStatusTip(tr("Cr\351e un nouveau sch\351ma", "status bar tip"));
	open_file         -> setStatusTip(tr("Ouvre un sch\351ma existant", "status bar tip"));
	close_file        -> setStatusTip(tr("Ferme le sch\351ma courant", "status bar tip"));
	save_file         -> setStatusTip(tr("Enregistre le sch\351ma courant", "status bar tip"));
	save_file_sous    -> setStatusTip(tr("Enregistre le sch\351ma courant avec un autre nom de fichier", "status bar tip"));
	save_all          -> setStatusTip(tr("Enregistre tous les sch\351mas du projet courant", "status bar tip"));
	import_diagram    -> setStatusTip(tr("Importe un sch\351ma dans le sch\351ma courant", "status bar tip"));
	export_diagram    -> setStatusTip(tr("Exporte le sch\351ma courant dans un autre format", "status bar tip"));
	print             -> setStatusTip(tr("Imprime le sch\351ma courant", "status bar tip"));
	quit_editor       -> setStatusTip(tr("Ferme l'application QElectroTech", "status bar tip"));
	
	undo              -> setStatusTip(tr("Annule l'action pr\351c\351dente", "status bar tip"));
	redo              -> setStatusTip(tr("Restaure l'action annul\351e", "status bar tip"));
	cut               -> setStatusTip(tr("Transf\350re les \351l\351ments s\351lectionn\351s dans le presse-papier", "status bar tip"));
	copy              -> setStatusTip(tr("Copie les \351l\351ments s\351lectionn\351s dans le presse-papier", "status bar tip"));
	paste             -> setStatusTip(tr("Place les \351l\351ments du presse-papier sur le sch\351ma", "status bar tip"));
	select_all        -> setStatusTip(tr("S\351lectionne tous les \351l\351ments du sch\351ma", "status bar tip"));
	select_nothing    -> setStatusTip(tr("D\351s\351lectionne tous les \351l\351ments du sch\351ma", "status bar tip"));
	select_invert     -> setStatusTip(tr("D\351s\351lectionne les \351l\351ments s\351lectionn\351s et s\351lectionne les \351l\351ments non s\351lectionn\351s", "status bar tip"));
	delete_selection  -> setStatusTip(tr("Enl\350ve les \351l\351ments s\351lectionn\351s du sch\351ma", "status bar tip"));
	rotate_selection  -> setStatusTip(tr("Pivote les \351l\351ments s\351lectionn\351s", "status bar tip"));
	selection_prop    -> setStatusTip(tr("\311dite les propri\351t\351s des objets s\351lectionn\351", "status bar tip"));
	conductor_reset   -> setStatusTip(tr("Recalcule les chemins des conducteurs sans tenir compte des modifications", "status bar tip"));
	conductor_default -> setStatusTip(tr("Sp\351cifie les propri\351t\351s par d\351faut des conducteurs", "status bar tip"));
	infos_diagram     -> setStatusTip(tr("\311dite les informations affich\351es par le cartouche", "status bar tip"));
	add_column        -> setStatusTip(tr("Ajoute une colonne au sch\351ma", "status bar tip"));
	remove_column     -> setStatusTip(tr("Enl\350ve une colonne au sch\351ma", "status bar tip"));
	add_row           -> setStatusTip(tr("Agrandit le sch\351ma en hauteur", "status bar tip"));
	remove_row        -> setStatusTip(tr("R\351tr\351cit le sch\351ma en hauteur", "status bar tip"));
	
	zoom_in           -> setStatusTip(tr("Agrandit le sch\351ma", "status bar tip"));
	zoom_out          -> setStatusTip(tr("R\351tr\351cit le sch\351ma", "status bar tip"));
	zoom_fit          -> setStatusTip(tr("Adapte la taille du sch\351ma afin qu'il soit enti\350rement visible", "status bar tip"));
	zoom_reset        -> setStatusTip(tr("Restaure le zoom par d\351faut", "status bar tip"));
	
	windowed_view_mode -> setStatusTip(tr("Pr\351sente les diff\351rents projets ouverts dans des sous-fen\352tres", "status bar tip"));
	tabbed_view_mode   -> setStatusTip(tr("Pr\351sente les diff\351rents projets ouverts des onglets", "status bar tip"));
	
	mode_selection    -> setStatusTip(tr("Permet de s\351lectionner les \351l\351ments", "status bar tip"));
	mode_visualise    -> setStatusTip(tr("Permet de visualiser le sch\351ma sans pouvoir le modifier", "status bar tip"));
	
	configure         -> setStatusTip(tr("Permet de r\351gler diff\351rents param\350tres de QElectroTech", "status bar tip"));
	
	tile_window       -> setStatusTip(tr("Dispose les fen\352tres en mosa\357que", "status bar tip"));
	cascade_window    -> setStatusTip(tr("Dispose les fen\352tres en cascade", "status bar tip"));
	next_window       -> setStatusTip(tr("Active le projet suivant", "status bar tip"));
	prev_window       -> setStatusTip(tr("Active le projet pr\351c\351dent", "status bar tip"));
	
	about_qet         -> setStatusTip(tr("Affiche des informations sur QElectroTech", "status bar tip"));
	about_qt          -> setStatusTip(tr("Affiche des informations sur la biblioth\350que Qt", "status bar tip"));
	
	// traitements speciaux
	add_text           -> setCheckable(true);
	windowed_view_mode -> setCheckable(true);
	tabbed_view_mode   -> setCheckable(true);
	mode_selection     -> setCheckable(true);
	mode_visualise     -> setCheckable(true);
	mode_selection     -> setChecked(true);
	
	grp_visu_sel = new QActionGroup(this);
	grp_visu_sel -> addAction(mode_selection);
	grp_visu_sel -> addAction(mode_visualise);
	grp_visu_sel -> setExclusive(true);
	
	grp_view_mode = new QActionGroup(this);
	grp_view_mode -> addAction(windowed_view_mode);
	grp_view_mode -> addAction(tabbed_view_mode);
	grp_view_mode -> setExclusive(true);
	
	QETApp *qet_app = QETApp::instance();
	
	// connexion a des slots
	connect(quit_editor,        SIGNAL(triggered()), this,       SLOT(close())                     );
	connect(select_all,         SIGNAL(triggered()), this,       SLOT(slot_selectAll())            );
	connect(select_nothing,     SIGNAL(triggered()), this,       SLOT(slot_selectNothing())        );
	connect(select_invert,      SIGNAL(triggered()), this,       SLOT(slot_selectInvert())         );
	connect(delete_selection,   SIGNAL(triggered()), this,       SLOT(slot_delete())               );
	connect(rotate_selection,   SIGNAL(triggered()), this,       SLOT(slot_rotate())               );
	connect(fullscreen,         SIGNAL(triggered()), this,       SLOT(toggleFullScreen())          );
	connect(configure,          SIGNAL(triggered()), qet_app,    SLOT(configureQET())              );
	connect(windowed_view_mode, SIGNAL(triggered()), this,       SLOT(setWindowedMode())           );
	connect(tabbed_view_mode,   SIGNAL(triggered()), this,       SLOT(setTabbedMode())             );
	connect(mode_selection,     SIGNAL(triggered()), this,       SLOT(slot_setSelectionMode())     );
	connect(mode_visualise,     SIGNAL(triggered()), this,       SLOT(slot_setVisualisationMode()) );
	connect(about_qet,          SIGNAL(triggered()), qet_app,    SLOT(aboutQET())                  );
	connect(about_qt,           SIGNAL(triggered()), qet_app,    SLOT(aboutQt())                   );
	connect(prj_edit_prop,      SIGNAL(triggered()), this,       SLOT(editCurrentProjectProperties()));
	connect(prj_add_diagram,    SIGNAL(triggered()), this,       SLOT(addDiagramToProject())       );
	connect(prj_del_diagram,    SIGNAL(triggered()), this,       SLOT(removeDiagramFromProject())  );
	connect(prj_clean,          SIGNAL(triggered()), this,       SLOT(cleanCurrentProject())       );
	connect(zoom_in,            SIGNAL(triggered()), this,       SLOT(slot_zoomIn())               );
	connect(zoom_out,           SIGNAL(triggered()), this,       SLOT(slot_zoomOut())              );
	connect(zoom_fit,           SIGNAL(triggered()), this,       SLOT(slot_zoomFit())              );
	connect(zoom_reset,         SIGNAL(triggered()), this,       SLOT(slot_zoomReset())            );
	connect(print,              SIGNAL(triggered()), this,       SLOT(printDialog())               );
	connect(export_diagram,     SIGNAL(triggered()), this,       SLOT(exportDialog())              );
	connect(save_file_sous,     SIGNAL(triggered()), this,       SLOT(saveAsDialog())              );
	connect(save_file,          SIGNAL(triggered()), this,       SLOT(save())                      );
	connect(save_all,           SIGNAL(triggered()), this,       SLOT(saveAll())                   );
	connect(new_file,           SIGNAL(triggered()), this,       SLOT(newProject())                );
	connect(open_file,          SIGNAL(triggered()), this,       SLOT(openProject())               );
	connect(close_file,         SIGNAL(triggered()), this,       SLOT(closeCurrentProject())       );
	connect(cut,                SIGNAL(triggered()), this,       SLOT(slot_cut())                  );
	connect(copy,               SIGNAL(triggered()), this,       SLOT(slot_copy())                 );
	connect(paste,              SIGNAL(triggered()), this,       SLOT(slot_paste())                );
	connect(tile_window,        SIGNAL(triggered()), &workspace, SLOT(tileSubWindows())            );
	connect(cascade_window,     SIGNAL(triggered()), &workspace, SLOT(cascadeSubWindows())         );
	connect(next_window,        SIGNAL(triggered()), &workspace, SLOT(activateNextSubWindow())     );
	connect(prev_window,        SIGNAL(triggered()), &workspace, SLOT(activatePreviousSubWindow()) );
	connect(selection_prop,     SIGNAL(triggered()), this,       SLOT(editSelectionProperties())   );
	connect(conductor_reset,    SIGNAL(triggered()), this,       SLOT(slot_resetConductors())      );
	connect(conductor_default,  SIGNAL(triggered()), this,       SLOT(slot_editDefaultConductors()));
	connect(infos_diagram,      SIGNAL(triggered()), this,       SLOT(editCurrentDiagramProperties()));
	connect(add_text,           SIGNAL(triggered()), this,       SLOT(slot_addText())              );
	connect(add_column,         SIGNAL(triggered()), this,       SLOT(slot_addColumn())            );
	connect(remove_column,      SIGNAL(triggered()), this,       SLOT(slot_removeColumn())         );
	connect(add_row,            SIGNAL(triggered()), this,       SLOT(slot_addRow())               );
	connect(remove_row,         SIGNAL(triggered()), this,       SLOT(slot_removeRow())            );
}

/**
	Gere les evenements du l'editeur de schema
	Reimplemente ici pour :
	  * eviter un conflit sur le raccourci clavier "Ctrl+W" (QKeySequence::Close)
	  * mettre a jour l'action permettant d'entrer en mode plein ecran ou d'en sortir
	@param e Evenement
*/
bool QETDiagramEditor::event(QEvent *e) {
	if (e -> type() == QEvent::ShortcutOverride) {
		QKeyEvent *shortcut_event = static_cast<QKeyEvent *>(e);
		if (shortcut_event && shortcut_event -> matches(QKeySequence::Close)) {
			close_file -> trigger();
			e -> accept();
			return(true);
		}
	} else if (e -> type() == QEvent::WindowStateChange) {
		slot_updateFullScreenAction();
	}
	return(QMainWindow::event(e));
}

/**
	Mise en place des menus
*/
void QETDiagramEditor::menus() {
	QMenu *menu_fichier   = menuBar() -> addMenu(tr("&Fichier"));
	QMenu *menu_edition   = menuBar() -> addMenu(tr("&\311dition"));
	QMenu *menu_project   = menuBar() -> addMenu(tr("&Projet"));
	QMenu *menu_affichage = menuBar() -> addMenu(tr("Afficha&ge"));
	//QMenu *menu_outils    = menuBar() -> addMenu(tr("O&utils"));
	QMenu *menu_config    = menuBar() -> addMenu(tr("&Configuration"));
	windows_menu          = menuBar() -> addMenu(tr("Fe&n\352tres"));
	QMenu *menu_aide      = menuBar() -> addMenu(tr("&Aide"));
	
	// tear off feature rulezz... pas ^^ mais bon...
	menu_fichier   -> setTearOffEnabled(true);
	menu_edition   -> setTearOffEnabled(true);
	menu_project   -> setTearOffEnabled(true);
	menu_affichage -> setTearOffEnabled(true);
	//menu_outils    -> setTearOffEnabled(true);
	menu_config    -> setTearOffEnabled(true);
	windows_menu   -> setTearOffEnabled(true);
	menu_aide      -> setTearOffEnabled(true);
	
	// menu Fichier
	menu_fichier -> addAction(new_file);
	menu_fichier -> addAction(open_file);
	menu_fichier -> addMenu(QETApp::projectsRecentFiles() -> menu());
	connect(QETApp::projectsRecentFiles(), SIGNAL(fileOpeningRequested(const QString &)), this, SLOT(openRecentFile(const QString &)));
	menu_fichier -> addAction(save_file);
	menu_fichier -> addAction(save_file_sous);
	menu_fichier -> addAction(save_all);
	menu_fichier -> addAction(close_file);
	menu_fichier -> addSeparator();
	//menu_fichier -> addAction(import_diagram);
	menu_fichier -> addAction(export_diagram);
	//menu_fichier -> addSeparator();
	menu_fichier -> addAction(print);
	menu_fichier -> addSeparator();
	menu_fichier -> addAction(quit_editor);
	
	// menu Edition
	menu_edition -> addAction(undo);
	menu_edition -> addAction(redo);
	menu_edition -> addSeparator();
	menu_edition -> addAction(cut);
	menu_edition -> addAction(copy);
	menu_edition -> addAction(paste);
	menu_edition -> addSeparator();
	menu_edition -> addAction(select_all);
	menu_edition -> addAction(select_nothing);
	menu_edition -> addAction(select_invert);
	menu_edition -> addSeparator();
	menu_edition -> addAction(delete_selection);
	menu_edition -> addAction(rotate_selection);
	menu_edition -> addAction(selection_prop);
	menu_edition -> addSeparator();
	menu_edition -> addAction(conductor_reset);
	menu_edition -> addAction(conductor_default);
	menu_edition -> addSeparator();
	menu_edition -> addAction(infos_diagram);
	menu_edition -> addAction(add_column);
	menu_edition -> addAction(remove_column);
	menu_edition -> addAction(add_row);
	menu_edition -> addAction(remove_row);
	
	// menu Projet
	menu_project -> addAction(prj_edit_prop);
	menu_project -> addAction(prj_add_diagram);
	menu_project -> addAction(prj_del_diagram);
	menu_project -> addAction(prj_clean);
	
	// menu Configurer > Afficher
	QMenu *display_toolbars = createPopupMenu();
	display_toolbars -> setTearOffEnabled(true);
	display_toolbars -> setTitle(tr("Afficher"));
	display_toolbars -> setIcon(QET::Icons::ConfigureToolbars);
	main_bar    -> toggleViewAction() -> setStatusTip(tr("Affiche ou non la barre d'outils principale"));
	view_bar    -> toggleViewAction() -> setStatusTip(tr("Affiche ou non la barre d'outils Affichage"));
	diagram_bar -> toggleViewAction() -> setStatusTip(tr("Affiche ou non la barre d'outils Sch\351ma"));
	qdw_pa      -> toggleViewAction() -> setStatusTip(tr("Affiche ou non le panel d'appareils"));
	qdw_undo    -> toggleViewAction() -> setStatusTip(tr("Affiche ou non la liste des modifications"));
	
	// menu Affichage
	QMenu *projects_view_mode = menu_affichage -> addMenu(tr("Afficher les projets"));
	projects_view_mode -> setTearOffEnabled(true);
	projects_view_mode -> addAction(windowed_view_mode);
	projects_view_mode -> addAction(tabbed_view_mode);
	
	menu_affichage -> addSeparator();
	menu_affichage -> addAction(mode_selection);
	menu_affichage -> addAction(mode_visualise);
	menu_affichage -> addSeparator();
	menu_affichage -> addAction(zoom_in);
	menu_affichage -> addAction(zoom_out);
	menu_affichage -> addAction(zoom_fit);
	menu_affichage -> addAction(zoom_reset);
	
	// menu Configuration
	menu_config -> addMenu(display_toolbars);
	menu_config -> addAction(fullscreen);
	menu_config -> addAction(configure);
	
	// menu Fenetres
	slot_updateWindowsMenu();
	
	// menu Aide
	menu_aide -> addAction(about_qet);
	menu_aide -> addAction(about_qt);
}

/**
	Mise en place de la barre d'outils
*/
void QETDiagramEditor::toolbar() {
	main_bar = new QToolBar(tr("Outils"), this);
	main_bar -> setObjectName("toolbar");
	
	view_bar = new QToolBar(tr("Affichage"), this);
	view_bar -> setObjectName("display");
	
	diagram_bar = new QToolBar(tr("Sch\351ma"), this);
	diagram_bar -> setObjectName("diagram");
	
	main_bar -> addAction(new_file);
	main_bar -> addAction(open_file);
	main_bar -> addAction(save_file);
	main_bar -> addAction(save_file_sous);
	main_bar -> addAction(save_all);
	main_bar -> addAction(close_file);
	main_bar -> addAction(print);
	main_bar -> addSeparator();
	main_bar -> addAction(undo);
	main_bar -> addAction(redo);
	main_bar -> addSeparator();
	main_bar -> addAction(cut);
	main_bar -> addAction(copy);
	main_bar -> addAction(paste);
	main_bar -> addSeparator();
	main_bar -> addAction(delete_selection);
	main_bar -> addAction(rotate_selection);
	main_bar -> addAction(selection_prop);
	
	// Modes selection / visualisation et zoom
	view_bar -> addAction(mode_selection);
	view_bar -> addAction(mode_visualise);
	view_bar -> addSeparator();
	view_bar -> addAction(zoom_in);
	view_bar -> addAction(zoom_out);
	view_bar -> addAction(zoom_fit);
	view_bar -> addAction(zoom_reset);
	
	diagram_bar -> addAction(infos_diagram);
	diagram_bar -> addAction(add_text);
	diagram_bar -> addAction(conductor_default);
	diagram_bar -> addAction(conductor_reset);
	
	// ajout de la barre d'outils a la fenetre principale
	addToolBar(Qt::TopToolBarArea, main_bar);
	addToolBar(Qt::TopToolBarArea, view_bar);
	addToolBar(Qt::TopToolBarArea, diagram_bar);
}

/**
	Imprime le schema courant
*/
void QETDiagramEditor::printDialog() {
	ProjectView *current_project = currentProject();
	if (!current_project) return;
	current_project -> printProject();
}

/**
	Gere l'export de schema sous forme d'image
*/
void QETDiagramEditor::exportDialog() {
	ProjectView *current_project = currentProject();
	if (!current_project) return;
	current_project -> exportProject();
}

/**
	Methode enregistrant le schema dans le dernier nom de fichier connu.
	@return true si l'enregistrement a reussi, false sinon
*/
bool QETDiagramEditor::save() {
	if (ProjectView *project_view = currentProject()) {
		bool save_file = project_view -> save();
		if (save_file) {
			QETApp::projectsRecentFiles() -> fileWasOpened(project_view -> project() -> filePath());
		}
		return(save_file);
	}
	return(false);
}

/**
	Cette methode demande un nom de fichier a l'utilisateur pour enregistrer le schema
	@return true si l'enregistrement a reussi, false sinon
*/
bool QETDiagramEditor::saveAsDialog() {
	if (ProjectView *project_view = currentProject()) {
		bool save_file = project_view -> saveAs();
		if (save_file) {
			QETApp::projectsRecentFiles() -> fileWasOpened(project_view -> project() -> filePath());
		}
		return(save_file);
	}
	return(false);
}

/**
	Methode enregistrant tous les schemas.
	@return true si l'enregistrement a reussi, false sinon
*/
bool QETDiagramEditor::saveAll() {
	if (ProjectView *project_view = currentProject()) {
		bool save_file = project_view -> saveAll();
		if (save_file) {
			QETApp::projectsRecentFiles() -> fileWasOpened(project_view -> project() -> filePath());
		}
		return(save_file);
	}
	return(false);
}

/**
	Cree un nouveau projet vide
*/
bool QETDiagramEditor::newProject() {
	// cree un nouveau projet sans schema
	QETProject *new_project = new QETProject(0);
	
	// transmet les proprietes par defaut des nouveaux schemas
	new_project -> setDefaultBorderProperties(defaultBorderProperties());
	new_project -> setDefaultConductorProperties(defaultConductorProperties());
	new_project -> setDefaultInsetProperties(defaultInsetProperties());
	
	// ajoute un schema au projet
	new_project -> addNewDiagram();
	
	return(addProject(new_project));
}

/**
	Slot utilise pour ouvrir un fichier recent.
	Transfere filepath au slot openAndAddDiagram seulement si cet editeur est
	actif
	@param filepath Fichier a ouvrir
	@see openAndAddDiagram
*/
bool QETDiagramEditor::openRecentFile(const QString &filepath) {
	if (qApp -> activeWindow() != this) return(false);
	return(openAndAddProject(filepath));
}

/**
	Cette fonction demande un nom de fichier a ouvrir a l'utilisateur
	@return true si l'ouverture a reussi, false sinon
*/
bool QETDiagramEditor::openProject() {
	// demande un chemin de fichier a ouvrir a l'utilisateur
	QString filepath = QFileDialog::getOpenFileName(
		this,
		tr("Ouvrir un fichier"),
		open_dialog_dir.absolutePath(),
		tr("Sch\351mas QElectroTech (*.qet);;Fichiers XML (*.xml);;Tous les fichiers (*)")
	);
	if (filepath.isEmpty()) return(false);
	
	// retient le dossier contenant le dernier projet ouvert
	open_dialog_dir = QDir(filepath);
	
	// ouvre le fichier
	return(openAndAddProject(filepath));
}

/**
	Ferme un projet
	@param project_view Projet a fermer
	@return true si la fermeture du projet a reussi, false sinon
	Note : cette methode renvoie true si project est nul
*/
bool QETDiagramEditor::closeProject(ProjectView *project_view) {
	if (project_view) {
		activateProject(project_view);
		if (QMdiSubWindow *sub_window = subWindowForWidget(project_view)) {
			return(sub_window -> close());
		}
	}
	return(true);
}

/**
	Ferme un projet
	@param project projet a fermer
	@return true si la fermeture du fichier a reussi, false sinon
	Note : cette methode renvoie true si project est nul
*/
bool QETDiagramEditor::closeProject(QETProject *project) {
	if (ProjectView *project_view = findProject(project)) {
		return(closeProject(project_view));
	}
	return(true);
}

/**
	Ferme le projet courant
	@return true si la fermeture du fichier a reussi, false sinon
	Note : cette methode renvoie true s'il n'y a pas de projet courant
*/
bool QETDiagramEditor::closeCurrentProject() {
	if (ProjectView *project_view = currentProject()) {
		return(closeProject(project_view));
	}
	return(true);
}

/**
	Ouvre un projet depuis un fichier et l'ajoute a cet editeur
	@param filepath Chemin du projet a ouvrir
	@param interactive true pour afficher des messages a l'utilisateur, false sinon
	@return true si l'ouverture a reussi, false sinon
*/
bool QETDiagramEditor::openAndAddProject(const QString &filepath, bool interactive) {
	if (filepath.isEmpty()) return(false);
	
	QFileInfo filepath_info(filepath);
	// verifie que le projet n'est pas deja ouvert dans un editeur
	QString my_filepath = filepath_info.canonicalFilePath();
	if (QETDiagramEditor *diagram_editor = QETApp::diagramEditorForFile(filepath)) {
		if (diagram_editor == this) {
			if (ProjectView *project_view = viewForFile(filepath)) {
				activateWidget(project_view);
				show();
				activateWindow();
			}
			return(false);
		} else {
			// demande a l'autre editeur d'afficher le fichier
			return(diagram_editor -> openAndAddProject(filepath));
		}
	}
	
	// verifie que le fichier est accessible en lecture
	if (!filepath_info.isReadable()) {
		if (interactive) {
			QMessageBox::critical(
				this,
				tr("Impossible d'ouvrir le fichier"),
				tr("Il semblerait que le fichier que vous essayez d'ouvrir ne "
				"soit pas accessible en lecture. Il est donc impossible de "
				"l'ouvrir. Veuillez v\351rifier les permissions du fichier.")
			);
		}
		return(false);
	}
	
	// gere le fait que le fichier puisse etre en lecture seule
	if (!filepath_info.isWritable()) {
		if (interactive) {
			QMessageBox::warning(
				this,
				tr("Ouverture du projet en lecture seule"),
				tr("Il semblerait que le projet que vous essayez d'ouvrir ne "
				"soit pas accessible en \351criture. Il sera donc ouvert en "
				"lecture seule.")
			);
		}
	}
	
	// cree le projet a partir du fichier
	QETProject *project = new QETProject(filepath);
	if (project -> state() != QETProject::Ok) {
		if (interactive) {
			QMessageBox::warning(
				this,
				tr("\311chec de l'ouverture du projet", "message box title"),
				QString(
					tr(
						"Il semblerait que le fichier %1 ne soit pas un fichier"
						" projet QElectroTech. Il ne peut donc \352tre ouvert.",
						"message box content"
					)
				).arg(filepath)
			);
		}
		return(false);
	}
	
	// a ce stade, l'ouverture du fichier a reussi
	// on l'ajoute a la liste des fichiers recents
	QETApp::projectsRecentFiles() -> fileWasOpened(filepath);
	// ... et on l'ajoute dans l'application
	return(addProject(project));
}

/**
	Ajoute un projet
	@param project projet a ajouter
*/
bool QETDiagramEditor::addProject(QETProject *project) {
	// enregistre le projet
	QETApp::registerProject(project);
	
	// cree un ProjectView pour visualiser le projet
	ProjectView *project_view = new ProjectView(project);
	addProjectView(project_view);
	
	// met a jour le panel d'elements
	pa -> elementsPanel().projectWasOpened(project);
	
	return(true);
}

/**
	@return la liste des projets ouverts dans cette fenetre
*/
QList<ProjectView *> QETDiagramEditor::openedProjects() const {
	QList<ProjectView *> result;
	QList<QMdiSubWindow *> window_list(workspace.subWindowList());
	foreach(QMdiSubWindow *window, window_list) {
		if (ProjectView *project_view = qobject_cast<ProjectView *>(window -> widget())) {
			result << project_view;
		}
	}
	return(result);
}

/**
	@return Le projet actuellement edite (= qui a le focus dans l'interface
	MDI) ou 0 s'il n'y en a pas
*/
ProjectView *QETDiagramEditor::currentProject() const {
	QMdiSubWindow *current_window = workspace.activeSubWindow();
	if (!current_window) return(0);
	
	QWidget *current_widget = current_window -> widget();
	if (!current_widget) return(0);
	
	if (ProjectView *project_view = qobject_cast<ProjectView *>(current_widget)) {
		return(project_view);
	}
	return(0);
}

/**
	@return Le schema actuellement edite (= l'onglet ouvert dans le projet
	courant) ou 0 s'il n'y en a pas
*/
DiagramView *QETDiagramEditor::currentDiagram() const {
	if (ProjectView *project_view = currentProject()) {
		return(project_view -> currentDiagram());
	}
	return(0);
}

/**
	Cette methode permet de retrouver le projet contenant un schema donne.
	@param diagram_view Schema dont il faut retrouver
	@return la vue sur le projet contenant ce schema ou 0 s'il n'y en a pas
*/
ProjectView *QETDiagramEditor::findProject(DiagramView *diagram_view) const {
	foreach(ProjectView *project_view, openedProjects()) {
		if (project_view -> diagrams().contains(diagram_view)) {
			return(project_view);
		}
	}
	return(0);
}

/**
	Cette methode permet de retrouver le projet contenant un schema donne.
	@param diagram Schema dont il faut retrouver
	@return la vue sur le projet contenant ce schema ou 0 s'il n'y en a pas
*/
ProjectView *QETDiagramEditor::findProject(Diagram *diagram) const {
	foreach(ProjectView *project_view, openedProjects()) {
		foreach(DiagramView *diagram_view, project_view -> diagrams()) {
			if (diagram_view -> diagram() == diagram) {
				return(project_view);
			}
		}
	}
	return(0);
}

/**
	@param project Projet dont il faut trouver la vue
	@return la vue du projet passe en parametre
*/
ProjectView *QETDiagramEditor::findProject(QETProject *project) const {
	foreach(ProjectView *opened_project, openedProjects()) {
		if (opened_project -> project() == project) {
			return(opened_project);
		}
	}
	return(0);
}

/**
	@param filepath Chemin de fichier d'un projet
	@return le ProjectView correspondant au chemin passe en parametre, ou 0 si
	celui-ci n'a pas ete trouve
*/
ProjectView *QETDiagramEditor::findProject(const QString &filepath) const {
	foreach(ProjectView *opened_project, openedProjects()) {
		if (QETProject *project = opened_project -> project()) {
			if (project -> filePath() == filepath) {
				return(opened_project);
			}
		}
	}
	return(0);
}

/**
	@param widget Widget a rechercher dans la zone MDI
	@return La sous-fenetre accueillant le widget passe en parametre, ou 0 si
	celui-ci n'a pas ete trouve.
*/
QMdiSubWindow *QETDiagramEditor::subWindowForWidget(QWidget *widget) const {
	foreach(QMdiSubWindow *sub_window, workspace.subWindowList()) {
		if (sub_window -> widget() == widget) {
			return(sub_window);
		}
	}
	return(0);
}

/**
	@param widget Widget a activer
*/
void QETDiagramEditor::activateWidget(QWidget *widget) {
	QMdiSubWindow *sub_window = subWindowForWidget(widget);
	if (sub_window) {
		workspace.setActiveSubWindow(sub_window);
	}
}

/**
	@param project_view Projet concerne
	@param from Index de l'onglet avant le deplacement
	@param to   Index de l'onglet apres le deplacement
*/
void QETDiagramEditor::diagramOrderChanged(ProjectView *project_view, int from, int to) {
	if (!project_view) return;
	
	QETProject *project = project_view -> project();
	if (!project) return;
	
	pa -> elementsPanel().diagramOrderChanged(project, from, to);
}

/**
	Effectue l'action "couper" sur le schema en cours
*/
void QETDiagramEditor::slot_cut() {
	if(currentDiagram()) currentDiagram() -> cut();
}

/**
	Effectue l'action "copier" sur le diagram en cours
*/
void QETDiagramEditor::slot_copy() {
	if(currentDiagram()) currentDiagram() -> copy();
}

/**
	Effectue l'action "coller" sur le schema en cours
*/
void QETDiagramEditor::slot_paste() {
	if(currentDiagram()) currentDiagram() -> paste();
}

/**
	Effectue l'action "zoom avant" sur le diagram en cours
*/
void QETDiagramEditor::slot_zoomIn() {
	if(currentDiagram()) currentDiagram() -> zoomIn();
}

/**
	Effectue l'action "zoom arriere" sur le schema en cours
*/
void QETDiagramEditor::slot_zoomOut() {
	if(currentDiagram()) currentDiagram() -> zoomOut();
}

/**
	Effectue l'action "zoom arriere" sur le diagram en cours
*/
void QETDiagramEditor::slot_zoomFit() {
	if(currentDiagram()) currentDiagram() -> zoomFit();
}

/**
	Effectue l'action "zoom par defaut" sur le schema en cours
*/
void QETDiagramEditor::slot_zoomReset() {
	if(currentDiagram()) currentDiagram() -> zoomReset();
}

/**
	Effectue l'action "selectionner tout" sur le schema en cours
*/
void QETDiagramEditor::slot_selectAll() {
	if(currentDiagram()) currentDiagram() -> selectAll();
}

/**
	Effectue l'action "deselectionenr tout" sur le schema en cours
*/
void QETDiagramEditor::slot_selectNothing() {
	if(currentDiagram()) currentDiagram() -> selectNothing();
}

/**
	Effectue l'action "inverser la selection" sur le schema en cours
*/
void QETDiagramEditor::slot_selectInvert() {
	if(currentDiagram()) currentDiagram() -> selectInvert();
}

/**
	Effectue l'action "supprimer" sur le schema en cours
*/
void QETDiagramEditor::slot_delete() {
	if(currentDiagram()) currentDiagram() -> deleteSelection();
}

/**
	Effectue l'action "pivoter" sur le schema en cours
*/
void QETDiagramEditor::slot_rotate() {
	if(currentDiagram()) currentDiagram() -> rotateSelection();
}

/**
	Effectue l'action "mode selection" sur le schema en cours
*/
void QETDiagramEditor::slot_setSelectionMode() {
	if(currentDiagram()) currentDiagram() -> setSelectionMode();
}

/**
	Effectue l'action "mode visualisation" sur le schema en cours
*/
void QETDiagramEditor::slot_setVisualisationMode() {
	if(currentDiagram()) currentDiagram() -> setVisualisationMode();
}

/**
	gere les actions
*/
void QETDiagramEditor::slot_updateActions() {
	DiagramView *dv = currentDiagram();
	ProjectView *pv = currentProject();
	bool opened_project = pv;
	bool opened_diagram = dv;
	bool editable_project = (pv && !pv -> project() -> isReadOnly());
	bool editable_diagram = (dv && !dv -> diagram() -> isReadOnly());
	
	// actions ayant juste besoin d'un document ouvert
	close_file        -> setEnabled(opened_project);
	save_file         -> setEnabled(opened_project && editable_project);
	save_file_sous    -> setEnabled(opened_project);
	save_all          -> setEnabled(opened_diagram && editable_diagram);
	prj_edit_prop     -> setEnabled(editable_project);
	prj_add_diagram   -> setEnabled(editable_project);
	prj_del_diagram   -> setEnabled(editable_project);
	prj_clean         -> setEnabled(editable_project);
	import_diagram    -> setEnabled(editable_project);
	export_diagram    -> setEnabled(opened_diagram);
	print             -> setEnabled(opened_diagram);
	select_all        -> setEnabled(opened_diagram);
	select_nothing    -> setEnabled(opened_diagram);
	select_invert     -> setEnabled(opened_diagram);
	zoom_in           -> setEnabled(opened_diagram);
	zoom_out          -> setEnabled(opened_diagram);
	zoom_fit          -> setEnabled(opened_diagram);
	zoom_reset        -> setEnabled(opened_diagram);
	conductor_default -> setEnabled(editable_diagram);
	infos_diagram     -> setEnabled(editable_diagram);
	add_text          -> setEnabled(editable_diagram);
	add_column        -> setEnabled(editable_diagram);
	remove_column     -> setEnabled(editable_diagram);
	add_row           -> setEnabled(editable_diagram);
	remove_row        -> setEnabled(editable_diagram);
	
	// affiche les actions correspondant au diagram view en cours
	if (dv) {
		if (can_update_actions) {
			undo_group.setActiveStack(&(dv -> diagram() -> undoStack()));
			undo -> setEnabled(undo_group.canUndo());
			redo -> setEnabled(undo_group.canRedo());
		}
	} else {
		undo -> setEnabled(false);
		redo -> setEnabled(false);
	}
	
	slot_updateFullScreenAction();
	slot_updateModeActions();
	slot_updatePasteAction();
	slot_updateComplexActions();
}

/**
	gere les actions ayant des besoins precis pour etre active ou non
	Cette methode ne fait rien si aucun document n'est ouvert
*/
void QETDiagramEditor::slot_updateComplexActions() {
	DiagramView *dv = currentDiagram();
	bool editable_diagram = (dv && !dv -> diagram() -> isReadOnly());
	
	// nombre de conducteurs selectionnes
	int selected_conductors_count = dv ? dv -> diagram() -> selectedConductors().count() : 0;
	conductor_reset  -> setEnabled(editable_diagram && selected_conductors_count);
	
	// actions ayant aussi besoin d'elements selectionnes
	bool selected_elements = dv ? (dv -> hasSelectedItems()) : false;
	cut              -> setEnabled(editable_diagram && selected_elements);
	copy             -> setEnabled(selected_elements);
	delete_selection -> setEnabled(editable_diagram && selected_elements);
	rotate_selection -> setEnabled(editable_diagram && selected_elements && dv -> diagram() -> canRotateSelection());
	selection_prop   -> setEnabled(editable_diagram && selected_elements);
}

/**
	Gere l'action permettant de passer en plein ecran ou d'en sortir
*/
void QETDiagramEditor::slot_updateFullScreenAction() {
	if (windowState() & Qt::WindowFullScreen) {
		fullscreen -> setText(tr("Sortir du &mode plein \351cran"));
		fullscreen -> setIcon(QET::Icons::FullScreenExit);
		fullscreen -> setStatusTip(tr("Affiche QElectroTech en mode fen\352tr\351", "status bar tip"));
	} else {
		fullscreen -> setText(tr("Passer en &mode plein \351cran"));
		fullscreen -> setIcon(QET::Icons::FullScreenEnter);
		fullscreen -> setStatusTip(tr("Affiche QElectroTech en mode plein \351cran", "status bar tip"));
	}
}

/**
	Gere les actions relatives au mode du schema
*/
void QETDiagramEditor::slot_updateModeActions() {
	DiagramView *dv = currentDiagram();
	
	// actions ayant aussi besoin d'un document ouvert et de la connaissance de son mode
	if (!dv) {
		grp_visu_sel -> setEnabled(false);
	} else {
		switch((int)(dv -> dragMode())) {
			case QGraphicsView::NoDrag:
				grp_visu_sel -> setEnabled(false);
				break;
			case QGraphicsView::ScrollHandDrag:
				grp_visu_sel -> setEnabled(true);
				mode_visualise -> setChecked(true);
				break;
			case QGraphicsView::RubberBandDrag:
				grp_visu_sel -> setEnabled(true);
				mode_selection -> setChecked(true);
				break;
		}
	}
}

/**
	Gere les actions ayant besoin du presse-papier
*/
void QETDiagramEditor::slot_updatePasteAction() {
	DiagramView *dv = currentDiagram();
	bool editable_diagram = (dv && !dv -> diagram() -> isReadOnly());
	
	// pour coller, il faut un schema ouvert et un schema dans le presse-papier
	paste -> setEnabled(editable_diagram && Diagram::clipboardMayContainDiagram());
}

/**
	Ajoute un projet dans l'espace de travail
	@param project_view Le projet a ajouter dans l'espace de travail
*/
void QETDiagramEditor::addProjectView(ProjectView *project_view) {
	if (!project_view) return;
	
	// on maximise la nouvelle fenetre si la fenetre en cours est inexistante ou bien maximisee
	QWidget *current_window = workspace.activeSubWindow();
	bool maximise = ((!current_window) || (current_window -> windowState() & Qt::WindowMaximized));
	
	// ajoute la fenetre
	QMdiSubWindow *sub_window = workspace.addSubWindow(project_view);
	sub_window -> setWindowIcon(project_view -> windowIcon());
	
	// lie les schemas du projet a l'editeur :
	// quand on change de schemas a l'interieur d'un projet, on met a jour les menus
	connect(project_view, SIGNAL(diagramActivated(DiagramView *)), this, SLOT(slot_updateWindowsMenu()));
	connect(project_view, SIGNAL(diagramActivated(DiagramView *)), this, SLOT(slot_updateActions()));
	foreach(DiagramView *dv, project_view -> diagrams()) {
		diagramWasAdded(dv);
	}
	
	// gere la fermeture du projet
	connect(project_view, SIGNAL(projectClosed(ProjectView*)), this, SLOT(projectWasClosed(ProjectView *)));
	
	// gere l'ajout et le retrait de schema du projet
	connect(project_view, SIGNAL(diagramAdded(DiagramView *)),   this, SLOT(diagramWasAdded(DiagramView *)));
	connect(project_view, SIGNAL(diagramAdded(DiagramView *)),   this, SLOT(slot_updateActions()));
	connect(project_view, SIGNAL(diagramAboutToBeRemoved(DiagramView *)), this, SLOT(diagramIsAboutToBeRemoved(DiagramView *)));
	connect(project_view, SIGNAL(diagramRemoved(DiagramView *)), this, SLOT(diagramWasRemoved(DiagramView *)));
	connect(project_view, SIGNAL(diagramRemoved(DiagramView *)), this, SLOT(slot_updateActions()));
	if (QETProject *project = project_view -> project()) {
		connect(project, SIGNAL(diagramAdded  (QETProject *, Diagram*)), &(pa -> elementsPanel()), SLOT(diagramWasAdded  (QETProject *, Diagram*)));
		connect(project, SIGNAL(diagramRemoved(QETProject *, Diagram*)), &(pa -> elementsPanel()), SLOT(diagramWasRemoved(QETProject *, Diagram*)));
		
		// on met aussi les menus a jour quand un projet passe en lecture seule ou non
		connect(project, SIGNAL(readOnlyChanged(QETProject *, bool)), this, SLOT(slot_updateActions()));
	}
	
	// gere les changements de l'ordre des schemas dans le projet
	connect(project_view, SIGNAL(diagramOrderChanged(ProjectView *, int, int)), this, SLOT(diagramOrderChanged(ProjectView *, int, int)));
	
	// gere les demandes consistant a retrouver un element dans le panel
	connect(project_view, SIGNAL(findElementRequired(const ElementsLocation &)), this, SLOT(findElementInPanel(const ElementsLocation &)));

	// gere les demandes pour l'edition d'un element
	connect(project_view, SIGNAL(editElementRequired(const ElementsLocation &)), this, SLOT(editElementInEditor(const ElementsLocation &)));
		
	// affiche la fenetre
	if (maximise) project_view -> showMaximized();
	else project_view -> show();
}

/**
	@return la liste des fichiers edites par cet editeur de schemas
*/
QList<QString> QETDiagramEditor::editedFiles() const {
	QList<QString> edited_files_list;
	foreach (ProjectView *project_view, openedProjects()) {
		QString diagram_file(project_view -> project() -> filePath());
		if (!diagram_file.isEmpty()) {
			edited_files_list << QFileInfo(diagram_file).canonicalFilePath();
		}
	}
	return(edited_files_list);
}

/**
	@param filepath Un chemin de fichier
	Note : si filepath est une chaine vide, cette methode retourne 0.
	@return le ProjectView editant le fichier filepath, ou 0 si ce fichier n'est
	pas edite par cet editeur de schemas.
*/
ProjectView *QETDiagramEditor::viewForFile(const QString &filepath) const {
	if (filepath.isEmpty()) return(0);
	
	QString searched_can_file_path = QFileInfo(filepath).canonicalFilePath();
	foreach (ProjectView *project_view, openedProjects()) {
		QString project_can_file_path = QFileInfo(project_view -> project() -> filePath()).canonicalFilePath();
		if (project_can_file_path == searched_can_file_path) {
			return(project_view);
		}
	}
	return(0);
}

/**
	met a jour le menu "Fenetres"
*/
void QETDiagramEditor::slot_updateWindowsMenu() {
	// nettoyage du menu
	foreach(QAction *a, windows_menu -> actions()) windows_menu -> removeAction(a);
	
	// actions de fermeture
	windows_menu -> addAction(close_file);
	//windows_menu -> addAction(closeAllAct);
	
	// actions de reorganisation des fenetres
	windows_menu -> addSeparator();
	windows_menu -> addAction(tile_window);
	windows_menu -> addAction(cascade_window);
	
	// actions de deplacement entre les fenetres
	windows_menu -> addSeparator();
	windows_menu -> addAction(next_window);
	windows_menu -> addAction(prev_window);
	
	// liste des fenetres
	QList<ProjectView *> windows = openedProjects();
	
	tile_window    -> setEnabled(!windows.isEmpty() && workspace.viewMode() == QMdiArea::SubWindowView);
	cascade_window -> setEnabled(!windows.isEmpty() && workspace.viewMode() == QMdiArea::SubWindowView);
	next_window    -> setEnabled(windows.count() > 1);
	prev_window    -> setEnabled(windows.count() > 1);
	
	if (!windows.isEmpty()) windows_menu -> addSeparator();
	QActionGroup *windows_actions = new QActionGroup(this);
	foreach(ProjectView *project_view, windows) {
		QString pv_title = project_view -> windowTitle();
		QAction *action  = windows_menu -> addAction(pv_title);
		windows_actions -> addAction(action);
		action -> setStatusTip(QString(tr("Active le projet \253\240%1\240\273")).arg(pv_title));
		action -> setCheckable(true);
		action -> setChecked(project_view == currentProject());
		connect(action, SIGNAL(triggered()), &windowMapper, SLOT(map()));
		windowMapper.setMapping(action, project_view);
	}
}

/**
	Edite les informations du schema en cours
*/
void QETDiagramEditor::editCurrentDiagramProperties() {
	if (ProjectView *project_view = currentProject()) {
		activateProject(project_view);
		project_view -> editCurrentDiagramProperties();
	}
}

/**
	Edite les proprietes du schema diagram
	@param diagram_view schema dont il faut editer les proprietes
*/
void QETDiagramEditor::editDiagramProperties(DiagramView *diagram_view) {
	if (ProjectView *project_view = findProject(diagram_view)) {
		activateProject(project_view);
		project_view -> editDiagramProperties(diagram_view);
	}
}

/**
	Edite les proprietes du schema diagram
	@param diagram schema dont il faut editer les proprietes
*/
void QETDiagramEditor::editDiagramProperties(Diagram *diagram) {
	if (ProjectView *project_view = findProject(diagram)) {
		activateProject(project_view);
		project_view -> editDiagramProperties(diagram);
	}
}

/**
	Ajoute une colonne au schema en cours
*/
void QETDiagramEditor::slot_addColumn() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> addColumn();
	}
}

/**
	Enleve une colonne au schema en cours
*/
void QETDiagramEditor::slot_removeColumn() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> removeColumn();
	}
}

/**
	Allonge le schema en cours en hauteur
*/
void QETDiagramEditor::slot_addRow() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> addRow();
	}
}

/**
	Retrecit le schema en cours en hauteur
*/
void QETDiagramEditor::slot_removeRow() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> removeRow();
	}
}

/**
	Edite les proprietes des objets selectionnes
*/
void QETDiagramEditor::editSelectionProperties() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> editSelectionProperties();
	}
}

/**
	Edite les proprietes du conducteur selectionne
*/
void QETDiagramEditor::slot_editConductor() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> editConductor();
	}
}

/**
	Reinitialise les conducteurs selectionnes
*/
void QETDiagramEditor::slot_resetConductors() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> resetConductors();
	}
}

/**
	Edite les proprietes par defaut des conducteurs
*/
void QETDiagramEditor::slot_editDefaultConductors() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> editDefaultConductorProperties();
	}
}

/**
	Ajoute un texte au schema courant
*/
void QETDiagramEditor::slot_addText() {
	if (DiagramView *dv = currentDiagram()) {
		dv -> addText();
	}
}

/**
	Affiche les projets dans des fenetres.
*/
void QETDiagramEditor::setWindowedMode() {
	workspace.setViewMode(QMdiArea::SubWindowView);
	windowed_view_mode -> setChecked(true);
	slot_updateWindowsMenu();
}

/**
	Affiche les projets dans des onglets.
*/
void QETDiagramEditor::setTabbedMode() {
	workspace.setViewMode(QMdiArea::TabbedView);
	tabbed_view_mode -> setChecked(true);
	slot_updateWindowsMenu();
}

/// Lit les parametres de l'editeur de schemas
void QETDiagramEditor::readSettings() {
	QSettings &settings = QETApp::settings();
	
	// dimensions et position de la fenetre
	QVariant geometry = settings.value("diagrameditor/geometry");
	if (geometry.isValid()) restoreGeometry(geometry.toByteArray());
	
	// etat de la fenetre (barres d'outils, docks...)
	QVariant state = settings.value("diagrameditor/state");
	if (state.isValid()) restoreState(state.toByteArray());
	
	// gestion des projets (onglets ou fenetres)
	bool tabbed = settings.value("diagrameditor/viewmode", "tabbed") == "tabbed";
	if (tabbed) {
		setTabbedMode();
	} else {
		setWindowedMode();
	}
}

/// Enregistre les parametres de l'editeur de schemas
void QETDiagramEditor::writeSettings() {
	QSettings &settings = QETApp::settings();
	settings.setValue("diagrameditor/geometry", saveGeometry());
	settings.setValue("diagrameditor/state", saveState());
}

/**
	Active le schema passe en parametre
	@param diagram Schema a activer
*/
void QETDiagramEditor::activateDiagram(Diagram *diagram) {
	if (QETProject *project = diagram -> project()) {
		if (ProjectView *project_view = findProject(project)) {
			activateWidget(project_view);
			project_view -> showDiagram(diagram);
		}
	} else {
		/// @todo gerer ce cas
	}
}

/**
	Active le projet passe en parametre
	@param project Projet a activer
*/
void QETDiagramEditor::activateProject(QETProject *project) {
	activateProject(findProject(project));
}

/**
	Active le projet passe en parametre
	@param project_view Projet a activer
*/
void QETDiagramEditor::activateProject(ProjectView *project_view) {
	if (!project_view) return;
	activateWidget(project_view);
}

/**
	Gere la fermeture d'une ProjectView
	@param project_view ProjectView fermee
*/
void QETDiagramEditor::projectWasClosed(ProjectView *project_view) {
	QETProject *project = project_view -> project();
	if (project) {
		pa -> elementsPanel().projectWasClosed(project);
		QETApp::unregisterProject(project);
	}
	project_view -> deleteLater();
	project -> deleteLater();
}

/**
	Edite les proprietes du projet courant.
*/
void QETDiagramEditor::editCurrentProjectProperties() {
	editProjectProperties(currentProject());
}

/**
	Edite les proprietes du projet project_view.
	@param project_view Vue sur le projet dont il faut editer les proprietes
*/
void QETDiagramEditor::editProjectProperties(ProjectView *project_view) {
	if (!project_view) return;
	activateProject(project_view);
	project_view -> editProjectProperties();
}

/**
	Edite les proprietes du projet project.
	@param project Projet dont il faut editer les proprietes
*/
void QETDiagramEditor::editProjectProperties(QETProject *project) {
	editProjectProperties(findProject(project));
}

/**
	Ajoute un nouveau schema au projet courant
*/
void QETDiagramEditor::addDiagramToProject() {
	if (ProjectView *current_project = currentProject()) {
		current_project -> addNewDiagram();
	}
}

/**
	Ajoute un nouveau schema a un projet
	@param project Projet auquel il faut ajouter un schema
*/
void QETDiagramEditor::addDiagramToProject(QETProject *project) {
	if (!project) return;
	
	// recupere le ProjectView visualisant ce projet
	if (ProjectView *project_view = findProject(project)) {
		
		// affiche le projet en question
		activateProject(project);
		
		// ajoute un schema au projet
		project_view -> addNewDiagram();
	}
}

/**
	Supprime un schema de son projet
	@param diagram Schema a supprimer
*/
void QETDiagramEditor::removeDiagram(Diagram *diagram) {
	if (!diagram) return;
	
	// recupere le projet contenant le schema
	if (QETProject *diagram_project = diagram -> project()) {
		// recupere la vue sur ce projet
		if (ProjectView *project_view = findProject(diagram_project)) {
			
			// affiche le schema en question
			project_view -> showDiagram(diagram);
			
			// supprime le schema
			project_view -> removeDiagram(diagram);
		}
	}
}

/**
	Nettoie le projet courant
*/
void QETDiagramEditor::cleanCurrentProject() {
	if (ProjectView *current_project = currentProject()) {
		int clean_count = current_project -> cleanProject();
		if (clean_count) pa -> reloadAndFilter();
	}
}

/**
	Supprime le schema courant du projet courant
*/
void QETDiagramEditor::removeDiagramFromProject() {
	if (ProjectView *current_project = currentProject()) {
		if (DiagramView *current_diagram = current_project -> currentDiagram()) {
			can_update_actions = false;
			current_project -> removeDiagram(current_diagram);
		}
	}
}

/**
	Gere l'ajout d'un schema dans un projet
	@param dv DiagramView concerne
*/
void QETDiagramEditor::diagramWasAdded(DiagramView *dv) {
	// quand on change qqc a l'interieur d'un schema, on met a jour les menus
	undo_group.addStack(&(dv -> diagram() -> undoStack()));
	connect(dv,              SIGNAL(selectionChanged()),         this,     SLOT(slot_updateComplexActions()));
	connect(dv,              SIGNAL(modeChanged()),              this,     SLOT(slot_updateModeActions()));
	connect(dv,              SIGNAL(textAdded(bool)),            add_text, SLOT(setChecked(bool)));
	connect(dv, SIGNAL(titleChanged(DiagramView *, const QString &)), this, SLOT(diagramTitleChanged(DiagramView *)));
}

/**
	Gere le retrait d'un schema dans un projet avant que le retrait ne soit effectif
	@param dv DiagramView concerne
*/
void QETDiagramEditor::diagramIsAboutToBeRemoved(DiagramView *dv) {
	undo_group.removeStack(&(dv -> diagram() -> undoStack()));
	can_update_actions = false;
}

/**
	Gere le retrait d'un schema dans un projet apres que le retrait soit effectif
	@param dv DiagramView concerne
*/
void QETDiagramEditor::diagramWasRemoved(DiagramView *) {
	can_update_actions = true;
}

/**
	Gere le changement de titre d'un schema dans un projet
	@param dv DiagramView concerne
*/
void QETDiagramEditor::diagramTitleChanged(DiagramView *dv) {
	if (Diagram *diagram = dv -> diagram()) {
		if (QETProject *project = diagram -> project()) {
			pa -> elementsPanel().diagramTitleChanged(project, diagram);
		}
	}
}

/**
	@param location Emplacement de l'element a retrouver dans le panel
	d'elements.
*/
void QETDiagramEditor::findElementInPanel(const ElementsLocation &location) {
	bool element_found = pa -> elementsPanel().scrollToElement(location);
	if (!element_found) {
		// l'element n'a pas ete trouve
		
		ElementsCollectionItem *element = QETApp::collectionItem(location);
		if (element) {
			// mais il semble exister tout de meme
			
			// peut-etre vient-il d'un projet ouvert dans un autre editeur ?
			if (location.project() && !findProject(location.project())) {
				statusBar() -> showMessage(
					tr("Impossible de retrouver cet \351l\351ment dans le panel car il semble \351dit\351 dans une autre fen\352tre"),
					10000
				);
			} else {
				// il devrait etre affiche : on tente de recharger le panel
				statusBar() -> showMessage(
					tr("Impossible de retrouver cet \351l\351ment dans le panel... rechargement du panel..."),
					10000
				);
				pa -> reloadAndFilter();
				statusBar() -> clearMessage();
				element_found = pa -> elementsPanel().scrollToElement(location);
			}
		}
	}
	
	if (!element_found) {
		statusBar() -> showMessage(
			tr("Impossible de retrouver cet \351l\351ment dans le panel"),
			10000
		);
	}
}


/**
	Lance l'editeur d'element pour l'element filename
	@param location Emplacement de l'element a editer
*/
void QETDiagramEditor::editElementInEditor(const ElementsLocation &location) {
	QETElementEditor *editor = new QETElementEditor();
	editor -> fromLocation(location);
	editor -> show();
}


/**
	@return Les proprietes par defaut pour le cartouche d'un schema
*/
InsetProperties QETDiagramEditor::defaultInsetProperties() {
	// accede a la configuration de l'application
	QSettings &settings = QETApp::settings();
	
	InsetProperties def;
	// lit le cartouche par defaut dans la configuration
	def.fromSettings(settings, "diagrameditor/default");
	
	return(def);
}

/**
	@return Les dimensions par defaut d'un schema
*/
BorderProperties QETDiagramEditor::defaultBorderProperties() {
	// accede a la configuration de l'application
	QSettings &settings = QETApp::settings();
	
	BorderProperties def;
	// lit les dimensions par defaut dans la configuration
	def.fromSettings(settings, "diagrameditor/default");
	
	return(def);
}

/**
	@return Les proprietes par defaut d'un conducteur
*/
ConductorProperties QETDiagramEditor::defaultConductorProperties() {
	// accede a la configuration de l'application
	QSettings &settings = QETApp::settings();
	
	ConductorProperties def;
	// lit les caracteristiques des conducteurs par defaut dans la configuration
	def.fromSettings(settings, "diagrameditor/defaultconductor");
	
	return(def);
}
