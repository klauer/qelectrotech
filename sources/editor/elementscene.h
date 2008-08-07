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
#ifndef ELEMENT_SCENE_H
#define ELEMENT_SCENE_H
#include <QtGui>
#include <QtXml>
#include "nameslistwidget.h"
#include "orientationsetwidget.h"
#include "qgimanager.h"
class QETElementEditor;
class PartLine;
class PartEllipse;
class PartCircle;
class PartPolygon;
class PartArc;
/**
	Cette classe est le canevas permettant l'edition d'un element electrique.
	Elle regroupe les differentes parties composant le dessin de l'element mais
	egalement les informations complementaires : dimensions, orientations,
	noms.
*/
class ElementScene : public QGraphicsScene {
	Q_OBJECT
	
	// enum
	enum Behavior { Normal, Line, Circle, Ellipse, Polygon, Text, Terminal, Arc, TextField };
	
	// constructeurs, destructeur
	public:
	ElementScene(QETElementEditor *, QObject * = 0);
	virtual ~ElementScene();
	
	private:
	ElementScene(const ElementScene &);
	
	// attributs
	public:
	static const int xGrid; ///< Taille horizontale de la grille
	static const int yGrid; ///< Taille verticale de la grille
	
	private:
	/// longueur de l'element en dizaines de pixels
	uint _width;
	/// hauteur de l'element en dizaines de pixels
	uint _height;
	/// position du point de saisie
	QPoint _hotspot;
	/// Liste des noms de l'element
	NamesList _names;
	/// Liste des orientations de l'element
	OrientationSet ori;
	/// booleen indiquant si les bornes de l'element peuvent etre reliees a des bornes de ce meme element
	bool internal_connections;
	/// Gestionnaire de QGraphicsItem
	QGIManager qgi_manager;
	/// Pile des actions annulables
	QUndoStack undo_stack;
	/// Position du premier item selectionne (utilise pour annuler les deplacements)
	QPointF fsi_pos;
	
	/// Variables relatives a la gestion du dessin des parties sur la scene
	Behavior behavior;
	PartLine *current_line;
	PartEllipse *current_ellipse;
	PartCircle *current_circle;
	PartPolygon *current_polygon;
	PartArc *current_arc;
	QETElementEditor *element_editor;
	
	// methodes
	public:
	void setWidth(const uint &);
	uint width() const;
	void setHeight(const uint &);
	uint height() const;
	void setHotspot(const QPoint &);
	QPoint hotspot() const;
	void setNames(const NamesList &);
	NamesList names() const;
	OrientationSet orientations();
	void setOrientations(const OrientationSet &);
	bool internalConnections();
	void setInternalConnections(bool);
	virtual const QDomDocument toXml() const;
	virtual void fromXml(const QDomDocument &);
	virtual void reset();
	virtual QList<QGraphicsItem *> zItems(bool = false) const;
	QRectF sceneContent() const;
	QUndoStack &undoStack();
	QGIManager &qgiManager();
	
	protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
	virtual void drawBackground(QPainter *, const QRectF &);
	virtual void drawForeground(QPainter *, const QRectF &);
	
	public slots:
	void slot_move();
	void slot_addLine();
	void slot_addCircle();
	void slot_addEllipse();
	void slot_addPolygon();
	void slot_addText();
	void slot_addArc();
	void slot_addTerminal();
	void slot_addTextField();
	void slot_selectAll();
	void slot_deselectAll();
	void slot_invertSelection();
	void slot_delete();
	void slot_editSizeHotSpot();
	void slot_editNames();
	void slot_editOrientations();
	void slot_bringForward();
	void slot_raise();
	void slot_lower();
	void slot_sendBackward();
	
	signals:
	/**
		Signal emis lorsque la scene exige que l'editeur d'element repasse
		en mode normal
	*/
	void needNormalMode();
	/// Signal emis lorsqu'une ou plusieurs parties sont ajoutees
	void partsAdded();
	/// Signal emis lorsqu'une ou plusieurs parties sont enlevees
	void partsRemoved();
	/// Signal emis lorsque la zValue d'une ou plusieurs parties change
	void partsZValueChanged();
};

/**
	@param wid Nouvelle largeur de l'element edite
*/
inline void ElementScene::setWidth(const uint &wid) {
	_width = wid;
	while (_width % 10) ++ _width;
	_width /= 10;
}

/**
	@return la largeur de l'element edite
*/
inline uint ElementScene::width() const {
	return(_width * 10);
}

/**
	@param hei Nouvelle hauteur de l'element edite
*/
inline void ElementScene::setHeight(const uint &hei) {
	_height = hei;
	while (_height % 10) ++ _height;
	_height /= 10;
}

/**
	@return la largeur de l'element edite
*/
inline uint ElementScene::height() const {
	return(_height * 10);
}

/**
	@param hs Nouveau point de saisie de l'element edite
*/
inline void ElementScene::setHotspot(const QPoint &hs) {
	_hotspot = hs;
}

/**
	@return le point de saisie de l'element edite
*/
inline QPoint ElementScene::hotspot() const {
	return(_hotspot);
}

/**
	@param nameslist Nouvel ensemble de noms de l'element edite
*/
inline void ElementScene::setNames(const NamesList &nameslist) {
	_names = nameslist;
}

/**
	@return l'ensemble de noms de l'element edite
*/
inline NamesList ElementScene::names() const {
	return(_names);
}

/**
	@return l'ensemble d'orientations de l'element edite
*/
inline OrientationSet ElementScene::orientations() {
	return(ori);
}

/**
	@param orientation_set Nouvel ensemble d'orientations de l'element edite
*/
inline void ElementScene::setOrientations(const OrientationSet &orientation_set) {
	ori = orientation_set;
}

/**
	@return true si les connexions internes sont acceptees, false sinon
*/
inline bool ElementScene::internalConnections() {
	return(internal_connections);
}

/**
	@param ic true pour que les connexions internes soient acceptees, false sinon
*/
inline void ElementScene::setInternalConnections(bool ic) {
	internal_connections = ic;
}

#endif
