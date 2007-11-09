#include "diagramcommands.h"
#include "element.h"
#include "conductor.h"
#include "diagram.h"
#include "qgimanager.h"
/**
	Constructeur
	@param d Schema auquel on ajoute un element
	@param elmt Element ajoute
	@param p Position a laquelle l'element est ajoute
	@param parent QUndoCommand parent
*/
AddElementCommand::AddElementCommand(
	Diagram *d,
	Element *elmt,
	const QPointF &p,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("ajouter 1 ") + elmt -> nom(), parent),
	element(elmt),
	diagram(d),
	position(p)
{
	diagram -> qgiManager().manage(element);
}

/// Destructeur
AddElementCommand::~AddElementCommand() {
	diagram -> qgiManager().release(element);
}

/// Annule l'ajout
void AddElementCommand::undo() {
	diagram -> removeItem(element);
}

/// Refait l'ajout
void AddElementCommand::redo() {
	diagram -> addItem(element);
	element -> setPos(position);
	element -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

/**
	Constructeur
	@param dia Schema auquel on ajoute du texte
	@param elmt Texte ajoute
	@param p Position a laquelle le texte est ajoute
	@param parent QUndoCommand parent
*/
AddTextCommand::AddTextCommand(Diagram *dia, DiagramTextItem *text, const QPointF &pos, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("Ajouter un champ de texte"), parent),
	textitem(text),
	diagram(dia),
	position(pos)
{
	diagram -> qgiManager().manage(textitem);
}

/// Destructeur
AddTextCommand::~AddTextCommand() {
	diagram -> qgiManager().release(textitem);
}

/// Annule l'ajout
void AddTextCommand::undo() {
	diagram -> removeItem(textitem);
}

/// Refait l'ajour
void AddTextCommand::redo() {
	diagram -> addItem(textitem);
	textitem -> setPos(position);
}

/**
	Constructeur
	@param d Schema auquel on ajoute un conducteur
	@param c Conducteur ajoute
	@param parent QUndoCommand parent
*/
AddConductorCommand::AddConductorCommand(
	Diagram *d,
	Conductor *c,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("ajouter un conducteur"), parent),
	conductor(c),
	diagram(d)
{
	diagram -> qgiManager().manage(conductor);
}

/// Destructeur
AddConductorCommand::~AddConductorCommand() {
	diagram -> qgiManager().release(conductor);
}

/// Annule l'ajout
void AddConductorCommand::undo() {
	// detache le conducteur sans le detruire
	conductor -> terminal1 -> removeConductor(conductor);
	conductor -> terminal2 -> removeConductor(conductor);
	diagram -> removeItem(conductor);
}

/// Refait l'ajout
void AddConductorCommand::redo() {
	diagram -> addItem(conductor);
}

/**
	Constructeur
	@param dia Schema dont on supprime des elements et conducteurs
	@param content Contenu supprime
	@param parent QUndoCommand parent
*/
DeleteElementsCommand::DeleteElementsCommand(
	Diagram *dia,
	const DiagramContent &content,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	removed_content(content),
	diagram(dia)
{
	setText(QObject::tr("supprimer ") + removed_content.sentence(true));
	diagram -> qgiManager().manage(removed_content.items());
}

/// Destructeur
DeleteElementsCommand::~DeleteElementsCommand() {
	diagram -> qgiManager().release(removed_content.items());
}

/// annule les suppressions
void DeleteElementsCommand::undo() {
	// remet les elements
	foreach(Element *e, removed_content.elements) {
		diagram -> addItem(e);
	}
	
	// remet les conducteurs
	foreach(Conductor *c, removed_content.conductors()) {
		diagram -> addItem(c);
		c -> terminal1 -> addConductor(c);
		c -> terminal2 -> addConductor(c);
	}
	
	// remet les textes
	foreach(DiagramTextItem *t, removed_content.textFields) {
		diagram -> addItem(t);
	}
}

/// refait les suppressions
void DeleteElementsCommand::redo() {
	// enleve les conducteurs
	foreach(Conductor *c, removed_content.conductors()) {
		c -> terminal1 -> removeConductor(c);
		c -> terminal2 -> removeConductor(c);
		diagram -> removeItem(c);
	}
	
	// enleve les elements
	foreach(Element *e, removed_content.elements) {
		diagram -> removeItem(e);
	}
	
	// enleve les textes
	foreach(DiagramTextItem *t, removed_content.textFields) {
		diagram -> removeItem(t);
	}
}

/**
	Constructeur
	@param dia Schema sur lequel on colle les elements et conducteurs
	@param e Elements colles sur le schema
	@param c Conducteurs colles sur le schema
	@param t Textes colles sur le schema
	@param parent QUndoCommand parent
*/
PasteDiagramCommand::PasteDiagramCommand(
	Diagram *dia,
	const DiagramContent &c,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	content(c),
	diagram(dia),
	first_redo(true)
{
	setText(QObject::tr("coller ") + content.sentence());
	diagram -> qgiManager().manage(content.items());
}

/// Destructeur
PasteDiagramCommand::~PasteDiagramCommand() {
	diagram -> qgiManager().release(content.items());
}

/// annule le coller
void PasteDiagramCommand::undo() {
	// enleve les conducteurs
	foreach(Conductor *c, content.conductorsToMove) {
		c -> terminal1 -> removeConductor(c);
		c -> terminal2 -> removeConductor(c);
		diagram -> removeItem(c);
	}
	
	// enleve les elements
	foreach(Element *e, content.elements) diagram -> removeItem(e);
	
	// enleve les textes
	foreach(DiagramTextItem *t, content.textFields) diagram -> removeItem(t);
}

/// refait le coller
void PasteDiagramCommand::redo() {
	if (first_redo) first_redo = false;
	else {
		// pose les elements
		foreach(Element *e, content.elements)  diagram -> addItem(e);
		
		// pose les conducteurs
		foreach(Conductor *c, content.conductorsToMove) {
			diagram -> addItem(c);
			c -> terminal1 -> addConductor(c);
			c -> terminal2 -> addConductor(c);
		}
		
		// pose les textes
		foreach(DiagramTextItem *t, content.textFields) diagram -> addItem(t);
	}
	foreach(Element *e, content.elements) e -> setSelected(true);
	foreach(Conductor *c, content.conductorsToMove) c -> setSelected(true);
	foreach(DiagramTextItem *t, content.textFields) t -> setSelected(true);
}

/**
	Constructeur
	@param dia Schema dont on coupe des elements et conducteurs
	@param content Contenu coupe
	@param parent QUndoCommand parent
*/
CutDiagramCommand::CutDiagramCommand(
	Diagram *dia,
	const DiagramContent &content,
	QUndoCommand *parent
) : 
	DeleteElementsCommand(dia, content, parent)
{
	setText(QObject::tr("couper ") + content.sentence(true));
}

/// Destructeur
CutDiagramCommand::~CutDiagramCommand() {
}

/**
	Constructeur
	@param dia Schema sur lequel on deplace des elements
	@param diagram_content Contenu a deplacer
	@param m translation subie par les elements
	@param parent QUndoCommand parent
*/
MoveElementsCommand::MoveElementsCommand(
	Diagram *dia,
	const DiagramContent &diagram_content,
	const QPointF &m,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	diagram(dia),
	content_to_move(diagram_content),
	movement(m),
	first_redo(true)
{
	setText(QObject::tr("d\351placer ") + content_to_move.sentence());
}

/// Destructeur
MoveElementsCommand::~MoveElementsCommand() {
}

/// annule le deplacement
void MoveElementsCommand::undo() {
	move(-movement);
}

/// refait le deplacement
void MoveElementsCommand::redo() {
	if (first_redo) first_redo = false;
	else move(movement);
}

/**
	deplace les elements et conducteurs
	@param actual_movement translation a effectuer sur les elements et conducteurs
*/
void MoveElementsCommand::move(const QPointF &actual_movement) {
	// deplace les elements
	foreach(Element *element, content_to_move.elements) {
		element -> setPos(element -> pos() + actual_movement);
	}
	
	// deplace certains conducteurs
	foreach(Conductor *conductor, content_to_move.conductorsToMove) {
		conductor -> setPos(conductor -> pos() + actual_movement);
	}
	
	// recalcule les autres conducteurs
	foreach(Conductor *conductor, content_to_move.conductorsToUpdate.keys()) {
		conductor -> updateWithNewPos(
			QRectF(),
			content_to_move.conductorsToUpdate[conductor],
			content_to_move.conductorsToUpdate[conductor] -> amarrageConductor()
		);
	}
	
	// deplace les textes
	foreach(DiagramTextItem *text, content_to_move.textFields) {
		text -> setPos(text -> pos() + actual_movement);
	}
}

/**
	Constructeur
	@param dti Champ de texte modifie
	@param before texte avant
	@param after texte apres
	@param parent QUndoCommand parent
*/
ChangeDiagramTextCommand::ChangeDiagramTextCommand(
	DiagramTextItem *dti,
	const QString &before,
	const QString &after,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier le texte"), parent),
	text_item(dti),
	text_before(before),
	text_after(after),
	first_redo(true)
{
}

/// destructeur
ChangeDiagramTextCommand::~ChangeDiagramTextCommand() {
}

/// annule la modification de texte
void ChangeDiagramTextCommand::undo() {
	text_item -> setPlainText(text_before);
	text_item -> previous_text = text_before;
}

/// refait la modification de texte
void ChangeDiagramTextCommand::redo() {
	if (first_redo) first_redo = false;
	else {
		text_item -> setPlainText(text_after);
		text_item -> previous_text = text_after;
	}
}

/**
	Constructeur
	@param elements Elements a pivoter associes a leur orientation d'origine
	@param parent QUndoCommand parent
*/
RotateElementsCommand::RotateElementsCommand(const QHash<Element *, QET::Orientation> &elements, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("pivoter ") + QET::ElementsAndConductorsSentence(elements.count(), 0), parent),
	elements_to_rotate(elements)
{
}

/// Destructeur
RotateElementsCommand::~RotateElementsCommand() {
}

/// defait le pivotement
void RotateElementsCommand::undo() {
	foreach(Element *e, elements_to_rotate.keys()) {
		e -> setOrientation(elements_to_rotate[e]);
	}
}

/// refait le pivotement
void RotateElementsCommand::redo() {
	foreach(Element *e, elements_to_rotate.keys()) {
		e -> setOrientation(e -> orientation().next());
		e -> update();
	}
}

/**
	Constructeur
	@param c Conducteur modifie
	@param old_p ancien profil du conducteur
	@param new_p nouveau profil du conducteur
	@param parent QUndoCommand parent
*/
ChangeConductorCommand::ChangeConductorCommand(
	Conductor *c,
	const ConductorProfile &old_p,
	const ConductorProfile &new_p,
	Qt::Corner path_t,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier un conducteur"), parent),
	conductor(c),
	old_profile(old_p),
	new_profile(new_p),
	path_type(path_t),
	first_redo(true)
{
}

/// Destructeur
ChangeConductorCommand::~ChangeConductorCommand() {
}

/// Annule la modification du conducteur
void ChangeConductorCommand::undo() {
	conductor -> setProfile(old_profile, path_type);
}

/// Refait la modification du conducteur
void ChangeConductorCommand::redo() {
	if (first_redo) first_redo = false;
	else conductor -> setProfile(new_profile, path_type);
}

/**
	Constructeur
	@param cp Conducteurs reinitialises, associes a leur ancien profil
	@param parent QUndoCommand parent
*/
ResetConductorCommand::ResetConductorCommand(
	const QHash<Conductor *, ConductorProfilesGroup> &cp,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("R\351initialiser ") + QET::ElementsAndConductorsSentence(0, cp.count()), parent),
	conductors_profiles(cp)
{
}

/// Destructeur
ResetConductorCommand::~ResetConductorCommand() {
}

/// Annule la reinitialisation des conducteurs
void ResetConductorCommand::undo() {
	foreach(Conductor *c, conductors_profiles.keys()) {
		c -> setProfiles(conductors_profiles[c]);
	}
}

/// Refait la reinitialisation des conducteurs
void ResetConductorCommand::redo() {
	foreach(Conductor *c, conductors_profiles.keys()) {
		c -> setProfiles(ConductorProfilesGroup());
	}
}

/**
	Constructeur
	@param d Schema dont on modifie le cartouche
	@param old_ip Anciennes proprietes du cartouche
	@param new_ip Nouvelles proprietes du cartouche
	@param parent QUndoCommand parent
*/
ChangeInsetCommand::ChangeInsetCommand(
	Diagram *d,
	const InsetProperties &old_ip,
	const InsetProperties &new_ip,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier le cartouche"), parent),
	diagram(d),
	old_inset(old_ip),
	new_inset(new_ip)
{
}

/// Destructeur
ChangeInsetCommand::~ChangeInsetCommand() {
}

/// Annule la modification de cartouche
void ChangeInsetCommand::undo() {
	diagram -> border_and_inset.importInset(old_inset);
	diagram -> invalidate(diagram -> border());
}

/// Refait la modification de cartouche
void ChangeInsetCommand::redo() {
	diagram -> border_and_inset.importInset(new_inset);
	diagram -> invalidate(diagram -> border());
}

/**
	Constructeur
	@param dia Schema modifie
	@param parent QUndoCommand parent
*/
ChangeBorderCommand::ChangeBorderCommand(Diagram *dia, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("modifier les dimensions du sch\351ma"), parent),
	diagram(dia),
	columnsCountDifference(0),
	columnsHeightDifference(0.0),
	columnsWidthDifference(0.0),
	headersHeightDifference(0.0)
{
}

/// Destructeur
ChangeBorderCommand::~ChangeBorderCommand() {
}

/**
	Applique les changements au schema
	@param coeff comme les changements s'expriment sous forme de nombres dont
	il suffit d'inverser le signe pour les annuler, ces valeurs sont ici
	multipliees par le coefficient passe en parametre avant d'etre appliquees.
	Pour resumer : 1 pour refaire, -1 pour annuler.
*/
void ChangeBorderCommand::applyChanges(int coeff) {
	// reference vers l'objet border_and_inset du schema
	BorderInset &border = diagram -> border_and_inset;
	if (columnsCountDifference) {
		border.setNbColumns(border.nbColumn() + (columnsCountDifference * coeff));
	}
	if (columnsHeightDifference) {
		border.setColumnsHeight(border.columnsHeight() + (columnsHeightDifference * coeff));
	}
	if (columnsWidthDifference) {
		border.setColumnsWidth(border.columnsWidth() + (columnsWidthDifference * coeff));
	}
	if (headersHeightDifference) {
		border.setColumnsHeaderHeight(border.columnsHeaderHeight() + (headersHeightDifference * coeff));
	}
	border.adjustInsetToColumns();
}

/// Annule les changements apportes au schema
void ChangeBorderCommand::undo() {
	applyChanges(-1);
}

/// Refait les changements apportes au schema
void ChangeBorderCommand::redo() {
	applyChanges(1);
}

/**
	Constructeur
	@param c Le conducteur dont on modifie les proprietes
	@param parent QUndoCommand parent
*/
ChangeConductorPropertiesCommand::ChangeConductorPropertiesCommand(Conductor *c, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("modifier les propri\351t\351s d'un conducteur"), parent),
	conductor(c),
	old_settings_set(false),
	new_settings_set(false)
{
}

/// Destructeur
ChangeConductorPropertiesCommand::~ChangeConductorPropertiesCommand() {
}

/// definit l'ancienne configuration
void ChangeConductorPropertiesCommand::setOldSettings(const ConductorProperties &properties) {
	old_properties = properties;
	old_settings_set = true;
}

/// definit la nouvelle configuration
void ChangeConductorPropertiesCommand::setNewSettings(const ConductorProperties &properties) {
	new_properties = properties;
	new_settings_set = true;
}

/**
	Annule les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeConductorPropertiesCommand::undo() {
	if (old_settings_set && new_settings_set) {
		conductor -> setProperties(old_properties);
		conductor -> update();
	}
}

/**
	Refait les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeConductorPropertiesCommand::redo() {
	if (old_settings_set && new_settings_set) {
		conductor -> setProperties(new_properties);
		conductor -> update();
	}
}
