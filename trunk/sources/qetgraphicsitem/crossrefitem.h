/*
	Copyright 2006-2014 The QElectroTech Team
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
#ifndef CROSSREFITEM_H
#define CROSSREFITEM_H

#include "qetgraphicsitem/qetgraphicsitem.h"
class element;

/**
 * @brief The CrossRefItem class
 * This clas provide an item, for show the cross reference, like the contacts linked to a coil.
 * The item setpos automaticaly when parent move.
 * All slave displayed in cross ref will be updated when folio position change in the project.
 * It's the responsability of the parent to informe displayed slave are moved,
 * by calling the slot @updateLabel
 */
class CrossRefItem : public QetGraphicsItem
{
	Q_OBJECT

	//Methods
	public:
	explicit CrossRefItem(Element *elmt, QetGraphicsItem *parent = 0);
	~CrossRefItem();

	QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	signals:

	public slots:
	void updateLabel();
	void autoPos();

	protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

	private:
	void setUpBoundingRect(QPainter &painter);
	void fillCrossRef(QPainter &painter);
	void fillExtraInfo(QPainter &painter);

	//Attributes
	private:
	Element *element_; //element to display the cross reference
	QRectF bounding_rect_ , text_rect_;
	QPicture drawing_;
	QPainterPath shape_path_;

};

#endif // CROSSREFITEM_H
