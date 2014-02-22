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
#include "masterelement.h"

/**
 * @brief MasterElement::MasterElement
 * Default constructor
 * @param location location of xml definition
 * @param qgi parent QGraphicItem
 * @param s parent diagram
 * @param state int used to know if the creation of element have error
 */
MasterElement::MasterElement(const ElementsLocation &location, QGraphicsItem *qgi, Diagram *s, int *state) :
	CustomElement(location, qgi, s, state)
{
	link_type_ = Master;
}

/**
 * @brief MasterElement::~MasterElement
 * default destructor
 */
MasterElement::~MasterElement() {
	unlinkAllElements();
}

/**
 * @brief MasterElement::linkToElement
 * Link this master to another element
 * For this class element must be a slave
 * @param elmt
 */
void MasterElement::linkToElement(Element *elmt) {	
	// check if element is slave and if isn't already linked
	if (elmt->linkType() == Slave && !connected_elements.contains(elmt)) {
		///TODO create the cross ref and connection
		connected_elements << elmt;
		elmt->linkToElement(this);
	}
}

/**
 * @brief MasterElement::unlinkAllElements
 * Unlink all of the element in the QList connected_elements
 */
void MasterElement::unlinkAllElements() {
	// if this element is free no need to do something
	if (!isFree()) {
		foreach(Element *elmt, connected_elements) {
			unlinkElement(elmt);
		}
	}
}

/**
 * @brief MasterElement::unlinkElement
 * Unlink the given elmt in parametre
 * @param elmt element to unlink from this
 */
void MasterElement::unlinkElement(Element *elmt) {
	//Ensure elmt is linked to this element
	if (connected_elements.contains(elmt)) {
		connected_elements.removeOne(elmt);
		elmt->unlinkElement(this);
	}
}
