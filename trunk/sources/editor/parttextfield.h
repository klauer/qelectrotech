/*
	Copyright 2006-2012 Xavier Guerrin
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
	This class represents an editable text field which may be used to compose the
	drawing of an electrical element within the element editor. Users may specify
	a default value. The field will remain editable once the element is added onto
	a diagram. lorsque l'element sera pose sur un schema.
*/
class PartTextField : public QGraphicsTextItem, public CustomElementPart {
	Q_OBJECT
	
	// constructors, destructor
	public:
	PartTextField(QETElementEditor *, QGraphicsItem * = 0, QGraphicsScene * = 0);
	virtual ~PartTextField();
	
	private:
	PartTextField(const PartTextField &);
	
	// attributes
	bool follow_parent_rotations;
	
	// methods
	public:
	enum { Type = UserType + 1108 };
	/**
		Enable the use of qgraphicsitem_cast to safely cast a QGraphicsItem into a
		PartTextField.
		@return the QGraphicsItem type
	*/
	virtual int type() const { return Type; }
	virtual QString name() const { return(QObject::tr("champ de texte", "element part name")); }
	virtual QString xmlName() const { return(QString("input")); }
	void fromXml(const QDomElement &);
	const QDomElement toXml(QDomDocument &) const;
	qreal rotationAngle() const;
	void setRotationAngle(const qreal &);
	bool followParentRotations();
	void setFollowParentRotations(bool);
	virtual void setProperty(const QString &, const QVariant &);
	virtual QVariant property(const QString &);
	virtual bool isUseless() const;
	virtual void startUserTransformation(const QRectF &);
	virtual void handleUserTransformation(const QRectF &, const QRectF &);
	virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0 );
	
	public slots:
	void adjustItemPosition(int = 0);
	
	protected:
	virtual void focusOutEvent(QFocusEvent *);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
	virtual QVariant itemChange(GraphicsItemChange, const QVariant &);
	QRectF boundingRect() const;
	
	private:
	QPointF margin() const;
#ifdef QET_DEBUG_EDITOR_TEXTS
	void drawPoint(QPainter *, const QPointF &);
#endif
	QString previous_text;
	QPointF saved_point_;
	int saved_font_size_;
};
#endif
