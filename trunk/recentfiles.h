/*
	Copyright 2006-2008 Xavier Guerrin
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
#ifndef RECENT_FILES_H
#define RECENT_FILES_H
#include <QtCore>
class QMenu;
/**
	Cette classe permet de gerer des fichiers recents.
*/
class RecentFiles : public QObject {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	RecentFiles(const QString &, int = 10, QObject * = 0);
	virtual ~RecentFiles();
	private:
	RecentFiles(const RecentFiles &);
	
	// methodes
	public:
	int size() const;
	QMenu *menu() const;
	
	public slots:
	void clear();
	void save();
	void fileWasOpened(const QString &);
	
	signals:
	void fileOpeningRequested(const QString &);
	
	private:
	void extractFilesFromSettings();
	void insertFile(const QString &);
	void saveFilesToSettings();
	void buildMenu();
	
	private slots:
	void handleMenuRequest(const QString &);
	
	// attributs
	private:
	QString identifier_;
	int size_;
	QList<QString> list_;
	QMenu *menu_;
	QSignalMapper *mapper_;
};
#endif
