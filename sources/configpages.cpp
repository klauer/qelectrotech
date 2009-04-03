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
#include "configpages.h"
#include "borderpropertieswidget.h"
#include "conductorpropertieswidget.h"
#include "insetpropertieswidget.h"
#include "qetapp.h"
#include "qetdiagrameditor.h"
#include "borderinset.h"

/**
	Constructeur
	@param parent QWidget parent
*/
NewDiagramPage::NewDiagramPage(QWidget *parent) : ConfigPage(parent) {
	// dimensions par defaut d'un schema
	bpw = new BorderPropertiesWidget(QETDiagramEditor::defaultBorderProperties());
	
	// proprietes par defaut d'un cartouche
	ipw = new InsetPropertiesWidget(QETDiagramEditor::defaultInsetProperties(), true);
	
	// proprietes par defaut des conducteurs
	cpw = new ConductorPropertiesWidget(QETDiagramEditor::defaultConductorProperties());
	cpw -> setContentsMargins(0, 0, 0, 0);
	
	QVBoxLayout *vlayout1 = new QVBoxLayout();
	
	QLabel *title = new QLabel(this -> title());
	vlayout1 -> addWidget(title);
	
	QFrame *horiz_line = new QFrame();
	horiz_line -> setFrameShape(QFrame::HLine);
	vlayout1 -> addWidget(horiz_line);
	
	QHBoxLayout *hlayout1 = new QHBoxLayout();
	QVBoxLayout *vlayout2 = new QVBoxLayout();
	
	vlayout2 -> addWidget(bpw);
	vlayout2 -> addWidget(ipw);
	vlayout2 -> setSpacing(5);
	hlayout1 -> addLayout(vlayout2);
	hlayout1 -> addWidget(cpw);
	vlayout1 -> addLayout(hlayout1);
	vlayout1 -> addStretch(1);
	hlayout1 -> setAlignment(cpw, Qt::AlignTop);
	setLayout(vlayout1);
}

/// Destructeur
NewDiagramPage::~NewDiagramPage() {
}

/**
	Applique la configuration de cette page
*/
void NewDiagramPage::applyConf() {
	QSettings &settings = QETApp::settings();
	
	// dimensions des nouveaux schemas
	bpw -> borderProperties().toSettings(settings, "diagrameditor/default");
	
	// proprietes du cartouche
	ipw-> insetProperties().toSettings(settings, "diagrameditor/default");
	
	// proprietes par defaut des conducteurs
	cpw -> conductorProperties().toSettings(settings, "diagrameditor/defaultconductor");
}

/// @return l'icone de cette page
QIcon NewDiagramPage::icon() const {
	return(QIcon(":/ico/conf_new_diagram_110.png"));
}

/// @return le titre de cette page
QString NewDiagramPage::title() const {
	return(tr("Nouveau sch\351ma", "configuration page title"));
}


/**
	Constructeur
	@param parent QWidget parent
*/
GeneralConfigurationPage::GeneralConfigurationPage(QWidget *parent) : ConfigPage(parent) {
	
	// acces a la configuration de QElectroTech
	QSettings &settings = QETApp::settings();
	bool tabbed = settings.value("diagrameditor/viewmode", "windowed") == "tabbed";
	bool integrate_elements = settings.value("diagrameditor/integrate-elements", true).toBool();
	
	projects_view_mode_ = new QGroupBox(tr("Projets"), this);
	windowed_mode_ = new QRadioButton(tr("Utiliser des fen\352tres"), projects_view_mode_);
	tabbed_mode_ = new QRadioButton(tr("Utiliser des onglets"), projects_view_mode_);
	warning_view_mode_ = new QLabel(tr("Ces param\350tres s'appliqueront d\350s la prochaine ouverture d'un \351diteur de sch\351mas."));
	
	elements_management_ = new QGroupBox(tr("Gestion des \351l\351ments"), this);
	integrate_elements_ = new QCheckBox(tr("Int\351grer automatiquement les \351l\351ments dans les projets (recommand\351)"), elements_management_);
	
	if (tabbed) {
		tabbed_mode_ -> setChecked(true);
	} else {
		windowed_mode_ -> setChecked(true);
	}
	
	integrate_elements_ -> setChecked(integrate_elements);
	
	QVBoxLayout *projects_view_mode_layout = new QVBoxLayout();
	projects_view_mode_layout -> addWidget(windowed_mode_);
	projects_view_mode_layout -> addWidget(tabbed_mode_);
	projects_view_mode_layout -> addWidget(warning_view_mode_);
	projects_view_mode_ -> setLayout(projects_view_mode_layout);
	
	QVBoxLayout *elements_management_layout = new QVBoxLayout();
	elements_management_layout -> addWidget(integrate_elements_);
	elements_management_ -> setLayout(elements_management_layout);
	
	QVBoxLayout *vlayout1 = new QVBoxLayout();
	
	QLabel *title_label_ = new QLabel(title());
	vlayout1 -> addWidget(title_label_);
	
	QFrame *horiz_line_ = new QFrame();
	horiz_line_ -> setFrameShape(QFrame::HLine);
	vlayout1 -> addWidget(horiz_line_);
	
	vlayout1 -> addWidget(projects_view_mode_);
	vlayout1 -> addWidget(elements_management_);
	vlayout1 -> addStretch();
	
	setLayout(vlayout1);
}

/// Destructeur
GeneralConfigurationPage::~GeneralConfigurationPage() {
}

/**
	Applique la configuration de cette page
*/
void GeneralConfigurationPage::applyConf() {
	QSettings &settings = QETApp::settings();
	
	QString view_mode = tabbed_mode_ -> isChecked() ? "tabbed" : "windowed";
	settings.setValue("diagrameditor/viewmode", view_mode) ;
	
	settings.setValue("diagrameditor/integrate-elements", integrate_elements_ -> isChecked());
}

/// @return l'icone de cette page
QIcon GeneralConfigurationPage::icon() const {
	return(QIcon(":/ico/settings.png"));
}

/// @return le titre de cette page
QString GeneralConfigurationPage::title() const {
	return(tr("G\351n\351ral", "configuration page title"));
}
