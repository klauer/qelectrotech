/*
	Copyright 2006-2016 The QElectroTech Team
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
#include "qetgraphicshandlerutility.h"
#include <QPainter>

/**
 * @brief QetGraphicsHandlerUtility::QetGraphicsHandlerUtility
 * Constructor
 * @param size : the size of the handler
 */
QetGraphicsHandlerUtility::QetGraphicsHandlerUtility(qreal size) :
	m_size (size),
	m_zoom_factor(1)
{}

/**
 * @brief QetGraphicsHandlerUtility::drawHandler
 * Draw the handler at pos @point, using the QPainter @painter.
 * @param painter : painter to use for drawing the handler
 * @param point : point to draw the handler
 */
void QetGraphicsHandlerUtility::drawHandler(QPainter *painter, const QPointF &point, bool color2)
{
		//Color of handler
	QColor inner(0xFF, 0xFF, 0xFF);
	QColor outer(0x00, 0x61, 0xFF);
	if(color2) outer = QColor(0x1A, 0x5C, 0x14);
		//Setup the zoom factor to draw the handler in the same size at screen,
		//no matter the zoom of the QPainter
	m_zoom_factor = 1.0/painter->transform().m11();

	painter->save();
	painter->setBrush(QBrush(inner));
	QPen square_pen(QBrush(outer), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	square_pen.setCosmetic(true);
	painter->setPen(square_pen);
	painter->drawRect(getRect(point));
	painter->restore();
}

/**
 * @brief QetGraphicsHandlerUtility::drawHandler
 * Conveniance method for void QetGraphicsHandlerUtility::drawHandler(QPainter *painter, const QPointF &point, bool color2)
 * @param painter
 * @param points
 * @param color2
 */
void QetGraphicsHandlerUtility::drawHandler(QPainter *painter, const QVector<QPointF> &points, bool color2) {
	foreach(QPointF point, points)
		drawHandler(painter, point, color2);
}

/**
 * @brief QetGraphicsHandlerUtility::pointIsInHandler
 * @param point : point to compare
 * @param key_point : point at the center of handler (the point to modify, for exemple the corner of a rectangle)
 * @return true if point is in a handler. else false
 */
bool QetGraphicsHandlerUtility::pointIsInHandler(const QPointF &point, const QPointF &key_point) const {
	return (getRect(key_point).contains(point));
}

/**
 * @brief QetGraphicsHandlerUtility::pointIsHoverHandler
 * @param point : point to compare
 * @param vector : vector of key_point (the point to modify, for exemple the corners of a rectangle)
 * @return if point is hover an handler, return the index of the hovered key_point in the vector, else return -1
 */
int QetGraphicsHandlerUtility::pointIsHoverHandler(const QPointF &point, const QVector<QPointF> &vector) const
{
	foreach (QPointF key_point, vector)
		if (pointIsInHandler(point, key_point))
			return vector.indexOf(key_point);

	return -1;
}

/**
 * @brief QetGraphicsHandlerUtility::handlerRect
 * Return the rect of the handler for all key_point in vector (the point to modify, for exemple the corners of a rectangle)
 * The order of rect in the returned vector is the same as the given vector.
 * @param vector
 * @return
 */
QVector<QRectF> QetGraphicsHandlerUtility::handlerRect(const QVector<QPointF> &vector) const
{
	QVector <QRectF> rect_vector;

	foreach(QPointF point, vector)
		rect_vector << getRect(point);

	return rect_vector;
}

/**
 * @brief QetGraphicsHandlerUtility::getRect
 * @param point
 * @return
 */
QRectF QetGraphicsHandlerUtility::getRect(const QPointF &point) const
{
	qreal rect_size = m_size * m_zoom_factor;
	QRectF rect(point.x() - rect_size/2, point.y() - rect_size/2, rect_size, rect_size);
	return rect;
}

/**
 * @brief QetGraphicsHandlerUtility::pointsForRect
 * Return the keys points of the rectangle, stored in a vector.
 * The points in the vector are stored like this :
 * **********
 *   0---1---2
 *   |       |
 *   3       4
 *   |       |
 *   5---6---7
 * ************
 * @param rect
 * @return
 */
QVector<QPointF> QetGraphicsHandlerUtility::pointsForRect(const QRectF &rect)
{
	QVector<QPointF> vector;
	QPointF point;
	vector << rect.topLeft();//*****Top left
	point = rect.center();
	point.setY(rect.top());
	vector << point;//**************Middle top
	vector << rect.topRight();//****Top right
	point = rect.center();
	point.setX(rect.left());
	vector << point;//**************Middle left
	point.setX(rect.right());
	vector << point;//**************Middle right
	vector << rect.bottomLeft();//**Bottom left
	point = rect.center();
	point.setY(rect.bottom());
	vector <<  point;//*************Middle bottom
	vector << rect.bottomRight();//*Bottom right
	return vector;
}

/**
 * @brief QetGraphicsHandlerUtility::pointsForLine
 * The point that define a line in a QVector.
 * there is two points.
 * @param line
 * @return
 */
QVector<QPointF> QetGraphicsHandlerUtility::pointsForLine(const QLineF &line) {
	return (QVector<QPointF> {line.p1(), line.p2()});
}

/**
 * @brief QetGraphicsHandlerUtility::rectForPosAtIndex
 * Return a rectangle after modification of the point '@pos' at index '@index' of original rectangle '@old_rect'.
 * @param old_rect - the rectangle befor modification
 * @param pos - the new position of a key point
 * @param index - the index of the key point to modifie see QetGraphicsHandlerUtility::pointsForRect to know
 * the index of each keys points of a rectangle)
 * @return : the rectangle with modification. If index is lower than 0 or higher than 7, this method return old_rect.
 */
QRectF QetGraphicsHandlerUtility::rectForPosAtIndex(const QRectF &old_rect, const QPointF &pos, int index)
{
	if (index < 0 || index > 7) return old_rect;

	QRectF rect = old_rect;
	if (index == 0) rect.setTopLeft(pos);
	else if (index == 1) rect.setTop(pos.y());
	else if (index == 2) rect.setTopRight(pos);
	else if (index == 3) rect.setLeft(pos.x());
	else if (index == 4) rect.setRight(pos.x());
	else if (index == 5) rect.setBottomLeft(pos);
	else if (index == 6) rect.setBottom(pos.y());
	else if (index == 7) rect.setBottomRight(pos);

	return rect;
}

/**
 * @brief QetGraphicsHandlerUtility::lineForPosAtIndex
 * Return a line after modification of @pos at index @index of @old_line.
 * @param old_line
 * @param pos
 * @param index
 * @return
 */
QLineF QetGraphicsHandlerUtility::lineForPosAtIndex(const QLineF &old_line, const QPointF &pos, int index) {
	QLineF line = old_line;
	index == 0 ? line.setP1(pos) : line.setP2(pos);
	return line;
}