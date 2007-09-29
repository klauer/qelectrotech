#include "diagramview.h"
#include "diagram.h"
#include "customelement.h"
#include "exportdialog.h"
#include "conducer.h"
#include "diagramcommands.h"

/**
	Constructeur
	@param parent Le QWidegt parent de cette vue de schema
*/
DiagramView::DiagramView(QWidget *parent) : QGraphicsView(parent) {
	setInteractive(true);
	setCacheMode(QGraphicsView::CacheBackground);
	setOptimizationFlags(QGraphicsView::DontClipPainter|QGraphicsView::DontSavePainterState|QGraphicsView::DontAdjustForAntialiasing);
	
	// active l'antialiasing
	setRenderHint(QPainter::Antialiasing, true);
	setRenderHint(QPainter::TextAntialiasing, true);
	setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	setScene(scene = new Diagram(this));
	scene -> undoStack().setClean();
	setDragMode(RubberBandDrag);
	setAcceptDrops(true);
	setWindowIcon(QIcon(":/ico/qet-16.png"));
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	adjustSceneRect();
	updateWindowTitle();
	
	connect(scene, SIGNAL(selectionEmptinessChanged()), this, SLOT(slot_selectionChanged()));
	connect(&(scene -> border_and_inset), SIGNAL(borderChanged(QRectF, QRectF)), this, SLOT(adjustSceneRect()));
	connect(&(scene -> undoStack()), SIGNAL(cleanChanged(bool)), this, SLOT(updateWindowTitle()));
}

/**
	Destructeur
*/
DiagramView::~DiagramView() {
}

/**
	appelle la methode select sur tous les elements de la liste d'elements
*/
void DiagramView::selectAll() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(true);
}

/**
	appelle la methode deselect sur tous les elements de la liste d'elements
*/
void DiagramView::selectNothing() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(false);
}

/**
	Inverse l'etat de selection de tous les elements de la liste d'elements
 */
void DiagramView::selectInvert() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(!item -> isSelected());
}

/**
	Supprime les composants selectionnes
*/
void DiagramView::deleteSelection() {
	
	QSet<Element *> garbage_elmt;
	QSet<Conducer *> garbage_conducers;
	
	// creation de deux listes : une pour les conducteurs, une pour les elements
	foreach (QGraphicsItem *qgi, scene -> selectedItems()) {
		// pour chaque qgi selectionne, il s'agit soit d'un element soit d'un conducteur
		if (Conducer * c = qgraphicsitem_cast<Conducer *>(qgi)) {
			// s'il s'agit d'un conducteur, on le met dans la liste des conducteurs
			garbage_conducers << c;
		} else if (Element *e = qgraphicsitem_cast<Element *>(qgi)) {
			garbage_elmt << e;
			// s'il s'agit d'un element, on veille a enlever ses conducteurs
			garbage_conducers += e -> conducers().toSet();
		}
	}
	scene -> clearSelection();
	scene -> undoStack().push(new DeleteElementsCommand(scene, garbage_elmt, garbage_conducers));
}

/**
	Pivote les composants selectionnes
*/
void DiagramView::rotateSelection() {
	if (scene -> selectedItems().isEmpty()) return;
	QHash<Element *, QET::Orientation> elements_to_rotate;
	foreach (QGraphicsItem *item, scene -> selectedItems()) {
		if (Element *e = qgraphicsitem_cast<Element *>(item)) {
			elements_to_rotate.insert(e, e -> orientation().current());
		}
	}
	scene -> undoStack().push(new RotateElementsCommand(elements_to_rotate));
}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragEnterEvent correspondant au drag'n drop tente
*/
void DiagramView::dragEnterEvent(QDragEnterEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les dragleaveevent
	@param e le QDragEnterEvent correspondant au drag'n drop sortant
*/
void DiagramView::dragLeaveEvent(QDragLeaveEvent *) {
}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragMoveEvent correspondant au drag'n drop tente
*/
void DiagramView::dragMoveEvent(QDragMoveEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les depots (drop) acceptes sur le Diagram
	@param e le QDropEvent correspondant au drag'n drop effectue
*/
void DiagramView::dropEvent(QDropEvent *e) {
	QString fichier = e -> mimeData() -> text();
	int etat;
	Element *el = new CustomElement(fichier, 0, 0, &etat);
	if (etat) delete el;
	else {
		diagram() -> undoStack().push(new AddElementCommand(diagram(), el, mapToScene(e -> pos())));
	}
}

/**
	Passe le Diagram en mode visualisation
*/
void DiagramView::setVisualisationMode() {
	setDragMode(ScrollHandDrag);
	setInteractive(false);
	emit(modeChanged());
}

/**
	Passe le Diagram en mode Selection
*/
void DiagramView::setSelectionMode() {
	setDragMode(RubberBandDrag);
	setInteractive(true);
	emit(modeChanged());
}

/**
	Agrandit le schema (+33% = inverse des -25 % de zoomMoins())
*/
void DiagramView::zoomIn() {
	scale(4.0/3.0, 4.0/3.0);
	adjustGridToZoom();
}

/**
	Retrecit le schema (-25% = inverse des +33 % de zoomPlus())
*/
void DiagramView::zoomOut() {
	scale(0.75, 0.75);
	adjustGridToZoom();
}

/**
	Agrandit ou rectrecit le schema de facon a ce que tous les elements du
	schema soient visibles a l'ecran. S'il n'y a aucun element sur le schema,
	le zoom est reinitialise
*/
void DiagramView::zoomFit() {
	if (scene -> items().isEmpty()) {
		zoomReset();
		return;
	}
	adjustSceneRect();
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	adjustGridToZoom();
}

/**
	Reinitialise le zoom
*/
void DiagramView::zoomReset() {
	resetMatrix();
	adjustGridToZoom();
}

/**
	copie les elements selectionnes du schema dans le presse-papier puis les supprime
*/
void DiagramView::cut() {
	copy();
	QSet<Element *> cut_elmt;
	QSet<Conducer *> cut_conducers;
	
	// creation de deux listes : une pour les conducteurs, une pour les elements
	foreach (QGraphicsItem *qgi, scene -> selectedItems()) {
		// pour chaque qgi selectionne, il s'agit soit d'un element soit d'un conducteur
		if (Conducer * c = qgraphicsitem_cast<Conducer *>(qgi)) {
			// s'il s'agit d'un conducteur, on le met dans la liste des conducteurs
			cut_conducers << c;
		} else if (Element *e = qgraphicsitem_cast<Element *>(qgi)) {
			cut_elmt << e;
			// s'il s'agit d'un element, on veille a enlever ses conducteurs
			cut_conducers += e -> conducers().toSet();
		}
	}
	scene -> clearSelection();
	scene -> undoStack().push(new CutDiagramCommand(scene, cut_elmt, cut_conducers));
}

/**
	copie les elements selectionnes du schema dans le presse-papier
*/
void DiagramView::copy() {
	QClipboard *presse_papier = QApplication::clipboard();
	QString contenu_presse_papier = scene -> toXml(false).toString(4);
	if (presse_papier -> supportsSelection()) presse_papier -> setText(contenu_presse_papier, QClipboard::Selection);
	presse_papier -> setText(contenu_presse_papier);
}

/**
	importe les elements contenus dans le presse-papier dans le schema
*/
void DiagramView::paste() {
	QString texte_presse_papier;
	QDomDocument document_xml;
	if ((texte_presse_papier = QApplication::clipboard() -> text()) == QString()) return;
	if (!document_xml.setContent(texte_presse_papier)) return;
	
	// listes pour recupere les elements et conducteurs ajoutes au schema par le coller
	QList<Element *> elements_pasted;
	QList<Conducer *> conducers_pasted;
	scene -> fromXml(document_xml, QPointF(), false, &elements_pasted, &conducers_pasted);
	
	// si quelque chose a effectivement ete ajoute au schema, on cree
	if (elements_pasted.count() || conducers_pasted.count()) {
		scene -> clearSelection();
		scene -> undoStack().push(new PasteDiagramCommand(scene, elements_pasted, conducers_pasted));
	}
}

/**
	gere les clics et plus particulierement le clic du milieu (= coller pour X11)
*/
void DiagramView::mousePressEvent(QMouseEvent *e) {
	if (e -> buttons() == Qt::MidButton) {
		QString texte_presse_papier;
		QDomDocument document_xml;
		if ((texte_presse_papier = QApplication::clipboard() -> text(QClipboard::Selection)) == QString()) return;
		if (!document_xml.setContent(texte_presse_papier)) return;
		
		// listes pour recupere les elements et conducteurs ajoutes au schema par le coller
		QList<Element *> elements_pasted;
		QList<Conducer *> conducers_pasted;
		scene -> fromXml(document_xml, mapToScene(e -> pos()), false, &elements_pasted, &conducers_pasted);
		
		// si quelque chose a effectivement ete ajoute au schema, on cree
		if (elements_pasted.count() || conducers_pasted.count()) {
			scene -> clearSelection();
			scene -> undoStack().push(new PasteDiagramCommand(scene, elements_pasted, conducers_pasted));
		}
	} else {
		QGraphicsView::mousePressEvent(e);
	}
}

/**
	Ouvre un fichier *.qet dans cette DiagramView
	@param n_fichier Nom du fichier a ouvrir
	@param erreur Si le pointeur est specifie, cet entier est mis a 0 en cas de reussite de l'ouverture, 1 si le fichier n'existe pas, 2 si le fichier n'est pas lisible, 3 si le fichier n'est pas un element XML, 4 si l'ouverture du fichier a echoue pour une autre raison (c'est pas ca qui manque ^^)
	@return true si l'ouverture a reussi, false sinon
*/
bool DiagramView::open(QString n_fichier, int *erreur) {
	// verifie l'existence du fichier
	if (!QFileInfo(n_fichier).exists()) {
		if (erreur != NULL) *erreur = 1;
		return(false);
	}
	
	// ouvre le fichier
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::ReadOnly)) {
		if (erreur != NULL) *erreur = 2;
		return(false);
	}
	
	// lit son contenu dans un QDomDocument
	QDomDocument document;
	if (!document.setContent(&fichier)) {
		if (erreur != NULL) *erreur = 3;
		fichier.close();
		return(false);
	}
	fichier.close();
	
	// construit le schema a partir du QDomDocument
	QDomDocument &doc = document;
	if (scene -> fromXml(doc)) {
		if (erreur != NULL) *erreur = 0;
		file_name = n_fichier;
		scene -> undoStack().setClean();
		return(true);
	} else {
		if (erreur != NULL) *erreur = 4;
		return(false);
	}
}

/**
	Slot appele lorsque la selection change.
*/
void DiagramView::slot_selectionChanged() {
	emit(selectionChanged());
}

/**
	Gere la fermeture du schema.
	@param event Le QCloseEvent decrivant l'evenement
*/
void DiagramView::closeEvent(QCloseEvent *event) {
	bool retour;
	// si le schema est modifie
	if (!isWindowModified()) {
		retour = true;
	} else {
		// demande d'abord a l'utilisateur s'il veut enregistrer le schema en cours
		QMessageBox::StandardButton reponse = QMessageBox::question(
			this,
			tr("Enregistrer le sch\351ma en cours ?"),
			tr("Voulez-vous enregistrer le sch\351ma ") + windowTitle() + tr(" ?"),
			QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
			QMessageBox::Cancel
		);
		switch(reponse) {
			case QMessageBox::Cancel: retour = false;         break; // l'utilisateur annule : echec de la fermeture
			case QMessageBox::Yes:    retour = save(); break; // l'utilisateur dit oui : la reussite depend de l'enregistrement
			default:                  retour = true;                 // l'utilisateur dit non ou ferme le dialogue: c'est reussi
		}
	}
	if (retour) {
		event -> accept();
		delete this;
	} else event -> ignore();
	
}

/**
	Methode enregistrant le schema dans le dernier nom de fichier connu.
	Si aucun nom de fichier n'est connu, cette methode appelle la methode saveAs
	@return true si l'enregistrement a reussi, false sinon
*/
bool DiagramView::save() {
	if (file_name == QString()) return(saveAs());
	else return(saveDiagramToFile(file_name));
}

/**
	Cette methode demande un nom de fichier a l'utilisateur pour enregistrer le schema
	Si aucun nom n'est entre, elle renvoie faux.
	Si le nom ne se termine pas par l'extension .qet, celle-ci est ajoutee.
	Si l'enregistrement reussit, le nom du fichier est conserve et la fonction renvoie true.
	Sinon, faux est renvoye.
	@return true si l'enregistrement a reussi, false sinon
	@todo detecter le chemin du bureau automatiquement
*/
bool DiagramView::saveAs() {
	// demande un nom de fichier a l'utilisateur pour enregistrer le schema
	QString n_fichier = QFileDialog::getSaveFileName(
		this,
		tr("Enregistrer sous"),
		QDir::homePath(),
		tr("Sch\351ma QElectroTech (*.qet)")
	);
	// si aucun nom n'est entre, renvoie faux.
	if (n_fichier == "") return(false);
	// si le nom ne se termine pas par l'extension .qet, celle-ci est ajoutee
	if (!n_fichier.endsWith(".qet", Qt::CaseInsensitive)) n_fichier += ".qet";
	// tente d'enregistrer le fichier
	bool resultat_enregistrement = saveDiagramToFile(n_fichier);
	// si l'enregistrement reussit, le nom du fichier est conserve
	if (resultat_enregistrement) {
		file_name = n_fichier;
		updateWindowTitle();
	}
	// retourne un booleen representatif de la reussite de l'enregistrement
	return(resultat_enregistrement);
}

/**
	Methode privee gerant l'enregistrement du fichier XML. S'il n'est pas possible
	d'ecrire dans le fichier, cette fonction affiche un message d'erreur et renvoie false.
	Autrement, elle renvoie true.
	@param n_fichier Nom du fichier dans lequel l'arbre XML doit etre ecrit
	@return true si l'enregistrement a reussi, false sinon
*/
bool DiagramView::saveDiagramToFile(QString &n_fichier) {
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'ecrire dans ce fichier"));
		return(false);
	}
	QTextStream out(&fichier);
	out.setCodec("UTF-8");
	out << scene -> toXml().toString(4);
	fichier.close();
	scene -> undoStack().setClean();
	return(true);
}

/**
	Exporte le schema.
*/
void DiagramView::dialogExport() {
	ExportDialog ed(scene, this);
	ed.exec();
}

/**
	Imprime le schema.
*/
void DiagramView::dialogPrint() {
	QPrinter qprin;
	qprin.setOutputFormat(QPrinter::PdfFormat);
	qprin.setOrientation(QPrinter::Landscape);
	qprin.setPageSize(QPrinter::A4);
#ifdef Q_WS_X11
// 	qprin.setPrintProgram("lpr");
#endif
	QPrintDialog qpd(&qprin, this);
	
	if (qpd.exec() == QDialog::Accepted) {
		QPainter qp(&qprin);
		scene -> setDisplayGrid(false);
		scene -> render(&qp);
		scene -> setDisplayGrid(true);
	}
}

/**
	Edite les informations du schema.
*/
void DiagramView::dialogEditInfos() {
	// recupere le cartouche du schema
	InsetProperties inset = scene -> border_and_inset.exportInset();
	
	// construit le dialogue
	QDialog popup;
	popup.setMinimumWidth(400);
	popup.setWindowTitle(tr("Cartouche du sch\351ma"));
	
	QLineEdit *titre = new QLineEdit(inset.title, &popup);
	QLineEdit *auteur = new QLineEdit(inset.author, &popup);
	QDate date_diagram = QDate(inset.date);
	if (date_diagram.isNull() || !date_diagram.isValid()) date_diagram = QDate::currentDate();
	QDateEdit *date = new QDateEdit(date_diagram, &popup);
	date -> setCalendarPopup(true);
	QLineEdit *fichier = new QLineEdit(inset.filename, &popup);
	QLineEdit *folio = new QLineEdit(inset.folio, &popup);
	QWidget bidon(&popup);
	QGridLayout layout_champs(&bidon);
	layout_champs.addWidget(new QLabel(tr("Titre : ")),   0, 0);
	layout_champs.addWidget(titre,                        0, 1);
	layout_champs.addWidget(new QLabel(tr("Auteur : ")),  1, 0);
	layout_champs.addWidget(auteur,                       1, 1);
	layout_champs.addWidget(new QLabel(tr("Date : ")),    2, 0);
	layout_champs.addWidget(date,                         2, 1);
	layout_champs.addWidget(new QLabel(tr("Fichier : ")), 3, 0);
	layout_champs.addWidget(fichier,                      3, 1);
	layout_champs.addWidget(new QLabel(tr("Folio : ")),   4, 0);
	layout_champs.addWidget(folio,                        4, 1);
	
	// boutons
	QDialogButtonBox boutons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(&boutons, SIGNAL(accepted()), &popup, SLOT(accept()));
	connect(&boutons, SIGNAL(rejected()), &popup, SLOT(accept()));
	
	// ajout dans une disposition verticale
	QVBoxLayout layout_v(&popup);
	layout_v.addWidget(&bidon);
	layout_v.addWidget(&boutons);
	// si le dialogue est accepte
	if (popup.exec() == QDialog::Accepted) {
		InsetProperties new_inset;
		new_inset.title = titre -> text();
		new_inset.author = auteur -> text();
		new_inset.date = date -> date();
		new_inset.filename = fichier -> text();
		new_inset.folio = folio -> text();
		
		// s'il y a des modifications
		if (new_inset != inset) {
			scene -> undoStack().push(new ChangeInsetCommand(scene, inset, new_inset));
		}
	}
}

/**
	@return true s'il y a des elements selectionnes sur le schema, false sinon
*/
bool DiagramView::hasSelectedItems() {
	return(scene -> selectedItems().size() > 0);
}

/**
	Ajoute une colonne au schema.
*/
void DiagramView::addColumn() {
	ChangeBorderCommand *cbc = new ChangeBorderCommand(scene);
	cbc -> columnsCountDifference = 1;
	scene -> undoStack().push(cbc);
}

/**
	Enleve une colonne au schema.
*/
void DiagramView::removeColumn() {
	ChangeBorderCommand *cbc = new ChangeBorderCommand(scene);
	cbc -> columnsCountDifference = -1;
	scene -> undoStack().push(cbc);
}

/**
	Agrandit le schema en hauteur
*/
void DiagramView::expand() {
	ChangeBorderCommand *cbc = new ChangeBorderCommand(scene);
	cbc -> columnsHeightDifference = 20.0;
	scene -> undoStack().push(cbc);
}

/**
	Retrecit le schema en hauteur
*/
void DiagramView::shrink() {
	ChangeBorderCommand *cbc = new ChangeBorderCommand(scene);
	cbc -> columnsHeightDifference = 20.0;
	scene -> undoStack().push(cbc);
}

/**
	Ajuste le sceneRect (zone du schema visualisee par le DiagramView) afin que
	celui inclut a la fois les elements dans et en dehors du cadre et le cadre
	lui-meme.
*/
void DiagramView::adjustSceneRect() {
	QRectF old_scene_rect = sceneRect();
	
	// rectangle delimitant l'ensemble des elements
	QRectF elements_bounding_rect = scene -> itemsBoundingRect();
	
	// rectangle contenant le cadre = colonnes + cartouche
	QRectF border_bounding_rect = scene -> border().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN);
	
	// ajuste la sceneRect
	QRectF new_scene_rect = elements_bounding_rect.united(border_bounding_rect);
	setSceneRect(new_scene_rect);
	
	// met a jour la scene
	scene -> update(old_scene_rect.united(new_scene_rect));
}

void DiagramView::updateWindowTitle() {
	QString window_title;
	if (file_name.isNull()) window_title += tr("nouveau sch\351ma");
	else window_title += file_name;
	window_title += "[*]";
	setWindowTitle(window_title);
	setWindowModified(!(scene -> undoStack().isClean()));
}

void DiagramView::adjustGridToZoom() {
	QRectF viewed_scene = viewedSceneRect();
	scene -> setDisplayGrid(viewed_scene.width() < 2000 || viewed_scene.height() < 2000);
}

QRectF DiagramView::viewedSceneRect() const {
	// recupere la taille du widget viewport
	QSize viewport_size = viewport() -> size();
	
	// recupere la transformation viewport -> scene
	QTransform view_to_scene   = viewportTransform().inverted();
	
	// mappe le coin superieur gauche et le coin inferieur droit de la viewport sur la scene
	QPointF scene_left_top     = view_to_scene.map(QPointF(0.0, 0.0));
	QPointF scene_right_bottom = view_to_scene.map(QPointF(viewport_size.width(), viewport_size.height()));
	
	// en deduit le rectangle visualise par la scene
	return(QRectF(scene_left_top, scene_right_bottom));
}
