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
#ifndef QET_PROJECT_H
#define QET_PROJECT_H

#include "nameslist.h"
#include "elementslocation.h"
#include "borderproperties.h"
#include "conductorproperties.h"
#include "titleblockproperties.h"
#include "templatescollection.h"
#include "properties/xrefproperties.h"

class Diagram;
class ElementsLocation;
class QETResult;
class TitleBlockTemplate;
class MoveTitleBlockTemplatesHandler;
class NumerotationContext;
class QUndoStack;
class XmlElementCollection;
class QTimer;

/**
	This class represents a QET project. Typically saved as a .qet file, it
	consists in an XML document grouping 0 to n diagrams and embedding an elements
	collection. This collection enables users to export diagrams on remote
	machines without wondering whether required elements are available to them.
*/
class QETProject : public QObject
{
		Q_OBJECT

		Q_PROPERTY(bool autoConductor READ autoConductor WRITE setAutoConductor)
	
		// constructors, destructor
	public:
		QETProject (int = 1, QObject * = nullptr);
		QETProject (const QString &, QObject * = nullptr);
		~QETProject() override;
	
	private:
		QETProject(const QETProject &);
	
		// enums
	public:
	/**
		This enum lists possible states for a particular project.
	*/
	enum ProjectState {
		Ok                    = 0, /// no error
		FileOpenFailed        = 1, /// file opening failed
		XmlParsingFailed      = 2, /// XML parsing failed
		ProjectParsingRunning = 3, /// the XML content is currently being processed
		ProjectParsingFailed  = 4, /// the parsing of the XML content failed
		FileOpenDiscard       = 5  /// the user cancelled the file opening
	};
	
		// methods
	public:
		ProjectState state() const;
		QList<Diagram *> diagrams() const;
		int getFolioSheetsQuantity() const; /// get the folio sheets quantity for this project
		void setFolioSheetsQuantity(int);   /// set the folio sheets quantity for this project
		int folioIndex(const Diagram *) const;
		XmlElementCollection *embeddedElementCollection()const;
		TitleBlockTemplatesProjectCollection *embeddedTitleBlockTemplatesCollection();
		QString filePath();
		void setFilePath(const QString &);
		QString currentDir() const;
		QString pathNameTitle() const;
		QString title() const;
		qreal declaredQElectroTechVersion();
		void setTitle(const QString &);

			///DEFAULT PROPERTIES
		BorderProperties defaultBorderProperties() const;
		void             setDefaultBorderProperties(const BorderProperties &);

		TitleBlockProperties defaultTitleBlockProperties() const;
		void                 setDefaultTitleBlockProperties(const TitleBlockProperties &);

		ConductorProperties defaultConductorProperties() const;
		void                setDefaultConductorProperties(const ConductorProperties &);

		QString defaultReportProperties() const;
		void    setDefaultReportProperties (const QString &properties);

		XRefProperties					defaultXRefProperties (const QString &type) const {return m_default_xref_properties[type];}
		QHash <QString, XRefProperties> defaultXRefProperties() const					  {return m_default_xref_properties;}
		void setDefaultXRefProperties(const QString& type, const XRefProperties &properties);
		void setDefaultXRefProperties(QHash <QString, XRefProperties> hash);

		QHash <QString, NumerotationContext> conductorAutoNum() const;
		QHash <QString, NumerotationContext> elementAutoNum() const;
		QHash <QString, NumerotationContext> folioAutoNum() const;
		void addConductorAutoNum (const QString& key, const NumerotationContext& context);
		void addElementAutoNum (const QString& key, const NumerotationContext& context);
		void addFolioAutoNum     (const QString& key, const NumerotationContext& context);
		void removeConductorAutoNum (const QString& key);
		void removeElementAutoNum (const QString& key);
		void removeFolioAutoNum (const QString& key);
		NumerotationContext conductorAutoNum(const QString &key) const;
		NumerotationContext folioAutoNum(const QString &key)     const;
		NumerotationContext elementAutoNum(const QString &key);

		QString conductorAutoNumFormula(const QString& key) const; //returns Formula
		QString conductorCurrentAutoNum() const;
		void setCurrentConductorAutoNum(QString autoNum);

		QString elementAutoNumFormula(const QString& key) const;
		QString elementAutoNumCurrentFormula() const;
		QString elementCurrentAutoNum() const;
		void setCurrrentElementAutonum(QString autoNum);

			//Element
		void freezeExistentElementLabel(bool freeze, int from, int to);
		void freezeNewElementLabel(bool freeze, int from, int to);
		bool isFreezeNewElements();
		void setFreezeNewElements(bool);

			//Conductor
		void freezeExistentConductorLabel(bool freeze, int from, int to);
		void freezeNewConductorLabel(bool freeze, int from, int to);
		bool isFreezeNewConductors();
		void setFreezeNewConductors(bool);

		bool autoConductor () const;
		bool autoElement () const;
		bool autoFolio () const;
		void setAutoConductor (bool ac);
		void setAutoElement (bool ae);
		void autoFolioNumberingNewFolios ();
		void autoFolioNumberingSelectedFolios(int, int, const QString&);

		QDomDocument toXml();
		bool close();
		QETResult write();
		bool isReadOnly() const;
		void setReadOnly(bool);
		bool isEmpty() const;
		ElementsLocation importElement(ElementsLocation &location);
		QString integrateTitleBlockTemplate(const TitleBlockTemplateLocation &, MoveTitleBlockTemplatesHandler *handler);
		bool usesElement(const ElementsLocation &) const;
		QList <ElementsLocation> unusedElements() const;
		bool usesTitleBlockTemplate(const TitleBlockTemplateLocation &);
		bool projectWasModified();
		bool projectOptionsWereModified();
		DiagramContext projectProperties();
		void setProjectProperties(const DiagramContext &);
		QUndoStack* undoStack() {return m_undo_stack;}
	
	public slots:
		Diagram *addNewDiagram();
		QList <Diagram *> addNewDiagramFolioList();
		void removeDiagram(Diagram *);
		void diagramOrderChanged(int, int);
		void setModified(bool);
	
	signals:
		void projectFilePathChanged(QETProject *, const QString &);
		void projectTitleChanged(QETProject *, const QString &);
		void projectInformationsChanged(QETProject *);
		void diagramAdded(QETProject *, Diagram *);
		void diagramRemoved(QETProject *, Diagram *);
		void projectModified(QETProject *, bool);
		void projectDiagramsOrderChanged(QETProject *, int, int);
		void diagramUsedTemplate(TitleBlockTemplatesCollection *, const QString &);
		void readOnlyChanged(QETProject *, bool);
		void reportPropertiesChanged(const QString &old_str, const QString &new_str);
		void XRefPropertiesChanged ();
		void addAutoNumDiagram();
		void elementAutoNumAdded(QString name);
		void elementAutoNumRemoved(QString name);
		void conductorAutoNumAdded();
		void conductorAutoNumRemoved();
		void folioAutoNumAdded();
		void folioAutoNumRemoved();
		void folioAutoNumChanged(QString);
		void defaultTitleBlockPropertiesChanged();
		void conductorAutoNumChanged();
	
	private slots:
		void updateDiagramsFolioData();
		void updateDiagramsTitleBlockTemplate(TitleBlockTemplatesCollection *, const QString &);
		void removeDiagramsTitleBlockTemplate(TitleBlockTemplatesCollection *, const QString &);
		void usedTitleBlockTemplateChanged(const QString &);
		void undoStackChanged (bool a) {if (!a) setModified(true);}
	
	private:
		void setupTitleBlockTemplatesCollection();
		void readProjectXml(QDomDocument &xml_project);
		void readDiagramsXml(QDomDocument &xml_project);
		void readElementsCollectionXml(QDomDocument &xml_project);
		void readProjectPropertiesXml(QDomDocument &xml_project);
		void readDefaultPropertiesXml(QDomDocument &xml_project);

		void writeProjectPropertiesXml(QDomElement &);
		void writeDefaultPropertiesXml(QDomElement &);
		void addDiagram(Diagram *);
		NamesList namesListForIntegrationCategory();
		void writeBackup();
	
	// attributes
	private:
			/// File path this project is saved to
		QString m_file_path;
			/// Current state of the project
		ProjectState state_;
			/// Diagrams carried by the project
		QList<Diagram *> m_diagrams_list;
			/// Project title
		QString project_title_;
			/// QElectroTech version declared in the XML document at opening time
		qreal project_qet_version_;
			/// Whether options were modified
		bool modified_;
			/// Whether the project is read only
		bool read_only_;
			/// Filepath for which this project is considered read only
		QString read_only_file_path_;
			/// Default dimensions and properties for new diagrams created within the project
		BorderProperties default_border_properties_;
			/// Default conductor properties for new diagrams created within the project
		ConductorProperties default_conductor_properties_;
			/// Default title block properties for new diagrams created within the project
		TitleBlockProperties default_titleblock_properties_;
			/// Default report properties
		QString m_default_report_properties;
			/// Default xref properties
		QHash <QString, XRefProperties> m_default_xref_properties;
			/// Embedded title block templates collection
		TitleBlockTemplatesProjectCollection titleblocks_;
			/// project-wide variables that will be made available to child diagrams
		DiagramContext project_properties_;
			/// undo stack for this project
		QUndoStack *m_undo_stack;
			/// Conductor auto numerotation
		QHash <QString, NumerotationContext> m_conductor_autonum;//Title and NumContext hash
		QString m_current_conductor_autonum;
			/// Folio auto numbering
		QHash <QString, NumerotationContext> m_folio_autonum;
			/// Element Auto Numbering
		QHash <QString, NumerotationContext> m_element_autonum; //Title and NumContext hash
		QString m_current_element_autonum;
			/// Folio List Sheets quantity for this project.
		int folioSheetsQuantity;
		bool m_auto_conductor;
		XmlElementCollection *m_elements_collection;
		bool m_freeze_new_elements;
		bool m_freeze_new_conductors;
		QTimer m_save_backup_timer,
			   m_autosave_timer;
};

Q_DECLARE_METATYPE(QETProject *)
#endif
