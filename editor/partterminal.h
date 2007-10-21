#ifndef PART_TERMINAL_H
#define PART_TERMINAL_H
#include "customelementpart.h"
#include "qet.h"
#include <QtGui>
class TerminalEditor;
class QETElementEditor;
/**
	Cette classe represente une borne pouvant etre utilisee pour composer le
	dessin d'un element dans l'editeur d'element.
*/
class PartTerminal : public CustomElementPart, public QGraphicsItem {
	public:
	// constructeurs, destructeur
	PartTerminal(QETElementEditor *, QGraphicsItem * = 0, QGraphicsScene * = 0);
	virtual ~PartTerminal();
	private:
	PartTerminal(const PartTerminal &);
	
	// attributs
	private:
	QET::Orientation _orientation;
	QPointF second_point;
	TerminalEditor *informations;
	
	// methodes
	public:
	enum { Type = UserType + 1106 };
	virtual int type() const { return Type; }
	virtual void fromXml(const QDomElement &);
	virtual const QDomElement toXml(QDomDocument &) const;
	virtual QWidget *elementInformations();
	virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
	virtual QRectF boundingRect() const;
	QET::Orientation orientation() const;
	void setOrientation(QET::Orientation);
	virtual void setProperty(const QString &, const QVariant &);
	virtual QVariant property(const QString &);
	
	protected:
	QVariant itemChange(GraphicsItemChange, const QVariant &);
	
	private:
	void updateSecondPoint();
};
#endif
