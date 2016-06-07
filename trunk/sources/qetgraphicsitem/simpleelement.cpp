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
#include "simpleelement.h"
#include "commentitem.h"
#include "elementtextitem.h"

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
	link_type_ = Simple;
	connect(this, SIGNAL(elementInfoChange(DiagramContext, DiagramContext)), this, SLOT(updateLabel(DiagramContext, DiagramContext)));
	connect(this, SIGNAL(xChanged()),this, SLOT(changeElementInfo()));
	connect(this, SIGNAL(yChanged()),this, SLOT(changeElementInfo()));
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
	updateLabel(DiagramContext(), elementInformations());
}

/**
 * @brief SimpleElement::changeElementInfo()
 * Update label if it contains %c, %l, %f or %F variables
 */
void SimpleElement::changeElementInfo(){
	QString temp_label = this->elementInformations()["label"].toString();
	if (temp_label.contains("\%l")||temp_label.contains("\%c")||temp_label.contains("\%f")||temp_label.contains("\%F")) {
		if (this->diagram()!=NULL)
			this->updateLabel(this->elementInformations(),this->elementInformations());
	}
}

/**
 * @brief SimpleElement::updateLabel
 * update label of this element
 */
void SimpleElement::updateLabel(DiagramContext old_info, DiagramContext new_info) {
	QString label = new_info["label"].toString();
	Element *elmt = this;
	label = assignVariables(label,elmt);

	//Label of element
	if (old_info["label"].toString() != label) {
		if (new_info["label"].toString().isEmpty())
			setTaggedText("label", "_", false);
		else {
			setTaggedText("label", label, true);
		}
	}

	if (ElementTextItem *eti = taggedText("label")) {
		new_info["label"].toString().isEmpty() ? eti->setVisible(true) : eti -> setVisible(new_info.keyMustShow("label"));
	}

	//Comment and Location of element
	QString comment   = new_info["comment"].toString();
	bool    must_show = new_info.keyMustShow("comment");
	QString location  = new_info["location"].toString();
	bool must_show_location = new_info.keyMustShow("location");

	if ((!(comment.isEmpty() || !must_show) && !m_comment_item)||(!(location.isEmpty() || !must_show_location) && !m_comment_item)) {
		m_comment_item = new CommentItem(this);
	}
	else if (((comment.isEmpty() || !must_show) && m_comment_item) && ((location.isEmpty() || !must_show_location) && m_comment_item)) {
		delete m_comment_item;
		m_comment_item = nullptr;
	}
}
