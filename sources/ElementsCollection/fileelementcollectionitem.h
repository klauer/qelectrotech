/*
        Copyright 2006-2016 The QElectroTech Team
        This file is part of QElectroTech.

        QElectroTech is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 2 of the License, or
        (at your option) any later version.

        QElectroTech is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FILEELEMENTCOLLECTIONITEM_H
#define FILEELEMENTCOLLECTIONITEM_H

#include "elementcollectionitem.h"
#include <QString>
#include <QDir>
#include <QIcon>

/**
 * @brief The FileElementCollectionItem class
 * This class specialise ElementCollectionItem for manage a collection in
 * a file system. They represente a directory or an element.
 */
class FileElementCollectionItem : public ElementCollectionItem
{
		Q_OBJECT

    public:
		FileElementCollectionItem(ElementCollectionItem *parent = nullptr);
        ~FileElementCollectionItem();

        enum {Type = UserType + 1};
        virtual int type() const {return Type;}

        bool setRootPath(QString path);
		QString fileSystemPath() const;
		QString dirPath() const;

		QString collectionPath() const;
		QString collectionName() const;

		virtual QVariant data(int column, int role);
		virtual void clearData();
		virtual QMimeData *mimeData();
		virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column) const;
		virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column);
		virtual Qt::ItemFlags flags();

		virtual bool isDir() const;
		virtual bool isElement() const;
		virtual bool isCollectionRoot() const;
		bool isCommonCollection() const;
		virtual bool isValid() const;
		virtual QString name();

		virtual void insertNewItem(const QString &collection_name);

    private:
		void setPathName(QString path_name);
		void populate();

    private:
		QString m_path;
		QIcon m_icon;
};

#endif // FILEELEMENTCOLLECTIONITEM_H