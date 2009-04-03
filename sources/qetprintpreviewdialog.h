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
#ifndef QET_PRINT_PREVIEW_DIALOG
#define QET_PRINT_PREVIEW_DIALOG
#include <QtGui>
class Diagram;
class DiagramsChooser;
class QETProject;
/**
	Cette classe represente un dialogue permettant d'affiner les options
	d'impression d'un schema a l'aide d'un apercu de ce qu'elle donnerait sur
	papier.
*/
class QETPrintPreviewDialog : public QDialog {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	QETPrintPreviewDialog(QETProject *, QPrinter *, QWidget * = 0, Qt::WindowFlags = 0);
	virtual ~QETPrintPreviewDialog();
	private:
	QETPrintPreviewDialog(const QETPrintPreviewDialog &);
	
	// methodes
	public:
	DiagramsChooser *diagramsChooser();
	bool fitDiagramsToPages() const;
	
	// signaux
	signals:
	void paintRequested(const QList<Diagram *> &, bool, QPrinter *);
	
	public slots:
	void firstPage();
	void previousPage();
	void nextPage();
	void lastPage();
	void pageSetup();
	void useFullPage(bool);
	void fitDiagramToPage(bool);
	void zoomIn();
	void zoomOut();
	
	// attributs
	private:
	QETProject *project_;
	QPrinter *printer_;
	QHBoxLayout *hlayout0_;
	QVBoxLayout *vlayout0_;
	QVBoxLayout *vlayout1_;
	QVBoxLayout *vlayout2_;
	QToolBar *toolbar_;
	QPrintPreviewWidget *preview_;
	QLabel *diagrams_label_;
	DiagramsChooser *diagrams_list_;
	QAction *toggle_diagrams_list_;
	QAction *toggle_print_options_;
	QAction *adjust_width_;
	QAction *adjust_page_;
	QAction *zoom_in_;
	QComboBox *zoom_box_;
	QAction *zoom_out_;
	QAction *landscape_;
	QAction *portrait_;
	QAction *first_page_;
	QAction *previous_page_;
	QAction *next_page_;
	QAction *last_page_;
	QAction *all_pages_view_;
	QAction *facing_pages_view_;
	QAction *single_page_view_;
	QAction *page_setup_;
	QDialogButtonBox *buttons_;
	QGroupBox *print_options_box_;
	QCheckBox *use_full_page_;
	QLabel *use_full_page_label_;
	QCheckBox *fit_diagram_to_page_;
	QLabel *fit_diagram_to_page_label_;
	
	// methodes
	private:
	void build();
	
	private slots:
	void requestPaint(QPrinter *);
	void checkDiagramsCount();
	void setDiagramsListVisible(bool);
	void setPrintOptionsVisible(bool);
	void updateZoomList();
	void updatePreviewZoom();
};
#endif
