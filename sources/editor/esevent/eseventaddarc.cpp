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
#include <QObject>

#include "elementscene.h"
#include "partarc.h"
#include "editorcommands.h"
#include "eseventaddarc.h"

/**
 * @brief ESEventAddArc::ESEventAddArc
 * @param scene
 */
ESEventAddArc::ESEventAddArc(ElementScene *scene) :
	ESEventInterface(scene),
	m_arc(nullptr),
	m_inverted(false)
{}

/**
 * @brief ESEventAddArc::~ESEventAddArc
 */
ESEventAddArc::~ESEventAddArc() {
	if (m_running || m_abort)
		delete m_arc;
}

/**
 * @brief ESEventAddPolygon::mousePressEvent
 * @param event
 * @return
 */
bool ESEventAddArc::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event -> button() == Qt::LeftButton) {
		if(!m_running) m_running = true;
		QPointF pos = m_scene->snapToGrid(event -> scenePos());

		//create new arc
		if (!m_arc) {
			m_arc = new PartArc(m_editor, 0, m_scene);
			m_arc -> setRect(QRectF(pos, pos));
			m_arc -> setAngle(90);
			m_arc -> setProperty("antialias", true);
			m_origin = pos;
			return true;
		}

		//Add arc to scene
		m_arc -> setRect(m_arc->rect().normalized());
		m_scene -> undoStack().push(new AddPartCommand(QObject::tr("Arc"), m_scene, m_arc));

		//Set m_arc to nullptr for create new ellipse at next mouse press
		m_arc = nullptr;

		return true;
	}
	return false;
}

/**
 * @brief ESEventAddArc::mouseMoveEvent
 * @param event
 * @return
 */
bool ESEventAddArc::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	updateHelpCross(event -> scenePos());
	if (!m_arc) return false;

	m_mouse_pos = m_scene -> snapToGrid(event -> scenePos());
	updateArc();

	return true;
}

/**
 * @brief ESEventAddArc::mouseReleaseEvent
 * @param event
 * @return
 */
bool ESEventAddArc::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	if (event -> button() == Qt::RightButton) {
		if (m_arc) {delete m_arc; m_arc = nullptr;}
		else        {m_running = false;}
		return true;
	}
	return false;
}

/**
 * @brief ESEventAddArc::keyPressEvent
 * @param event
 * @return
 */
bool ESEventAddArc::keyPressEvent(QKeyEvent *event) {
	if (m_arc && event->key() == Qt::Key_Space) {
		m_inverted = m_inverted ? false : true;
		updateArc();
		return true;
	}

	return (ESEventInterface::keyPressEvent(event));
}

/**
 * @brief ESEventAddArc::updateArc
 * Redraw the arc with curent value
 */
void ESEventAddArc::updateArc() {

	qreal width  = (m_mouse_pos.x() - m_origin.x())*2;
	if (width < 0) width *= -1;
	qreal height = (m_mouse_pos.y() - m_origin.y())*2;
	if (height < 0) height *= -1;

	QPointF pos_ = m_origin;

	//Draw arc inverted
	if (m_inverted) {
		//Adjust the start angle to be snapped at the origin point of draw
		if (m_mouse_pos.y() > m_origin.y()) {

			if (m_mouse_pos.x() > m_origin.x()) {
				pos_.ry() -= height/2;
				m_arc->setStartAngle(180);
			}
			else {
				pos_.rx() -= width/2;
				m_arc->setStartAngle(90);
			}
		}
		else {
			if (m_mouse_pos.x() > m_origin.x()) {
				pos_.ry() -= height;
				pos_.rx() -= width/2;
				m_arc->setStartAngle(270);
			}
			else {
				pos_.rx() -= width;
				pos_.ry() -= height/2;
				m_arc->setStartAngle(0);
			}
		}
	}

	//Draw arc non inverted
	else {
		//Adjust the start angle to be snapped at the origin point of draw
		if (m_mouse_pos.y() > m_origin.y()) {

			if (m_mouse_pos.x() > m_origin.x()) {
				pos_.rx() -= width/2;
				m_arc->setStartAngle(0);
			}
			else {
				pos_.rx() -= width;
				pos_.ry() -= height/2;
				m_arc->setStartAngle(270);
			}
		}
		else {
			if (m_mouse_pos.x() > m_origin.x()) {
				pos_.ry() -= height/2;
				m_arc->setStartAngle(90);
			}
			else {
				pos_.rx() -= width/2;
				pos_.ry() -= height;
				m_arc->setStartAngle(180);
			}
		}
	}

	m_arc -> setRect(QRectF(pos_, QSizeF(width, height)));
}