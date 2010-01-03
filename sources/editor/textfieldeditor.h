/*
	Copyright 2006-2010 Xavier Guerrin
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
#ifndef TEXTFIELD_EDITOR_H
#define TEXTFIELD_EDITOR_H
#include <QtGui>
#include "elementitemeditor.h"
class PartTextField;
/**
	Cette classe represente un editeur de champ de texte
	Elle permet d'editer a travers une interface graphique les
	proprietes d'un champ de texte : taille de la police, texte par
	defaut et position.
*/
class TextFieldEditor : public ElementItemEditor {
	Q_OBJECT
	// Constructeurs, destructeur
	public:
	TextFieldEditor(QETElementEditor *, PartTextField *, QWidget * = 0);
	virtual ~TextFieldEditor();
	private:
	TextFieldEditor(const TextFieldEditor &);
	
	// attributs
	private:
	PartTextField *part;
	QLineEdit *qle_x, *qle_y, *qle_text;
	QSpinBox *font_size;
	QCheckBox *rotate;
	
	// methodes
	public slots:
	void updateTextField();
	void updateTextFieldX();
	void updateTextFieldY();
	void updateTextFieldT();
	void updateTextFieldS();
	void updateTextFieldR();
	void updateForm();
	
	private:
	void activeConnections(bool);
};
#endif
