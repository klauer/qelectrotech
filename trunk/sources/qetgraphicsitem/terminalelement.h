/*
	Copyright 2006-2016 The QElectroTech Team
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
#ifndef TERMINALELEMENT_H
#define TERMINALELEMENT_H

#include "customelement.h"

class CommentItem;
class QETProject;

class TerminalElement : public CustomElement
{
		Q_OBJECT
	public:
		TerminalElement(const ElementsLocation &, QGraphicsItem * = 0, int * = 0);
		~TerminalElement();
		virtual void initLink(QETProject *project);
		
		signals:

	public slots:
		void updateLabel(DiagramContext old_info, DiagramContext new_info);
		void changeElementInfo();

	private:
		CommentItem *m_comment_item;
		CommentItem *m_location_item;
};

#endif // TERMINALELEMENT_H
