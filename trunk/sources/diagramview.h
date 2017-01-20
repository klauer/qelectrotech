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
#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QGraphicsView>
#include <QClipboard>
#include "elementslocation.h"
#include "templatelocation.h"

class Conductor;
class Diagram;
class QETDiagramEditor;
class DVEventInterface;
class QMenu;
class QInputEvent;
class QGestureEvent;

/**
	This class provides a widget to render an electric diagram in an editable,
	interactive way.
*/
class DiagramView : public QGraphicsView
{
	Q_OBJECT
	
		// constructors, destructor
	public:
		DiagramView(Diagram *diagram, QWidget * = 0);
		virtual ~DiagramView();

	private:
		DiagramView(const DiagramView &);
	
		// attributes

		Diagram          *scene;
		DVEventInterface *m_event_interface;
		QMenu            *context_menu;
		QAction          *paste_here;
		QPoint            paste_here_pos;
		QPointF           rubber_band_origin;
		bool              fresh_focus_in_;    ///< Indicate the focus was freshly gained
		bool m_first_activation;

	public:
		QString title() const;
		void editDiagramProperties();
		void addColumn();
		void removeColumn();
		void addRow();
		void removeRow();
		/// @return the diagram rendered by this view
		Diagram *diagram() { return(scene); }
		QETDiagramEditor *diagramEditor() const;
		bool hasSelectedItems();
		bool hasCopiableItems();
		bool hasTextItems();
		bool hasDeletableItems();
		void editSelection();
		void setEventInterface (DVEventInterface *event_interface);
	
	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent *);
		virtual void contextMenuEvent(QContextMenuEvent *);
		virtual void wheelEvent(QWheelEvent *);
		virtual void focusInEvent(QFocusEvent *);
		virtual void keyPressEvent(QKeyEvent *);
		virtual void keyReleaseEvent(QKeyEvent *);
		virtual bool event(QEvent *);
		virtual bool switchToVisualisationModeIfNeeded(QInputEvent *e);
		virtual bool switchToSelectionModeIfNeeded(QInputEvent *e);
		virtual bool isCtrlShifting(QInputEvent *);
		virtual bool selectedItemHasFocus();
	
	private:
		void mousePressEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void dragEnterEvent(QDragEnterEvent *);
		void dragLeaveEvent(QDragLeaveEvent *);
		void dragMoveEvent(QDragMoveEvent *);
		void dropEvent(QDropEvent *);
		void handleElementDrop(QDropEvent *);
		void handleTitleBlockDrop(QDropEvent *);
		void handleTextDrop(QDropEvent *);
		void scrollOnMovement(QKeyEvent *);
		bool gestureEvent(QGestureEvent *event);
		QRectF viewedSceneRect() const;
		bool mustIntegrateTitleBlockTemplate(const TitleBlockTemplateLocation &) const;
		bool gestures() const;

	signals:
			/// Signal emitted after the selection changed
		void selectionChanged();
			/// Signal emitted after the selection mode changed
		void modeChanged();
			/// Signal emitted after the diagram title changed
		void titleChanged(DiagramView *, const QString &);
			/// Signal emitted when users wish to locate an element from the diagram within elements collection
		void findElementRequired(const ElementsLocation &);
			/// Signal emitted when users wish to edit an element from the diagram
		void editElementRequired(const ElementsLocation &);
			/// Signal emmitted when diagram must be show
		void showDiagram (Diagram *);
	
	public slots:
		void selectNothing();
		void selectAll();
		void selectInvert();
		void deleteSelection();
		void rotateSelection();
		void rotateTexts();
		void setVisualisationMode();
		void setSelectionMode();
		void zoom(const qreal zoom_factor);
		void zoomFit();
		void zoomContent();
		void zoomReset();
		void cut();
		void copy();
		void paste(const QPointF & = QPointF(), QClipboard::Mode = QClipboard::Clipboard);
		void pasteHere();
		void adjustSceneRect();
		void updateWindowTitle();
		void editSelectionProperties();
		void editSelectedConductorColor();
		void editConductorColor(Conductor *);
		void resetConductors();
	
	private slots:
		void adjustGridToZoom();
		void applyReadOnly();
};
#endif
