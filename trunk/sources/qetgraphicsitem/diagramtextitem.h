/*
	Copyright 2006-2017 The QElectroTech Team
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
#ifndef DIAGRAM_TEXT_ITEM_H
#define DIAGRAM_TEXT_ITEM_H

#include <QGraphicsTextItem>

class Diagram;
class QDomElement;
class QDomDocument;

/**
	This class represents a selectable, movable and editable text field on a
	diagram.
	@see QGraphicsItem::GraphicsItemFlags
*/
class DiagramTextItem : public QGraphicsTextItem
{
	Q_OBJECT
    
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
	
	signals:
		void fontSizeChanged(int size);
		void colorChanged(QColor color);

	public:
		DiagramTextItem(QGraphicsItem * = 0);
		DiagramTextItem(const QString &, QGraphicsItem * = 0);

	private:
		void build();
	
	public:
		enum { Type = UserType + 1004 };
		virtual int type() const override { return Type; }

		Diagram *diagram() const;
		virtual void fromXml(const QDomElement &) = 0;
		virtual QDomElement toXml(QDomDocument &) const;
		qreal rotationAngle() const;
		void setRotationAngle(const qreal &);
		void rotateBy(const qreal &);
		void edit();

		QPointF mapMovementToScene    (const QPointF &) const;
		QPointF mapMovementFromScene  (const QPointF &) const;
		QPointF mapMovementToParent   (const QPointF &) const;
		QPointF mapMovementFromParent (const QPointF &) const;

		void setFontSize(int s);
        int fontSize()const;
        
        void setColor(QColor color);
        QColor color() const;
        
		void setNoEditable(bool e = true) {m_no_editable = e;}

	protected:
		virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
		virtual void focusInEvent(QFocusEvent *) override;
		virtual void focusOutEvent(QFocusEvent *) override;

		virtual void mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event) override;
		virtual void mousePressEvent       (QGraphicsSceneMouseEvent *event) override;
		virtual void mouseMoveEvent        (QGraphicsSceneMouseEvent *event) override;
		virtual void mouseReleaseEvent     (QGraphicsSceneMouseEvent *event) override;

		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
		virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

		virtual void applyRotation(const qreal &);

	signals:
		void diagramTextChanged(DiagramTextItem *, const QString &, const QString &);
		void textEdited(const QString &old_str, const QString &new_str);
	
	protected:
		bool m_mouse_hover = false,
			 m_first_move = true,
		     m_no_editable;

		QString m_previous_html_text,
				m_previous_text;
		
		qreal m_rotation_angle;
		QPointF m_mouse_to_origin_movement;
};
#endif
