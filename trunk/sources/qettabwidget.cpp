/*
	Copyright 2006-2010 Xavier Guerrin
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

#include "qettabwidget.h"
#include <QCoreApplication>
#include <QWheelEvent>
#include "qettabbar.h"

/**
	Constructeur
	@param parent QWidget parent
*/
QETTabWidget::QETTabWidget(QWidget *parent) :
	QTabWidget(parent)
{
	tab_bar_ = new QETTabBar(this);
	setTabBar(tab_bar_);
	
	// re-emet les signaux emis par la barre d'onglets
	connect(tab_bar_, SIGNAL(lastTabRemoved()),      this, SIGNAL(lastTabRemoved()));
	connect(tab_bar_, SIGNAL(firstTabInserted()),    this, SIGNAL(firstTabInserted()));
	connect(tab_bar_, SIGNAL(tabMoved(int, int)),    this, SIGNAL(tabMoved(int, int)));
	connect(tab_bar_, SIGNAL(tabDoubleClicked(int)), this, SIGNAL(tabDoubleClicked(int)));
}

/**
	Destructeur
*/
QETTabWidget::~QETTabWidget() {
}

/**
	@param movable true pour que les onglets soient deplacables, false sinon
*/
void QETTabWidget::setMovable(bool movable) {
#if QT_VERSION < 0x040500
	tab_bar_ -> setMovable(movable);
#else
	QTabWidget::setMovable(movable);
#endif
}

/**
	@return true si les onglets sont deplacables, false sinon
*/
bool QETTabWidget::isMovable() const {
#if QT_VERSION < 0x040500
	return(tab_bar_ -> isMovable());
#else
	return(QTabWidget::isMovable());
#endif
}

/**
	Gere les evenements rollette sur cette barre d'onglets
	@param event Evenement rollette
*/
void QETTabWidget::wheelEvent(QWheelEvent *event) {
	QTabBar *tab_bar = tabBar();
	// rectangle occupe par la barre d'onglets
	QRect tab_bar_region(QPoint(0, 0), QSize(size().width(), tab_bar -> size().height()));
	if (tab_bar_region.contains(event -> pos())) {
		QCoreApplication::sendEvent(tab_bar, event);
	} else {
		event -> ignore();
	}
}
