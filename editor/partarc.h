#ifndef PART_ARC_H
#define PART_ARC_H
#include <QtGui>
#include "customelementgraphicpart.h"
class ArcEditor;
class PartArc : public QGraphicsEllipseItem, public CustomElementGraphicPart {
	// constructeurs, destructeur
	public:
	PartArc(QGraphicsItem * = 0, QGraphicsScene * = 0);
	virtual ~PartArc() {
		qDebug() << "~PartArc()";
	}
	
	private:
	PartArc(const PartArc &);
	
	// attributs
	private:
	ArcEditor *informations;
	int _angle;
	int start_angle;
	
	// methodes
	public:
	virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0);
	virtual const QDomElement toXml(QDomDocument &) const;
	virtual void fromXml(const QDomElement &);
	virtual QPointF sceneTopLeft() const;
	virtual void setAngle(int);
	virtual void setStartAngle(int);
	virtual int angle() const;
	virtual int startAngle() const;
	
	protected:
	QVariant itemChange(GraphicsItemChange, const QVariant &);
};
#endif
