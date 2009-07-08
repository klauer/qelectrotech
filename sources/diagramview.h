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
#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H
#include <QtGui>
#include "elementslocation.h"
class Conductor;
class Diagram;
class DiagramTextItem;
class Element;
class QETDiagramEditor;
/**
	Classe representant graphiquement un schema electrique
*/
class DiagramView : public QGraphicsView {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	DiagramView(Diagram * = 0, QWidget * = 0);
	virtual ~DiagramView();
	
	private:
	DiagramView(const DiagramView &);
	
	// attributs
	private:
	Diagram *scene;
	QMenu *context_menu;
	QAction *paste_here;
	QAction *find_element_;
	QPoint paste_here_pos;
	bool is_adding_text;
	ElementsLocation next_location_;
	QPoint next_position_;
	
	// methodes
	public:
	QString title() const;
	void editDiagramProperties();
	void addColumn();
	void removeColumn();
	void addRow();
	void removeRow();
	/// @return Le schema visualise par ce DiagramView
	Diagram *diagram() { return(scene); }
	QETDiagramEditor *diagramEditor() const;
	bool hasSelectedItems();
	void addText();
	DiagramTextItem *addDiagramTextAtPos(const QPointF &);
	
	protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void contextMenuEvent(QContextMenuEvent *);
	virtual void wheelEvent(QWheelEvent *);
	virtual bool event(QEvent *);
	
	private:
	void mousePressEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dragLeaveEvent(QDragLeaveEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
	QRectF viewedSceneRect() const;
	bool mustIntegrateElement(const ElementsLocation &) const;
	bool addElementAtPos(const ElementsLocation &, const QPoint &);
	
	signals:
	/// Signal emis lorsque la selection change
	void selectionChanged();
	/// Signal emis lorsque le mode de selection change
	void modeChanged();
	/// Signal emis lorsqu'un texte a ete pose
	void textAdded(bool);
	/// Signal emis lorsque le titre du schema change
	void titleChanged(DiagramView *, const QString &);
	/// Signal emis avant l'integration d'un element
	void aboutToAddElement();
	/// Signal emis lorsque l'utilisateur souhaite retrouver un element du schema dans les collections
	void findElementRequired(const ElementsLocation &);
	/// Signal emis lorsque l'utilisateur souhaite editer un element du schema
	void editElementRequired(const ElementsLocation &);
	
	public slots:
	void selectNothing();
	void selectAll();
	void selectInvert();
	void deleteSelection();
	void rotateSelection();
	void setVisualisationMode();
	void setSelectionMode();
	void zoomIn();
	void zoomOut();
	void zoomFit();
	void zoomReset();
	void cut();
	void copy();
	void paste(const QPointF & = QPointF(), QClipboard::Mode = QClipboard::Clipboard);
	void pasteHere();
	void adjustSceneRect();
	void updateWindowTitle();
	void editSelectionProperties();
	void editElement(Element *);
	void editConductor();
	void editConductor(Conductor *);
	void resetConductors();
	void editDefaultConductorProperties();
	
	private slots:
	void addDroppedElement();
	void adjustGridToZoom();
	void applyReadOnly();
};
#endif
