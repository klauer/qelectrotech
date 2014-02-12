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
#include "elementinfowidget.h"
#include "ui_elementinfowidget.h"
#include "qetapp.h"

/**
 * @brief ElementInfoWidget::ElementInfoWidget
 * Constructor
 * @param elmt element to edit information
 * @param parent parent widget
 */
ElementInfoWidget::ElementInfoWidget(Element *elmt, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ElementInfoWidget),
	element_(elmt),
	elmt_info(elmt->informations())
{
	ui->setupUi(this);
	buildInterface();
	fillInfo();
}

/**
 * @brief ElementInfoWidget::~ElementInfoWidget
 * Destructor
 */
ElementInfoWidget::~ElementInfoWidget()
{
	qDeleteAll(eipw_list);
	delete ui;
}

/**
 * @brief ElementInfoWidget::apply
 * Apply the new information
 */
void ElementInfoWidget::apply() {
	DiagramContext dc;
	foreach (ElementInfoPartWidget *eipw, eipw_list) {
		//add value only if they're something to store
		if (!eipw->text().isEmpty())
			dc.addValue(eipw->key(),
						eipw->text(),
						eipw->mustShow());
	}
	element_->setInformations(dc);
}

/**
 * @brief ElementInfoWidget::buildInterface
 * Build the widget
 */
void ElementInfoWidget::buildInterface() {
	foreach (QString str, QETApp::elementInfoKeys()) {
		ElementInfoPartWidget *eipw = new ElementInfoPartWidget(str, QETApp::elementTranslatedInfoKey(str), this);
		ui->scroll_vlayout->addWidget(eipw);
		eipw_list << eipw;
	}
}

/**
 * @brief ElementInfoWidget::fillInfo
 * fill information fetch in elmt_info to the
 * corresponding line edit
 */
void ElementInfoWidget::fillInfo() {
	foreach (ElementInfoPartWidget *eipw, eipw_list) {
		eipw->setText(elmt_info[eipw->key()].toString());
		eipw->setShow(elmt_info.keyMustShow(eipw->key()));
		//check "show" if this eipw is label and if elmt_info haven't got value for label.
		if (eipw->key() == "label" && (elmt_info["label"].toString().isEmpty()))
			eipw->setShow(true);
	}
}
