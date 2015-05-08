/*
	Copyright 2006-2015 The QElectroTech Team
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
#include "diagrampropertieseditordockwidget.h"
#include "elementpropertieswidget.h"
#include "diagram.h"
#include "element.h"
#include "diagramimageitem.h"
#include "imagepropertieswidget.h"

/**
 * @brief DiagramPropertiesEditorDockWidget::DiagramPropertiesEditorDockWidget
 * Constructor
 * @param parent : parent widget
 */
DiagramPropertiesEditorDockWidget::DiagramPropertiesEditorDockWidget(QWidget *parent) :
	PropertiesEditorDockWidget(parent),
	m_diagram(nullptr)
{}

/**
 * @brief DiagramPropertiesEditorDockWidget::setDiagram
 * Set the diagram to edit the selection.
 * Connect the diagram signal selectionChanged() to this slot selectionChanged();
 * If diagram = nullptr, we just disconnect all signal and remove editor.
 * @param diagram
 * @param diagram
 */
void DiagramPropertiesEditorDockWidget::setDiagram(Diagram *diagram)
{
	if (m_diagram == diagram) return;
	clear();

	if (m_diagram)
	{
		disconnect(m_diagram, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
		disconnect(m_diagram, SIGNAL(destroyed()),        this, SLOT(diagramWasDeleted()));
	}

	if (diagram)
	{
		m_diagram = diagram;
		connect(m_diagram, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
		connect(m_diagram, SIGNAL(destroyed()),        this, SLOT(diagramWasDeleted()));
		selectionChanged();
	}
	else
		m_diagram = nullptr;
}

/**
 * @brief DiagramPropertiesEditorDockWidget::selectionChanged
 * The current selection of diagram was changed.
 * We fill the dock with the appropriate ElementPropertiesWidget of the current selection.
 */
void DiagramPropertiesEditorDockWidget::selectionChanged()
{
	if (!m_diagram) return;
	clear();

	if (m_diagram->selectedItems().size() == 1)
	{
		QGraphicsItem *item = m_diagram->selectedItems().first();

		if (Element *elmt = dynamic_cast<Element*>(item))
			addEditor(new ElementPropertiesWidget(elmt, this));
		else if (DiagramImageItem *image = dynamic_cast<DiagramImageItem *>(item))
			addEditor(new ImagePropertiesWidget(image, this));
	}
}

/**
 * @brief DiagramPropertiesEditorDockWidget::diagramWasDeleted
 * Remove current editor and set m_diagram to nullptr.
 */
void DiagramPropertiesEditorDockWidget::diagramWasDeleted()
{
	m_diagram = nullptr;
	clear();
}
