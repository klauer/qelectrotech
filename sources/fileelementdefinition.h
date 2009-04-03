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
#ifndef FILE_ELEMENT_DEFINITION
#define FILE_ELEMENT_DEFINITION
#include <QtCore>
#include "elementdefinition.h"
class FileElementsCategory;
class FileElementsCollection;
/**
	Cette classe represente la definition d'un element stockee dans un fichier.
	
*/
class FileElementDefinition : public ElementDefinition {
	public:
	FileElementDefinition(const QString &, FileElementsCategory * = 0, FileElementsCollection * = 0);
	virtual ~FileElementDefinition();
	
	private:
	FileElementDefinition(const FileElementDefinition &);
	
	// methodes
	public:
	virtual QDomElement xml();
	virtual bool setXml(const QDomElement &);
	virtual bool write();
	virtual bool isNull() const;
	virtual QString pathName() const;
	virtual QString virtualPath();
	virtual void reload();
	virtual bool exists();
	virtual bool isReadable();
	virtual bool isWritable();
	virtual bool remove();
	virtual bool hasFilePath();
	virtual QString filePath();
	virtual void setFilePath(const QString &);
	
	// attributs
	private:
	bool is_null;
	QString file_path;
	QDomDocument xml_element_;
};
#endif
