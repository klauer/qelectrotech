/*
	Copyright 2006-2013 The QElectroTech Team
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
#ifndef TITLEBLOCK_PROPERTIES_H
#define TITLEBLOCK_PROPERTIES_H
#include <QtCore>
#include <QtXml>
#include "diagramcontext.h"
/**
	This class provides a container for the properties of a particular title
	block, i.e. title, author, date, filename, folio, template, custom
	properties, ...
*/
class TitleBlockProperties {
	public:
	TitleBlockProperties();
	virtual ~TitleBlockProperties();
	/// Lists the various ways to handle the date
	enum DateManagement {
		UseDateValue, ///< use the date attribute
		CurrentDate   ///< use the current date
	};
	
	bool operator==(const TitleBlockProperties &);
	bool operator!=(const TitleBlockProperties &);
	
	void toXml(QDomElement &) const;
	void fromXml(const QDomElement &);
	void toSettings(QSettings &, const QString & = QString()) const;
	void fromSettings(QSettings &, const QString & = QString());
	
	QDate finalDate() const ;
	
	// attributes
	QString title;            ///< Folio title (displayed by the default template)
	QString author;           ///< Author of the diagram/folio (displayed by the default template)
	QDate date;               ///< Date (displayed by the default template)
	QString filename;         ///< Filename (displayed by the default template)
	QString folio;            ///< Folio information (displayed by the default template)
	DateManagement useDate;   ///< Wheter to use the date attribute
	QString template_name;    ///< Name of the template used to render the title block - an empty string means "the default template provided by the application"
	DiagramContext context;   ///< Container for the additional, user-defined fields
	
	private:
	QString exportDate() const;
	void setDateFromString(const QString &);
};
#endif
