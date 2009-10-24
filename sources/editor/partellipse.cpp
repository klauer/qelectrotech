/*
	Copyright 2006-2009 Xavier Guerrin
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
#include "partellipse.h"
#include "ellipseeditor.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param parent Le QGraphicsItem parent de cette ellipse
	@param scene La scene sur laquelle figure cette ellipse
*/
PartEllipse::PartEllipse(QETElementEditor *editor, QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsEllipseItem(parent, scene), CustomElementGraphicPart(editor) {
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setAcceptedMouseButtons(Qt::LeftButton);
	informations = new EllipseEditor(elementEditor(), this);
	informations -> setElementTypeName(name());
	style_editor -> appendWidget(informations);
	style_editor -> setElementTypeName(name());
}

/// Destructeur
PartEllipse::~PartEllipse() {
}

/**
	Dessine l'ellipse
	@param painter QPainter a utiliser pour rendre le dessin
	@param options Options pour affiner le rendu
	@param widget Widget sur lequel le rendu est effectue
*/
void PartEllipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget) {
	Q_UNUSED(widget);
	applyStylesToQPainter(*painter);
	QPen t = painter -> pen();
	t.setCosmetic(options && options -> levelOfDetail < 1.0);
	if (isSelected()) {
		t.setColor(Qt::red);
	}
	painter -> setPen(t);
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
	Exporte l'ellipse en XML
	@param xml_document Document XML a utiliser pour creer l'element XML
	@return un element XML decrivant l'ellipse
*/
const QDomElement PartEllipse::toXml(QDomDocument &xml_document) const {
	QDomElement xml_element = xml_document.createElement("ellipse");
	QPointF top_left(sceneTopLeft());
	xml_element.setAttribute("x", QString("%1").arg(top_left.x()));
	xml_element.setAttribute("y", QString("%1").arg(top_left.y()));
	xml_element.setAttribute("width",  QString("%1").arg(rect().width()));
	xml_element.setAttribute("height", QString("%1").arg(rect().height()));
	stylesToXml(xml_element);
	return(xml_element);
}

/**
	Importe les proprietes d'une ellipse depuis un element XML
	@param qde Element XML a lire
*/
void PartEllipse::fromXml(const QDomElement &qde) {
	stylesFromXml(qde);
	setRect(
		QRectF(
			mapFromScene(
				qde.attribute("x", "0").toDouble(),
				qde.attribute("y", "0").toDouble()
			),
			QSizeF(
				qde.attribute("width",  "0").toDouble(),
				qde.attribute("height", "0").toDouble()
			)
		)
	);
}

/**
	Specifie la valeur d'une propriete donnee de l'ellipse
	@param property propriete a modifier. Valeurs acceptees :
		* x : abscisse du centre de l'ellipse
		* y : ordonnee du centre de l'ellipse
		* diameter_h : diametre horizontal de l'ellipse
		* diameter_v : diametre vertical de l'ellipse
	@param value Valeur a attribuer a la propriete
*/
void PartEllipse::setProperty(const QString &property, const QVariant &value) {
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
	} else if (property == "diameter_h") {
		qreal new_width = qAbs(value.toDouble());
		QRectF current_rect = rect();
		current_rect.translate((new_width - current_rect.width()) / -2.0, 0.0);
		current_rect.setWidth(new_width);
		setRect(current_rect);
	} else if (property == "diameter_v") {
		qreal new_height = qAbs(value.toDouble());
		QRectF current_rect = rect();
		current_rect.translate(0.0, (new_height - current_rect.height()) / -2.0);
		current_rect.setHeight(new_height);
		setRect(current_rect);
	}
	update();
}

/**
	Permet d'acceder a la valeur d'une propriete donnee de l'ellipse
	@param property propriete lue. Valeurs acceptees :
		* x : abscisse du centre de l'ellipse
		* y : ordonnee du centre de l'ellipse
		* diameter_h : diametre horizontal de l'ellipse
		* diameter_v : diametre vertical de l'ellipse
	@return La valeur de la propriete property
*/
QVariant PartEllipse::property(const QString &property) {
	// appelle la methode property de CustomElementGraphicpart pour les styles
	QVariant style_property = CustomElementGraphicPart::property(property);
	if (style_property != QVariant()) return(style_property);
	
	if (property == "x") {
		return(mapToScene(rect().center()).x());
	} else if (property == "y") {
		return(mapToScene(rect().center()).y());
	} else if (property == "diameter_h") {
		return(rect().width());
	} else if (property == "diameter_v") {
		return(rect().height());
	}
	return(QVariant());
}

/**
	Gere les changements intervenant sur cette partie
	@param change Type de changement
	@param value Valeur numerique relative au changement
*/
QVariant PartEllipse::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (scene()) {
		if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemSelectedChange) {
			informations -> updateForm();
		}
	}
	return(QGraphicsEllipseItem::itemChange(change, value));
}

/**
	@return le coin superieur gauche du rectangle dans lequel s'inscrit
	l'ellipse, dans les coordonnees de la scene.
*/
QPointF PartEllipse::sceneTopLeft() const {
	return(mapToScene(rect().topLeft()));
}

/**
	@return true si cette partie n'est pas pertinente et ne merite pas d'etre
	conservee / enregistree.
	Une ellipse est pertinente des lors que ses dimensions ne sont pas nulles
*/
bool PartEllipse::isUseless() const {
	return(rect().isNull());
}

/**
	@return le rectangle delimitant cette partie.
*/
QRectF PartEllipse::boundingRect() const {
	qreal adjust = 1.5;
	QRectF r(QGraphicsEllipseItem::boundingRect().normalized());
	r.adjust(-adjust, -adjust, adjust, adjust);
	return(r);
}
