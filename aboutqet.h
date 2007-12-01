/*
	Copyright 2006-2007 Xavier Guerrin
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
#ifndef ABOUTQET_H
#define ABOUTQET_H
#include <QtGui>
/**
	Cette classe represente la boite de dialogue
	� A propos de QElectroTech �
*/
class AboutQET : public QDialog {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	AboutQET(QWidget * = 0);
	virtual ~AboutQET();
	
	private:
	AboutQET(AboutQET &);
	
	// methodes
	private:
	QWidget *titre() const;
	QWidget *ongletAPropos() const;
	QWidget *ongletAuteurs() const;
	QWidget *ongletLicence() const;
};
#endif
