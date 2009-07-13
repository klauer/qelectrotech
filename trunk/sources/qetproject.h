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
#ifndef QET_PROJECT_H
#define QET_PROJECT_H
#include <QtCore>
#include <QtXml>
#include "nameslist.h"
#include "elementslocation.h"
#include "borderproperties.h"
#include "conductorproperties.h"
#include "insetproperties.h"
class Diagram;
class ElementsCollection;
class ElementsCategory;
class ElementDefinition;
class ElementsLocation;
class XmlElementsCollection;
class MoveElementsHandler;
/**
	Cette classe represente un projet QET. Typiquement enregistre dans un
	fichier, il s'agit d'un document XML integrant des schemas ainsi qu'une
	collection d'elements embarques. Ce dernier attribut permet d'exporter des
	schema sur d'autres machines sans se soucier de la presence des elements
	sur celle-ci.
	La classe QETProject gere l'enregistrement partiel. Ainsi, enregistrer les
	modifications apportees a un element reenregistrera le fichier mais en
	mettant a jour seulement les parties du document XML concerne.
*/
class QETProject : public QObject {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	QETProject(int = 1, QObject * = 0);
	QETProject(const QString &, QObject * = 0);
	QETProject(const QDomElement &, QObject * = 0);
	virtual ~QETProject();
	
	private:
	QETProject(const QETProject &);
	
	// enums
	public:
	/**
		Represente l'etat du projet
	*/
	enum ProjectState {
		Ok                   = 0, /// Le projet n'est pas en erreur
		FileOpenFailed       = 1, /// l'ouverture d'un fichier a echoue
		XmlParsingFailed     = 2, /// l'analyse XML a echoue
		ProjectParsingFailed = 3  /// la lecture en tant que projet a echoue
	};
	
	// methodes
	public:
	ProjectState state() const;
	QList<Diagram *> diagrams() const;
	ElementsCollection *embeddedCollection() const;
	QString filePath();
	void setFilePath(const QString &);
	QString currentDir() const;
	QString pathNameTitle() const;
	QString title() const;
	void setTitle(const QString &);
	BorderProperties defaultBorderProperties() const;
	void setDefaultBorderProperties(const BorderProperties &);
	InsetProperties defaultInsetProperties() const;
	void setDefaultInsetProperties(const InsetProperties &);
	ConductorProperties defaultConductorProperties() const;
	void setDefaultConductorProperties(const ConductorProperties &);
	QDomDocument toXml();
	bool close();
	bool write();
	bool isReadOnly() const;
	void setReadOnly(bool);
	bool isEmpty() const;
	bool ensureIntegrationCategoryExists();
	ElementsCategory *integrationCategory() const;
	QString integrateElement(const QString &, QString &);
	QString integrateElement(const QString &, MoveElementsHandler *, QString &);
	bool usesElement(const ElementsLocation &);
	void cleanUnusedElements(MoveElementsHandler *);
	void cleanEmptyCategories(MoveElementsHandler *);
	bool projectWasModified();
	bool embeddedCollectionWasModified();
	bool diagramsWereModified();
	
	public slots:
	void componentWritten();
	Diagram *addNewDiagram();
	void removeDiagram(Diagram *);
	void diagramOrderChanged(int, int);
	
	signals:
	void projectFilePathChanged(QETProject *, const QString &);
	void projectTitleChanged(QETProject *, const QString &);
	void projectInformationsChanged(QETProject *);
	void diagramAdded(QETProject *, Diagram *);
	void diagramRemoved(QETProject *, Diagram *);
	void readOnlyChanged(QETProject *, bool);
	
	private slots:
	void updateDiagramsFolioData();
	
	private:
	ElementsCategory *rootCategory() const;
	void readProjectXml();
	void readDiagramsXml();
	void readElementsCollectionXml();
	void readDefaultPropertiesXml();
	void writeDefaultPropertiesXml(QDomElement &);
	void addDiagram(Diagram *);
	NamesList namesListForIntegrationCategory();
	ElementsLocation copyElementWithHandler(ElementDefinition *, ElementsCategory *, MoveElementsHandler *, QString &);
	
	// attributs
	private:
	/// Chemin du fichier dans lequel ce projet est enregistre
	QString file_path_;
	/// Etat du projet
	ProjectState state_;
	/// Element XML representant le projet
	QDomDocument document_root_;
	/// Schemas contenus dans le projet
	QList<Diagram *> diagrams_;
	/// Collection d'elements embarquee
	XmlElementsCollection *collection_;
	/// Titre du projet
	QString project_title_;
	/// booleen indiquant si le projet est en ReadOnly ou non
	bool read_only_;
	/// Chemin du fichier pour lequel ce projet est considere comme etant en lecture seule
	QString read_only_file_path_;
	/// Nom de la categorie a utiliser pour l'integration automatique des elements
	static QString integration_category_name;
	/// Dimensions par defaut pour les nouveaux schemas dans ce projet
	BorderProperties default_border_properties_;
	/// Proprietes par defaut des conducteurs pour les nouveaux schemas dans ce projet
	ConductorProperties default_conductor_properties_;
	/// Proprietes par defaut du cartouche pour les nouveaux schemas dans ce projet
	InsetProperties default_inset_properties_;
};
#endif
