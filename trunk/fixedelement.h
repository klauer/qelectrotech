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
#ifndef ELEMENTFIXE_H
#define ELEMENTFIXE_H
#include "element.h"
/**
	Cette classe represente un element avec un nombre fixe de bornes.
*/
class FixedElement : public Element {
	
	// constructeurs, destructeur
	public:
	FixedElement(QGraphicsItem * = 0, Diagram * = 0);
	virtual ~FixedElement();
	
	// methodes
	public:
	int nbTerminalsMin() const;
	int nbTerminalsMax() const;
	virtual int nbTerminals() const = 0;
	virtual void paint(QPainter *, const QStyleOptionGraphicsItem *) = 0;
	virtual QString typeId() const = 0;
	virtual QString  nom() const = 0;
};
#endif
