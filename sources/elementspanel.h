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
#ifndef PANELAPPAREILS_H
#define PANELAPPAREILS_H
#include <QtGui>
#include "elementslocation.h"
class QETProject;
class Diagram;
class ElementsCollection;
class ElementsCollectionItem;
class ElementsCategory;
class ElementDefinition;
/**
	Cette classe represente le panel d'appareils (en tant qu'element
	graphique) dans lequel l'utilisateur choisit les composants de
	son choix et les depose sur le schema par drag'n drop.
*/
class ElementsPanel : public QTreeWidget {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	ElementsPanel(QWidget * = 0);
	virtual ~ElementsPanel();
	
	private:
	ElementsPanel(const ElementsPanel &);
	
	// methodes
	public:
	// methodes pour determiner ce que represente un item donne
	bool itemIsACollection(QTreeWidgetItem *) const;
	bool itemIsACategory(QTreeWidgetItem *) const;
	bool itemIsAnElement(QTreeWidgetItem *) const;
	bool itemIsAProject(QTreeWidgetItem *) const;
	bool itemIsADiagram(QTreeWidgetItem *) const;
	bool itemHasLocation(QTreeWidgetItem *) const;
	bool itemIsWritable(QTreeWidgetItem *) const;
	
	// methodes pour obtenir ce que represente un item donne
	ElementsCollectionItem *collectionItemForItem(QTreeWidgetItem *) const;
	QETProject *projectForItem(QTreeWidgetItem *) const;
	Diagram *diagramForItem(QTreeWidgetItem *) const;
	ElementsLocation locationForItem(QTreeWidgetItem *) const;
	ElementsCategory *categoryForItem(QTreeWidgetItem *);
	ElementsCategory *categoryForPos(const QPoint &);
	
	// methodes pour determiner ce que represente l'item selectionne
	bool selectedItemIsACollection() const;
	bool selectedItemIsACategory() const;
	bool selectedItemIsAnElement() const;
	bool selectedItemIsAProject() const;
	bool selectedItemIsADiagram() const;
	bool selectedItemHasLocation() const;
	bool selectedItemIsWritable() const;
	
	// methodes pour obtenir ce que represente l'item selectionne
	ElementsCollectionItem *selectedItem() const;
	QETProject *selectedProject() const;
	Diagram *selectedDiagram() const;
	ElementsLocation selectedLocation() const;
	
	signals:
	void requestForProject(QETProject *);
	void requestForDiagram(Diagram *);
	void requestForCollectionItem(ElementsCollectionItem *);
	void requestForMoveElements(ElementsCollectionItem *, ElementsCollectionItem *, QPoint);
	
	public slots:
	void slot_doubleClick(QTreeWidgetItem *, int);
	void reload(bool = true);
	void filter(const QString &);
	void projectWasOpened(QETProject *);
	void projectWasClosed(QETProject *);
	void projectInformationsChanged(QETProject *);
	void diagramWasAdded(QETProject *, Diagram *);
	void diagramWasRemoved(QETProject *, Diagram *);
	void diagramTitleChanged(QETProject *, Diagram *);
	void diagramOrderChanged(QETProject *, int, int);
	
	protected:
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
	void startDrag(Qt::DropActions);
	
	private:
	QTreeWidgetItem *addProject   (QTreeWidgetItem *, QETProject         *);
	QTreeWidgetItem *addDiagram   (QTreeWidgetItem *, Diagram            *);
	QTreeWidgetItem *addCollection(QTreeWidgetItem *, ElementsCollection *, const QString & = QString(), const QIcon & = QIcon());
	QTreeWidgetItem *addCategory  (QTreeWidgetItem *, ElementsCategory   *, const QString & = QString(), const QIcon & = QIcon());
	QTreeWidgetItem *addElement   (QTreeWidgetItem *, ElementDefinition  *, const QString & = QString());
	void saveExpandedCategories();
	QTreeWidgetItem *findPath(const QString &) const;
	void deleteItem(QTreeWidgetItem *);
	void updateProjectItemInformations(QETProject *);
	QString diagramTitleToDisplay(Diagram *) const;
	
	// attributs
	private:
	QStringList expanded_directories;
	QString last_selected_item;
	QHash<QTreeWidgetItem *, ElementsLocation> locations_;
	QSet<QETProject *> projects_to_display_;
	QHash<QTreeWidgetItem *, QETProject *> projects_;
	QHash<QTreeWidgetItem *, Diagram *> diagrams_;
	QTreeWidgetItem *common_collection_item_;
	QTreeWidgetItem *custom_collection_item_;
};
#endif
