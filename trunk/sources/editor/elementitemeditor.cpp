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
#include "elementitemeditor.h"
#include "qetelementeditor.h"
#include "editorcommands.h"

/**
	Constructeur
	@param editor QETElementEditor auquel cet editeur appartient
	@param parent QWidget parent de cet editeur
*/
ElementItemEditor::ElementItemEditor(QETElementEditor *editor, QWidget *parent) :
	QWidget(parent),
	element_editor(editor)
{
}

/// @return le QETElementEditor auquel cet editeur appartient
QETElementEditor *ElementItemEditor::elementEditor() const {
	return(element_editor);
}

/// @return l'ElementScene contenant les parties editees par cet editeur
ElementScene *ElementItemEditor::elementScene() const {
	return(element_editor -> elementScene());
}

/// @return la QUndoStack a utiliser pour les annulations
QUndoStack &ElementItemEditor::undoStack() const {
	return(elementScene() -> undoStack());
}

/**
	Ajoute une ChangePartCommand a l'UndoStack. L'ancienne valeur sera
	automatiquement recuperee.
	@param desc   nom de la propriete modifiee
	@param part   partie modifiee
	@param prop   propriete modifiee
	@param new_v  nouvelle valeur
*/
void ElementItemEditor::addChangePartCommand(const QString &desc, CustomElementPart *part, const QString &prop, const QVariant &new_v) {
	QVariant old_v = part -> property(prop);
	if (old_v == new_v) return;
	undoStack().push(
		new ChangePartCommand(
			desc + " " + element_type_name,
			part,
			prop,
			old_v,
			new_v
		)
	);
}

/// @return Le nom du type d'element edite
QString ElementItemEditor::elementTypeName() const {
	return(element_type_name);
}

/// @param name Nom du type d'element edite
void ElementItemEditor::setElementTypeName(const QString &name) {
	element_type_name = name;
}
