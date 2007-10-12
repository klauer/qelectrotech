#ifndef DIAGRAM_COMMANDS_H
#define DIAGRAM_COMMANDS_H
#include "qet.h"
#include "diagram.h"
#include "diagramtextitem.h"
#include "conductor.h"
#include <QtGui>
/**
	Cette classe represente l'action d'ajouter un element au schema
*/
class AddElementCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	AddElementCommand(Diagram *, Element *, const QPointF &, QUndoCommand * = 0);
	virtual ~AddElementCommand();
	private:
	AddElementCommand(const AddElementCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// element ajoute
	Element *element;
	/// schema sur lequel on ajoute l'element
	Diagram *diagram;
	/// position de l'element sur le schema
	QPointF position;
};

/**
	Cette classe represente l'action d'ajouter un conducteur au schema
*/
class AddConductorCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	AddConductorCommand(Diagram *, Conductor *, QUndoCommand * = 0);
	virtual ~AddConductorCommand();
	private:
	AddConductorCommand(const AddConductorCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// conducteur ajoute
	Conductor *conductor;
	/// schema auquel on ajoute le conducteur
	Diagram *diagram;
};

/**
	Cette classe represente l'action de supprimer des elements et / ou
	conducteurs d'un schema
*/
class DeleteElementsCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	DeleteElementsCommand(Diagram *, QSet<Element *>, QSet<Conductor *>, QUndoCommand * = 0);
	virtual ~DeleteElementsCommand();
	private:
	DeleteElementsCommand(const DeleteElementsCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// liste des elements enleves
	QSet<Element *> removed_elements;
	/// liste des conducteurs enleves
	QSet<Conductor *> removed_conductors;
	/// schema dont on supprime des elements et conducteurs
	Diagram *diagram;
};

/**
	Cette classe represente l'action de coller quelque chose sur un schema
*/
class PasteDiagramCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	PasteDiagramCommand(Diagram *, const QList<Element *> &, const QList<Conductor *> &, QUndoCommand * = 0);
	virtual ~PasteDiagramCommand();
	private:
	PasteDiagramCommand(const PasteDiagramCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// elements ajoutes
	QList<Element *> elements;
	/// conducteurs ajoutes
	QList<Conductor *> conductors;
	/// schema sur lequel on colle les elements et conducteurs
	Diagram *diagram;
	/// booleen pour empecher le premier appel a redo
	bool first_redo;
};

/**
	Cette classe represente l'action de supprimer des elements et / ou
	conducteurs d'un schema
*/
class CutDiagramCommand : public DeleteElementsCommand {
	// constructeurs, destructeur
	public:
	CutDiagramCommand(Diagram *, QSet<Element *>, QSet<Conductor *>, QUndoCommand * = 0);
	virtual ~CutDiagramCommand();
	private:
	CutDiagramCommand(const CutDiagramCommand &);
};

/**
	Cette classe represente l'action de deplacer des elements et des
	conducteurs sur un schema
*/
class MoveElementsCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	MoveElementsCommand(Diagram *, const QSet<Element *> &, const QSet<Conductor *> &, const QHash<Conductor *, Terminal *> &, const QPointF &m, QUndoCommand * = 0);
	virtual ~MoveElementsCommand();
	private:
	MoveElementsCommand(const MoveElementsCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	virtual void move(const QPointF &);
	
	// attributs
	private:
	/// schema sur lequel on deplace les elements
	Diagram *diagram;
	/// elements a deplacer
	QSet<Element *> elements_to_move;
	/// conducteurs a deplacer
	QSet<Conductor *> conductors_to_move;
	/// conducteurs a actualiser
	QHash<Conductor *, Terminal *> conductors_to_update;
	/// mouvement effectue
	QPointF movement;
	/// booleen pour ne pas executer le premier redo()
	bool first_redo;
};

/**
	Cette classe represente la modification d'un champ de texte
*/
class ChangeDiagramTextCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ChangeDiagramTextCommand(DiagramTextItem *, const QString &before, const QString &after, QUndoCommand * = 0);
	virtual ~ChangeDiagramTextCommand();
	private:
	ChangeDiagramTextCommand(const ChangeDiagramTextCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// DiagramTextItem modifie
	DiagramTextItem *text_item;
	/// texte avant changement
	QString text_before;
	/// texte apres changement
	QString text_after;
	/// booleen pour ne pas executer le premier redo()
	bool first_redo;
};

/**
	Cette classe represente l'action de pivoter plusieurs elements
*/
class RotateElementsCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	RotateElementsCommand(const QHash<Element *, QET::Orientation> &elements, QUndoCommand * = 0);
	virtual ~RotateElementsCommand();
	private:
	RotateElementsCommand(const RotateElementsCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// elements pivotes associes a leur ancienne orientation
	QHash<Element *, QET::Orientation> elements_to_rotate;
};

/**
	Cette classe represente l'action de modifier un conducteur
*/
class ChangeConductorCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ChangeConductorCommand(Conductor *, const ConductorProfile &, const ConductorProfile &, QUndoCommand * = 0);
	virtual ~ChangeConductorCommand();
	private:
	ChangeConductorCommand(const ChangeConductorCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// conducteur modifie
	Conductor *conductor;
	/// profil avant changement
	ConductorProfile old_profile;
	/// profil apres changement
	ConductorProfile new_profile;
	/// booleen pour ne pas executer le premier redo()
	bool first_redo;
};

/**
	Cette classe represente l'action de reinitialiser des conducteurs
*/
class ResetConductorCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ResetConductorCommand(const QHash<Conductor *, ConductorProfile> &, QUndoCommand * = 0);
	virtual ~ResetConductorCommand();
	private:
	ResetConductorCommand(const ResetConductorCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// conducteurs reinitialises associes a leur ancien profil
	QHash<Conductor *, ConductorProfile> conductors_profiles;
};

/**
	Cette classe represente l'action de modifier les informations du cartouche d'un schema
*/
class ChangeInsetCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ChangeInsetCommand(Diagram *, const InsetProperties &, const InsetProperties &, QUndoCommand * = 0);
	virtual ~ChangeInsetCommand();
	private:
	ChangeInsetCommand(const ChangeInsetCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	
	// attributs
	private:
	/// schema modifie
	Diagram *diagram;
	/// proprietes avant changement
	InsetProperties old_inset;
	/// proprietes apres changement
	InsetProperties new_inset;
};

/**
	Cette classe represente l'action de modifier :
	-le nombre de colonnes d'un schema
	-la hauteur des colonnes
	-la largeur des colonnes
	-la hauteur des en-tetes des colonnes
*/
class ChangeBorderCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ChangeBorderCommand(Diagram *, QUndoCommand * = 0);
	virtual ~ChangeBorderCommand();
	private:
	ChangeBorderCommand(const ChangeBorderCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	private:
	virtual void applyChanges(int = 1);
	
	// attributs
	private:
	/// schema modifie
	Diagram *diagram;
	public:
	/// nombre de colonnes ajoutees / enlevees
	int columnsCountDifference;
	/// delta pour la hauteur des colonnes
	qreal columnsHeightDifference;
	/// delta pour la largeur des colonnes
	qreal columnsWidthDifference;
	/// delta pour la hauteur des entetes des colonnes
	qreal headersHeightDifference;
};

/**
	Cette classe represente l'action de modifier les proprietes d'un conducteur
*/
class ChangeConductorPropertiesCommand : public QUndoCommand {
	// constructeurs, destructeur
	public:
	ChangeConductorPropertiesCommand(Conductor *, QUndoCommand * = 0);
	virtual ~ChangeConductorPropertiesCommand();
	private:
	ChangeConductorPropertiesCommand(const ChangeConductorPropertiesCommand &);
	
	// methodes
	public:
	virtual void undo();
	virtual void redo();
	virtual void setOldSettings(Conductor::ConductorType, const QString &, const SingleLineProperties &);
	virtual void setNewSettings(Conductor::ConductorType, const QString &, const SingleLineProperties &);
	
	// attributs
	private:
	/// conducteur modifie
	Conductor *conductor;
	/// anciennes proprietes
	Conductor::ConductorType old_type;
	QString old_conductor_text;
	SingleLineProperties old_slp;
	/// nouvelles proprietes
	Conductor::ConductorType new_type;
	QString new_conductor_text;
	SingleLineProperties new_slp;
	/// booleens indiquant si les proprietes ont ete definies ou non
	bool old_settings_set;
	bool new_settings_set;
};
#endif
