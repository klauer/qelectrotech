/*
	Copyright 2006-2007 Xavier Guerrin
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
#ifndef PART_TEXTFIELD_H
#define PART_TEXTFIELD_H
#include <QtGui>
#include "customelementpart.h"
class TextFieldEditor;
class QETElementEditor;
/**
	Cette classe represente un champ de texte editable pouvant etre utilise
	pour composer le dessin d'un element dans l'editeur d'element.
	L'utilisateur peut specifier un valeur par defaut. Le champ sera editable
	lorsque l'element sera pose sur un schema.
*/
class PartTextField : public QGraphicsTextItem, public CustomElementPart {
	// constructeurs, destructeur
	public:
	PartTextField(QETElementEditor *, QGraphicsItem * = 0, QGraphicsScene * = 0);
	virtual ~PartTextField();
	
	private:
	PartTextField(const PartTextField &);
	
	// attributs
	TextFieldEditor *infos;
	bool follow_parent_rotations;
	
	// methodes
	public:
	enum { Type = UserType + 1108 };
	/**
		permet de caster un QGraphicsItem en PartTextField avec qgraphicsitem_cast
		@return le type de QGraphicsItem
	*/
	virtual int type() const { return Type; }
	virtual QString name() const { return(QObject::tr("champ de texte")); }
	void fromXml(const QDomElement &);
	const QDomElement toXml(QDomDocument &) const;
	QWidget *elementInformations();
	QPointF pos() const;
	void setPos(const QPointF &);
	void setPos(qreal, qreal);
	bool followParentRotations();
	void setFollowParentRotations(bool);
	virtual void setProperty(const QString &, const QVariant &);
	virtual QVariant property(const QString &);
	virtual bool isUseless() const;
	
	protected:
	virtual void focusOutEvent(QFocusEvent *);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
	virtual QVariant itemChange(GraphicsItemChange, const QVariant &);
	QRectF boundingRect() const;
	
	private:
	QPointF margin() const;
	QString previous_text;
};
#endif
