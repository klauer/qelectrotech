/*
	Copyright 2006-2007 Xavier Guerrin
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
#include "terminaleditor.h"
#include "partterminal.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param term La borne a editer
	@param parent QWidget parent de ce widget
*/
TerminalEditor::TerminalEditor(QETElementEditor *editor, PartTerminal *term, QWidget *parent) : ElementItemEditor(editor, parent) {
	part = term;
	
	qle_x = new QLineEdit();
	qle_y = new QLineEdit();
	
	qle_x -> setValidator(new QDoubleValidator(qle_x));
	qle_y -> setValidator(new QDoubleValidator(qle_y));
	
	orientation = new QComboBox();
	orientation -> addItem(QIcon(":/ico/north.png"), tr("Nord"),  QET::North);
	orientation -> addItem(QIcon(":/ico/east.png"),  tr("Est"),   QET::East);
	orientation -> addItem(QIcon(":/ico/south.png"), tr("Sud"),   QET::South);
	orientation -> addItem(QIcon(":/ico/west.png"),  tr("Ouest"), QET::West);
	
	QVBoxLayout *main_layout = new QVBoxLayout();
	main_layout -> addWidget(new QLabel(tr("Position : ")));
	
	QHBoxLayout *position = new QHBoxLayout();
	position -> addWidget(new QLabel(tr("x : ")));
	position -> addWidget(qle_x                 );
	position -> addWidget(new QLabel(tr("y : ")));
	position -> addWidget(qle_y                 );
	main_layout -> addLayout(position);
	
	QHBoxLayout *ori = new QHBoxLayout();
	ori -> addWidget(new QLabel(tr("Orientation : ")));
	ori -> addWidget(orientation                     );
	main_layout -> addLayout(ori);
	main_layout -> addStretch();
	setLayout(main_layout);
	
	activeConnections(true);
	updateForm();
}

/// Destructeur
TerminalEditor::~TerminalEditor() {
};

/**
	Met a jour la borne a partir des donnees du formulaire
*/
void TerminalEditor::updateTerminal() {
	part -> setPos(qle_x -> text().toDouble(), qle_y -> text().toDouble());
	part -> setOrientation(
		static_cast<QET::Orientation>(
			orientation -> itemData(
				orientation -> currentIndex()
			).toInt()
		)
	);
}

/// Met a jour l'abscisse de la position de la borne et cree un objet d'annulation
void TerminalEditor::updateTerminalX() { addChangePartCommand(tr("abscisse"),    part, "x",           qle_x -> text().toDouble()); updateForm(); }
/// Met a jour l'ordonnee de la position de la borne et cree un objet d'annulation
void TerminalEditor::updateTerminalY() { addChangePartCommand(tr("ordonn\351e"), part, "y",           qle_y -> text().toDouble()); updateForm(); }
/// Met a jour l'orientation de la borne et cree un objet d'annulation
void TerminalEditor::updateTerminalO() { addChangePartCommand(tr("orientation"), part, "orientation", orientation -> itemData(orientation -> currentIndex()).toInt()); }

/**
	Met a jour le formulaire d'edition
*/
void TerminalEditor::updateForm() {
	activeConnections(false);
	qle_x -> setText(part -> property("x").toString());
	qle_y -> setText(part -> property("y").toString());
	orientation -> setCurrentIndex(static_cast<int>(part -> orientation()));
	activeConnections(true);
}

/**
	Active ou desactive les connexionx signaux/slots entre les widgets internes.
	@param active true pour activer les connexions, false pour les desactiver
*/
void TerminalEditor::activeConnections(bool active) {
	if (active) {
		connect(qle_x,       SIGNAL(editingFinished()), this, SLOT(updateTerminalX()));
		connect(qle_y,       SIGNAL(editingFinished()), this, SLOT(updateTerminalY()));
		connect(orientation, SIGNAL(activated(int)),    this, SLOT(updateTerminalO()));
	} else {
		disconnect(qle_x,       SIGNAL(editingFinished()), this, SLOT(updateTerminalX()));
		disconnect(qle_y,       SIGNAL(editingFinished()), this, SLOT(updateTerminalY()));
		disconnect(orientation, SIGNAL(activated(int)),    this, SLOT(updateTerminalO()));
	}
}
