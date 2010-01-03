/*
	Copyright 2006-2010 Xavier Guerrin
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
#ifndef PART_CIRCLE_H
#define PART_CIRCLE_H
#include <QtGui>
#include "customelementgraphicpart.h"
class CircleEditor;
/**
	Cette classe represente un cercle pouvant etre utilise pour composer le
	dessin d'un element dans l'editeur d'element.
*/
class PartCircle : public QGraphicsEllipseItem, public CustomElementGraphicPart {
	// constructeurs, destructeur
	public:
	PartCircle(QETElementEditor *, QGraphicsItem * = 0, QGraphicsScene * = 0);
	virtual ~PartCircle();
	
	private:
	PartCircle(const PartCircle &);
	
	// attributs
	private:
	CircleEditor *informations;
	
	// methodes
	public:
	enum { Type = UserType + 1102 };
	/**
		permet de caster un QGraphicsItem en PartCircle avec qgraphicsitem_cast
		@return le type de QGraphicsItem
	*/
	virtual int type() const { return Type; }
	virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0);
	virtual QString name() const { return(QObject::tr("cercle", "element part name")); }
	virtual const QDomElement toXml(QDomDocument &) const;
	virtual void fromXml(const QDomElement &);
	virtual QPointF sceneTopLeft() const;
	virtual QRectF boundingRect() const;
	QPointF sceneCenter() const;
	virtual void setProperty(const QString &, const QVariant &);
	virtual QVariant property(const QString &);
	virtual bool isUseless() const;
	
	protected:
	QVariant itemChange(GraphicsItemChange, const QVariant &);
};
#endif
