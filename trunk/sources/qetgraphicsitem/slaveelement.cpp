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
#include "slaveelement.h"
#include "diagramposition.h"
#include "qetapp.h"
#include "elementtextitem.h"
#include "diagram.h"

/**
 * @brief SlaveElement::SlaveElement
 * Default constructor
 * @param location location of xml definition
 * @param qgi parent QGraphicItem
 * @param s parent diagram
 * @param state int used to know if the creation of element have error
 */
SlaveElement::SlaveElement(const ElementsLocation &location, QGraphicsItem *qgi, int *state) :
	CustomElement(location, qgi, state)
{
	m_xref_item = nullptr;
	link_type_ = Slave;
	connect(this, SIGNAL(updateLabel()), this, SLOT(updateLabel()));
}

/**
 * @brief SlaveElement::~SlaveElement
 * default destructor
 */
SlaveElement::~SlaveElement() {
	unlinkAllElements();
}

/**
 * @brief SlaveElement::linkToElement
 * Link this slave to another element
 * For this class element must be a master
 * @param elmt
 */
void SlaveElement::linkToElement(Element *elmt)
{
		// check if element is master and if isn't already linked
	if (elmt->linkType() == Master && !connected_elements.contains(elmt))
	{
		if(!isFree()) unlinkAllElements();
		this->disconnect();
		connected_elements << elmt;

		connect(elmt,                 SIGNAL(xChanged()),                                       this, SLOT(updateLabel()));
		connect(elmt,                 SIGNAL(yChanged()),                                       this, SLOT(updateLabel()));
		connect(elmt,                 SIGNAL(elementInfoChange(DiagramContext, DiagramContext)),this, SLOT(updateLabel()));
		connect(diagram()->project(), SIGNAL(projectDiagramsOrderChanged(QETProject*,int,int)), this, SLOT(updateLabel()));
		connect(diagram()->project(), SIGNAL(diagramRemoved(QETProject*,Diagram*)),             this, SLOT(updateLabel()));
		connect(elmt -> diagram(),    SIGNAL(XRefPropertiesChanged()),                          this, SLOT(updateLabel()));
		connect(elmt,                 SIGNAL(updateLabel()),                                    this, SLOT(updateLabel()));

		updateLabel();
		elmt -> linkToElement(this);
		emit linkedElementChanged();
	}
}

/**
 * @brief SlaveElement::unlinkAllElements
 * Unlink all of the element in the QList connected_elements
 */
void SlaveElement::unlinkAllElements()
{
		// if this element is free no need to do something
	if (!isFree())
	{
		for (Element *elmt: connected_elements)
			unlinkElement(elmt);
		emit linkedElementChanged();
	}
}

/**
 * @brief SlaveElement::unlinkElement
 * Unlink the given elmt in parametre
 * @param elmt
 */
void SlaveElement::unlinkElement(Element *elmt)
{
		//Ensure elmt is linked to this element
	if (connected_elements.contains(elmt))
	{
		connected_elements.removeOne(elmt);

		disconnect(elmt,                 SIGNAL(xChanged()),                                       this, SLOT(updateLabel()));
		disconnect(elmt,                 SIGNAL(yChanged()),                                       this, SLOT(updateLabel()));
		disconnect(elmt,                 SIGNAL(elementInfoChange(DiagramContext, DiagramContext)),this, SLOT(updateLabel()));
		disconnect(diagram()->project(), SIGNAL(projectDiagramsOrderChanged(QETProject*,int,int)), this, SLOT(updateLabel()));
		disconnect(diagram()->project(), SIGNAL(diagramRemoved(QETProject*,Diagram*)),             this, SLOT(updateLabel()));
		disconnect(elmt -> diagram(),    SIGNAL(XRefPropertiesChanged()),                          this, SLOT(updateLabel()));
		disconnect(elmt,                 SIGNAL(updateLabel()),                                    this, SLOT(updateLabel()));

		delete m_xref_item; m_xref_item = NULL;
		
		if (ElementTextItem *eti = this->taggedText("label"))
			eti->setPlainText("_");
		
		updateLabel();
		elmt -> unlinkElement  (this) ;
		elmt -> setHighlighted (false);
		emit linkedElementChanged();
	}
}

/**
 * @brief SlaveElement::updateLabel
 * update the label (tagged with label) of this element.
 * If this element is connected to a master,
 * the label show the string tagged by "label" of the master
 * and add a qgraphicstextitem for show the position of the master
 */
void SlaveElement::updateLabel()
{
	QString label;
	if (ElementTextItem *eti = this->taggedText("label"))
		label = eti->toPlainText();
	QString Xreflabel;
	bool no_editable = false;

	//must be linked to set the label of master
	if (linkedElements().count())
	{
		no_editable = true;
		Element *elmt = linkedElements().first();
		label = elmt -> elementInformations()["label"].toString();
		XRefProperties xrp = elmt->diagram()->defaultXRefProperties(elmt->kindInformations()["type"].toString());
		Xreflabel = xrp.slaveLabel();
		Xreflabel = autonum::AssignVariables::formulaToLabel(Xreflabel, elmt->rSequenceStruct(), elmt->diagram(), elmt);
		label = autonum::AssignVariables::formulaToLabel(label, elmt->rSequenceStruct(), elmt->diagram(), elmt);
	}
	else
		label = autonum::AssignVariables::formulaToLabel(label, m_autoNum_seq, diagram(), this);

	// set the new label
	ElementTextItem *eti = setTaggedText("label", label, no_editable);
	if (eti && !isFree())
	{
		if (m_xref_item)
			m_xref_item->setPlainText(Xreflabel);
		else 
		{
			m_xref_item = new QGraphicsTextItem(Xreflabel, eti);
			m_xref_item->setFont(QETApp::diagramTextsFont(5));
			m_xref_item->setPos(eti ->  boundingRect().bottomLeft());
		}
	}
}
