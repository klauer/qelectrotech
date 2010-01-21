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
#include "partpolygon.h"
#include "qet.h"
#include "polygoneditor.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param parent Le QGraphicsItem parent de ce polygone
	@param scene La scene sur laquelle figure ce polygone
*/
PartPolygon::PartPolygon(QETElementEditor *editor, QGraphicsItem *parent, QGraphicsScene *scene) : 
	QGraphicsPolygonItem(parent, scene),
	CustomElementGraphicPart(editor),
	closed(false)
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
#if QT_VERSION >= 0x040600
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
	setAcceptedMouseButtons(Qt::LeftButton);
	informations = new PolygonEditor(elementEditor(), this);
	informations -> setElementTypeName(name());
	style_editor -> appendWidget(informations);
	style_editor -> setElementTypeName(name());
}

/// Destructeur
PartPolygon::~PartPolygon() {
}

/**
	Importe les proprietes d'un polygone depuis un element XML
	@param qde Element XML a lire
*/
void PartPolygon::fromXml(const QDomElement &qde) {
	stylesFromXml(qde);
	int i = 1;
	while(true) {
		if (
			QET::attributeIsAReal(qde, QString("x%1").arg(i)) &&\
			QET::attributeIsAReal(qde, QString("y%1").arg(i))
		) ++ i;
		else break;
	}
	
	QPolygonF temp_polygon;
	for (int j = 1 ; j < i ; ++ j) {
		temp_polygon << QPointF(
			qde.attribute(QString("x%1").arg(j)).toDouble(),
			qde.attribute(QString("y%1").arg(j)).toDouble()
		);
	}
	setPolygon(temp_polygon);
	
	closed = qde.attribute("closed") != "false";
}

/**
	Exporte le polygone en XML
	@param xml_document Document XML a utiliser pour creer l'element XML
	@return un element XML decrivant le polygone
*/
const QDomElement PartPolygon::toXml(QDomDocument &xml_document) const {
	QDomElement xml_element = xml_document.createElement("polygon");
	int i = 1;
	foreach(QPointF point, polygon()) {
		point = mapToScene(point);
		xml_element.setAttribute(QString("x%1").arg(i), point.x());
		xml_element.setAttribute(QString("y%1").arg(i), point.y());
		++ i;
	}
	if (!closed) xml_element.setAttribute("closed", "false");
	stylesToXml(xml_element);
	return(xml_element);
}

/**
	Dessine le polygone
	@param painter QPainter a utiliser pour rendre le dessin
	@param options Options pour affiner le rendu
	@param widget Widget sur lequel le rendu est effectue
*/
void PartPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget) {
	Q_UNUSED(widget);
	applyStylesToQPainter(*painter);
	QPen t = painter -> pen();
	t.setCosmetic(options && options -> levelOfDetail < 1.0);
	if (isSelected()) t.setColor(Qt::red);
	painter -> setPen(t);
	if (closed) painter -> drawPolygon(polygon());
	else painter -> drawPolyline(polygon());
}

/**
	Specifie la valeur d'une propriete donnee du polygone
	@param property propriete a modifier. Valeurs acceptees :
		* closed : true pour fermer le polygone, false sinon
	@param value Valeur a attribuer a la propriete
*/
void PartPolygon::setProperty(const QString &property, const QVariant &value) {
	CustomElementGraphicPart::setProperty(property, value);
	if (property == "closed") closed = value.toBool();
	update();
}

/**
	Permet d'acceder a la valeur d'une propriete donnee de la ligne
	@param property propriete lue. Valeurs acceptees :
		* closed : true pour fermer le polygone, false sinon
	@return La valeur de la propriete property
*/
QVariant PartPolygon::property(const QString &property) {
	// appelle la methode property de CustomElementGraphicpart pour les styles
	QVariant style_property = CustomElementGraphicPart::property(property);
	if (style_property != QVariant()) return(style_property);
	
	if (property == "closed") return(closed);
	return(QVariant());
}

/**
	Gere les changements intervenant sur cette partie
	@param change Type de changement
	@param value Valeur numerique relative au changement
*/
QVariant PartPolygon::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (scene()) {
		if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemSelectedChange) {
			informations -> updateForm();
		}
	}
	return(QGraphicsPolygonItem::itemChange(change, value));
}


/**
	@return true si cette partie n'est pas pertinente et ne merite pas d'etre
	conservee / enregistree.
	Un polygone est pertinent des lors qu'il possede deux points differents.
*/
bool PartPolygon::isUseless() const {
	QPolygonF poly(polygon());
	
	if (polygon().count() < 2) return(true);
	
	QPointF previous_point;
	for (int i = 1 ; i < poly.count() ; ++ i) {
		if (poly[i] != poly[i-1]) return(false);
	}
	
	return(true);
}

/**
	@return le rectangle delimitant cette partie.
*/
QRectF PartPolygon::boundingRect() const {
	qreal adjust = 1.5;
	QRectF r(QGraphicsPolygonItem::boundingRect());
	r.adjust(-adjust, -adjust, adjust, adjust);
	return(r);
}
