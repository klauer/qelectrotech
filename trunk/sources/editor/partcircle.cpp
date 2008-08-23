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
#include "partcircle.h"
#include "circleeditor.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param parent Le QGraphicsItem parent de ce cercle
	@param scene La scene sur laquelle figure ce cercle
*/
PartCircle::PartCircle(QETElementEditor *editor, QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsEllipseItem(parent, scene), CustomElementGraphicPart(editor) {
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setAcceptedMouseButtons(Qt::LeftButton);
	informations = new CircleEditor(elementEditor(), this);
	informations -> setElementTypeName(name());
	style_editor -> appendWidget(informations);
	style_editor -> setElementTypeName(name());
}

/// Destructeur
PartCircle::~PartCircle() {
}

/**
	Dessine le cercle
	@param painter QPainter a utiliser pour rendre le dessin
	@param options Options pour affiner le rendu
	@param widget Widget sur lequel le rendu est effectue
*/
void PartCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	applyStylesToQPainter(*painter);
	QPen t = painter -> pen();
	if (isSelected()) {
		t.setColor(Qt::red);
		painter -> setPen(t);
	}
	painter -> drawEllipse(rect());
	if (isSelected()) {
		painter -> setRenderHint(QPainter::Antialiasing, false);
		painter -> setPen((painter -> brush().color() == QColor(Qt::black) && painter -> brush().isOpaque()) ? Qt::yellow : Qt::blue);
		QPointF center = rect().center();
		painter -> drawLine(QLineF(center.x() - 2.0, center.y(), center.x() + 2.0, center.y()));
		painter -> drawLine(QLineF(center.x(), center.y() - 2.0, center.x(), center.y() + 2.0));
	}
}

/**
	Exporte le cercle en XML
	@param xml_document Document XML a utiliser pour creer l'element XML
	@return un element XML decrivant le cercle
*/
const QDomElement PartCircle::toXml(QDomDocument &xml_document) const {
	QDomElement xml_element = xml_document.createElement("circle");
	QPointF top_left(sceneTopLeft());
	xml_element.setAttribute("x", QString("%1").arg(top_left.x()));
	xml_element.setAttribute("y", QString("%1").arg(top_left.y()));
	xml_element.setAttribute("diameter",  QString("%1").arg(rect().width()));
	stylesToXml(xml_element);
	return(xml_element);
}

/**
	Importe les proprietes d'un cercle depuis un element XML
	@param qde Element XML a lire
*/
void PartCircle::fromXml(const QDomElement &qde) {
	stylesFromXml(qde);
	qreal diameter = qde.attribute("diameter",  "0").toDouble();
	setRect(
		QRectF(
			mapFromScene(
				qde.attribute("x", "0").toDouble(),
				qde.attribute("y", "0").toDouble()
			),
			QSizeF(
				diameter,
				diameter
			)
		)
	);
}

/**
	Specifie la valeur d'une propriete donnee du cercle
	@param property propriete a modifier. Valeurs acceptees :
		* x : abscisse du centre du cercle
		* y : ordonnee du centre du cercle
		* diameter : diametre du cercle
	@param value Valeur a attribuer a la propriete
*/
void PartCircle::setProperty(const QString &property, const QVariant &value) {
	CustomElementGraphicPart::setProperty(property, value);
	if (!value.canConvert(QVariant::Double)) return;
	if (property == "x") {
		QRectF current_rect = rect();
		QPointF current_pos = mapToScene(current_rect.center());
		setRect(current_rect.translated(value.toDouble() - current_pos.x(), 0.0));
	} else if (property == "y") {
		QRectF current_rect = rect();
		QPointF current_pos = mapToScene(current_rect.center());
		setRect(current_rect.translated(0.0, value.toDouble() - current_pos.y()));
	} else if (property == "diameter") {
		QRectF current_rect = rect();
		qreal new_diameter = qAbs(value.toDouble());
		current_rect.translate(
			(new_diameter - current_rect.width()) / -2.0,
			(new_diameter - current_rect.height()) / -2.0
		);
		current_rect.setSize(QSizeF(new_diameter, new_diameter));
		setRect(current_rect);
	}
}

/**
	Permet d'acceder a la valeur d'une propriete de style donnee.
	@param property propriete lue. Valeurs acceptees :
		* x : abscisse du centre du cercle
		* y : ordonnee du centre du cercle
		* diameter : diametre du cercle
	@return La valeur de la propriete property
*/
QVariant PartCircle::property(const QString &property) {
	// appelle la methode property de CustomElementGraphicpart pour les styles
	QVariant style_property = CustomElementGraphicPart::property(property);
	if (style_property != QVariant()) return(style_property);
	
	if (property == "x") {
		return(mapToScene(rect().center()).x());
	} else if (property == "y") {
		return(mapToScene(rect().center()).y());
	} else if (property == "diameter") {
		return(rect().width());
	}
	return(QVariant());
}

/**
	Gere les changements intervenant sur cette partie
	@param change Type de changement
	@param value Valeur numerique relative au changement
*/
QVariant PartCircle::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (scene()) {
		if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemSelectedChange) {
			informations -> updateForm();
		}
	}
	return(QGraphicsEllipseItem::itemChange(change, value));
}

/**
	@return le coin superieur gauche du rectangle dans lequel s'inscrit
	le cercle, dans les coordonnees de la scene.
*/
QPointF PartCircle::sceneTopLeft() const {
	return(mapToScene(rect().topLeft()));
}

/**
	@return le centre du cercle, dans les coordonnees de la scene.
*/
QPointF PartCircle::sceneCenter() const {
	return(mapToScene(rect().center()));
}

/**
	@return le rectangle delimitant cette partie.
*/
QRectF PartCircle::boundingRect() const {
	qreal adjust = 1.5;
	QRectF r(QGraphicsEllipseItem::boundingRect());
	r.adjust(-adjust, -adjust, adjust, adjust);
	return(r);
}

/**
	@return true si cette partie n'est pas pertinente et ne merite pas d'etre
	conservee / enregistree.
	Un cercle est pertinent des lors que son rayon n'est pas nul
*/
bool PartCircle::isUseless() const {
	return(rect().isNull());
}
