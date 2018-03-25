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
#include "simpleelement.h"
#include "commentitem.h"
#include "diagram.h"

/**
 * @brief SimpleElement::SimpleElement
 * @param location
 * @param qgi
 * @param s
 * @param state
 */
SimpleElement::SimpleElement(const ElementsLocation &location, QGraphicsItem *qgi, int *state) :
	CustomElement(location, qgi, state),
	m_comment_item (nullptr),
	m_location_item (nullptr)
{
	m_link_type = Simple;
}

/**
 * @brief SimpleElement::~SimpleElement
 */
SimpleElement::~SimpleElement() {
	if (m_comment_item) delete m_comment_item;
}

/**
 * @brief SimpleElement::initLink
 * @param project
 * Call init Link from custom element and after
 * call update label for setup it.
 */
void SimpleElement::initLink(QETProject *project) {
	CustomElement::initLink(project);
}
