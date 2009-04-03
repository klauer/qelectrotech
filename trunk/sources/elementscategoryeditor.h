/*
	Copyright 2006-2009 Xavier Guerrin
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
#ifndef ELEMENTS_CATEGORY_EDITOR_H
#define ELEMENTS_CATEGORY_EDITOR_H
#include <QtGui>
#include "elementslocation.h"
class ElementsCategory;
class NamesListWidget;
class QFileNameEdit;
/**
	Cette classe permet d'editer une categorie existante ou de creer une
	categorie.
*/
class ElementsCategoryEditor : public QDialog {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	ElementsCategoryEditor(const ElementsLocation &, bool = true, QWidget * = 0);
	virtual ~ElementsCategoryEditor();
	
	private:
	ElementsCategoryEditor(const ElementsCategoryEditor &);
	
	// attributs
	private:
	ElementsCategory *category;
	QDialogButtonBox *buttons;
	NamesListWidget *names_list;
	QLabel *internal_name_label_;
	QFileNameEdit *internal_name_;
	bool mode_edit;
	
	// methodes
	private:
	void buildDialog();
	
	public slots:
	void acceptCreation();
	void acceptUpdate();
};
#endif
