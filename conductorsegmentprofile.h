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
#ifndef CONDUCTOR_SEGMENT_PROFILE_H
#define CONDUCTOR_SEGMENT_PROFILE_H
#include <QtCore>
#include "conductorsegment.h"
/**
	Cette classe contient le profil (= les caracteristiques essentielles) d'un
	segment de conducteur.
*/
class ConductorSegmentProfile {
	// constructeurs, destructeur
	public:
	/**
		Constructeur
		@param l longueur du segment
		@param ori true si le segment est horizontal, false s'il est vertical
	*/
	ConductorSegmentProfile(qreal l, bool ori = true) :
		length(l),
		isHorizontal(ori)
	{
	}
	
	/**
		Constructeur
		@param segment ConductorSegment dont il faut extraire le profil
	*/
	ConductorSegmentProfile(ConductorSegment *segment) :
		length(segment -> length()),
		isHorizontal(segment -> isHorizontal())
	{
	}
	
	/// Destructeur
	virtual ~ConductorSegmentProfile() {
	}
	
	// attributs
	public:
	/// longueur du segment
	qreal length;
	/// orientation du segment
	bool isHorizontal;
};
#endif
