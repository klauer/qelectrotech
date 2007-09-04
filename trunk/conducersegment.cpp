#include "conducersegment.h"
#include <QDebug>

/**
	Constructeur
	@param p1 Le point 
	@param p2 
	@param cs1 Le segment precedent
	@param cs2 Le segment suivant
*/
ConducerSegment::ConducerSegment(
	const QPointF &p1,
	const QPointF &p2,
	ConducerSegment *cs1,
	ConducerSegment *cs2
) :
	point1(p1),
	point2(p2)
{
	setPreviousSegment(cs1);
	setNextSegment(cs2);
}

/**
	Destructeur - Relie le segment precedent au suivant
*/
ConducerSegment::~ConducerSegment() {
	//qDebug() << "~ConducerSegment()" << (void *)this;
	if (hasPreviousSegment()) previousSegment() -> setNextSegment(nextSegment());
	if (hasNextSegment()) nextSegment() -> setPreviousSegment(previousSegment());
}


/**
	Permet de savoir s'il est possible de deplacer le premier point du segment
	sans creer d'incoherence. La valeur du mouvement maximum qu'il est possible de faire
	sans incoherence est stockee dans le second parametre.
	@param asked_dx La valeur du mouvement demande
	@param possible_dx La valeur du mouvement possible (au maximum)
	@return true si le mouvement est possible ; false s'il doit etre limite
*/
bool ConducerSegment::canMove1stPointX(const qreal &asked_dx, qreal &possible_dx) const {
	
	Q_ASSERT_X(isVertical(), "ConducerSegment::canMove1stPointX", "segment non vertical");
	
	/// On ne bouge jamais le premier point d'un segment statique.
	if (isStatic()) {
		possible_dx = 0.0;
		return(false);
	}
	// a ce stade, on a forcement un segment precedent
	
	/// Si le segment precedent n'est pas statique, le mouvement est possible.
	if (previous_segment -> hasPreviousSegment()) {
		possible_dx = asked_dx;
		return(true);
	}
	// a ce stade, le segment precedent est forcement statique
	
	/// Si le segment precedent est vertical, le mouvement est possible :
	/// il induira la creation d'un segment horizontal supplementaire.
	if (previous_segment -> isVertical()) {
		possible_dx = asked_dx;
		return(true);
	}
	// a ce stade, le segment precedent est forcement horizontal
	
	// recupere quelques donnees
	qreal prev_segment_first_x = previous_segment -> point1.x();
	qreal first_x              = point1.x();
	
	/// Il se peut que le mouvement doive etre limite de facon a ce
	/// que le segment statique conserve une taille minimale.
	if (previous_segment -> length() > 0.0) {
		if (first_x + asked_dx < prev_segment_first_x + 12.0) {
			possible_dx = -first_x + prev_segment_first_x + 12.0;
			return(false);
		} else {
			possible_dx = asked_dx;
			return(true);
		}
	} else {
		if (first_x + asked_dx >= prev_segment_first_x - 12.0) {
			possible_dx = prev_segment_first_x - 12.0 - first_x;
			return(false);
		} else {
			possible_dx = asked_dx;
			return(true);
		}
	}
}

/**
	Permet de savoir s'il est possible de deplacer le second point du segment
	sans creer d'incoherence. La valeur du mouvement maximum qu'il est possible de faire
	sans incoherence est stockee dans le second parametre.
	@param asked_dx La valeur du mouvement demande
	@param possible_dx La valeur du mouvement possible (au maximum)
	@return true si le mouvement est possible ; false s'il doit etre limite
*/
bool ConducerSegment::canMove2ndPointX(const qreal &asked_dx, qreal &possible_dx) const {
	
	Q_ASSERT_X(isVertical(), "ConducerSegment::canMove2ndPointX", "segment non vertical");
	
	/// On ne modifie jamais l'abscisse du second point d'un segment statique.
	if (isStatic()) {
		possible_dx = 0.0;
		return(false);
	}
	// a ce stade, on a forcement un segment suivant
	
	/// Si le segment suivant n'est pas statique, le mouvement est possible.
	if (next_segment -> hasNextSegment()) {
		possible_dx = asked_dx;
		return(true);
	}
	// a ce stade, le segment suivant est forcement statique
	
	/// Si le segment suivant est vertical, le mouvement est possible :
	/// il induira la creation d'un segment horizontal supplementaire.
	if (next_segment -> isVertical()) {
		possible_dx = asked_dx;
		return(true);
	}
	// a ce stade, le segment suivant est forcement horizontal
	
	// recupere quelques donnees
	qreal next_segment_second_x = next_segment -> point2.x();
	qreal second_x              = point2.x();
	
	/// Il se peut que le mouvement doive etre limite de facon a ce
	/// que le segment statique conserve une taille minimale.
	if (next_segment -> length() < 0.0) {
		if (second_x + asked_dx < next_segment_second_x + 12.0) {
			possible_dx = -second_x + next_segment_second_x + 12.0;
			return(false);
		} else {
			possible_dx = asked_dx;
			return(true);
		}
	} else {
		if (second_x + asked_dx >= next_segment_second_x - 12.0) {
			possible_dx = next_segment_second_x - 12.0 - second_x;
			return(false);
		} else {
			possible_dx = asked_dx;
			return(true);
		}
	}
}

/**
	Permet de savoir s'il est possible de deplacer le premier point du segment
	sans creer d'incoherence. La valeur du mouvement maximum qu'il est possible de faire
	sans incoherence est stockee dans le second parametre.
	@param asked_dy La valeur du mouvement demande
	@param possible_dy La valeur du mouvement possible (au maximum)
	@return true si le mouvement est possible ; false s'il doit etre limite
*/
bool ConducerSegment::canMove1stPointY(const qreal &asked_dy, qreal &possible_dy) const {
	
	Q_ASSERT_X(isHorizontal(), "ConducerSegment::canMove1stPointY", "segment non horizontal");
	
	/// On ne bouge jamais le premier point d'un segment statique.
	if (isStatic()) {
		possible_dy = 0.0;
		return(false);
	}
	// a ce stade, on a forcement un segment precedent
	
	/// Si le segment precedent n'est pas statique, le mouvement est possible.
	if (previous_segment -> hasPreviousSegment()) {
		possible_dy = asked_dy;
		return(true);
	}
	// a ce stade, le segment precedent est forcement statique
	
	/// Si le segment precedent est horizontal, le mouvement est possible :
	/// il induira la creation d'un segment vertical supplementaire.
	if (previous_segment -> isHorizontal()) {
		possible_dy = asked_dy;
		return(true);
	}
	// a ce stade, le segment precedent est forcement vertical
	
	// recupere quelques donnees
	qreal prev_segment_first_y = previous_segment -> point1.y();
	qreal first_y              = point1.y();
	
	/// Il se peut que le mouvement doive etre limite de facon a ce
	/// que le segment statique conserve une taille minimale.
	if (previous_segment -> length() > 0.0) {
		if (first_y + asked_dy < prev_segment_first_y + 12.0) {
			possible_dy = -first_y + prev_segment_first_y + 12.0;
			return(false);
		} else {
			possible_dy = asked_dy;
			return(true);
		}
	} else {
		if (first_y + asked_dy >= prev_segment_first_y - 12.0) {
			possible_dy = prev_segment_first_y - 12.0 - first_y;
			return(false);
		} else {
			possible_dy = asked_dy;
			return(true);
		}
	}
}

/**
	Permet de savoir s'il est possible de deplacer le second point du segment
	sans creer d'incoherence. La valeur du mouvement maximum qu'il est possible de faire
	sans incoherence est stockee dans le second parametre.
	@param asked_dy La valeur du mouvement demande
	@param possible_dy La valeur du mouvement possible (au maximum)
	@return true si le mouvement est possible ; false s'il doit etre limite
*/
bool ConducerSegment::canMove2ndPointY(const qreal &asked_dy, qreal &possible_dy) const {
	
	Q_ASSERT_X(isHorizontal(), "ConducerSegment::canMove2ndPointY", "segment non horizontal");
	
	/// On ne modifie jamais l'abscisse du second point d'un segment statique.
	if (isStatic()) {
		possible_dy = 0.0;
		return(false);
	}
	// a ce stade, on a forcement un segment suivant
	
	/// Si le segment suivant n'est pas statique, le mouvement est possible.
	if (next_segment -> hasNextSegment()) {
		possible_dy = asked_dy;
		return(true);
	}
	// a ce stade, le segment suivant est forcement statique
	
	/// Si le segment suivant est horizontal, le mouvement est possible :
	/// il induira la creation d'un segment vertical supplementaire.
	if (next_segment -> isHorizontal()) {
		possible_dy = asked_dy;
		return(true);
	}
	// a ce stade, le segment suivant est forcement vertical
	
	// recupere quelques donnees
	qreal next_segment_second_y = next_segment -> point2.y();
	qreal second_y              = point2.y();
	
	/// Il se peut que le mouvement doive etre limite de facon a ce
	/// que le segment statique conserve une taille minimale.
	if (next_segment -> length() < 0.0) {
		if (second_y + asked_dy < next_segment_second_y + 12.0) {
			possible_dy = -second_y + next_segment_second_y + 12.0;
			return(false);
		} else {
			possible_dy = asked_dy;
			return(true);
		}
	} else {
		if (second_y + asked_dy >= next_segment_second_y - 12.0) {
			possible_dy = next_segment_second_y - 12.0 - second_y;
			return(false);
		} else {
			possible_dy = asked_dy;
			return(true);
		}
	}
}

/**
	Gere les mouvements sur l'axe horizontal
	@param dx taille du deplacement en pixels
*/
void ConducerSegment::moveX(const qreal &dx) {
	if (isHorizontal()) return;
	Q_ASSERT_X(isVertical(), "ConducerSegment::moveX", "segment non vertical");
	
	bool has_prev_segment = hasPreviousSegment();
	bool has_next_segment = hasNextSegment();
	
	if (isStatic()) return;
	
	// determine si le mouvement demande doit etre limite et, le cas echeant, a quelle valeur il faut le limiter
	qreal real_dx_for_1st_point = 0.0;
	qreal real_dx_for_2nd_point = 0.0;
	canMove1stPointX(dx, real_dx_for_1st_point);
	canMove2ndPointX(dx, real_dx_for_2nd_point);
	
	qreal final_movement = (dx <= 0.0) ? qMax(real_dx_for_1st_point, real_dx_for_2nd_point) : qMin(real_dx_for_1st_point, real_dx_for_2nd_point);
	
	// applique le mouvement au premier point
	if (has_prev_segment) {
		point1.rx() += final_movement;
		if (previous_segment -> isFirstSegment()) {
			new ConducerSegment(
				previous_segment -> point2,
				point1,
				previous_segment,
				this
			);
		} else previous_segment -> setSecondPoint(point1);
	}
	
	// applique le mouvement au second point
	if (has_next_segment) {
		point2.rx() += final_movement;
		if (next_segment -> isLastSegment()) {
			new ConducerSegment(
				point2,
				next_segment -> point1,
				this,
				next_segment
			);
		} else next_segment -> setFirstPoint(point2);
	}
}

/**
	Gere les mouvements sur l'axe vertical
	@param dx taille du deplacement en pixels
*/
void ConducerSegment::moveY(const qreal &dy) {
	if (isVertical()) return;
	Q_ASSERT_X(isHorizontal(), "ConducerSegment::moveY", "segment non horizontal");
	
	bool has_prev_segment = hasPreviousSegment();
	bool has_next_segment = hasNextSegment();
	
	if (isStatic()) return;
	
	// determine si le mouvement demande doit etre limite et, le cas echeant, a quelle valeur il faut le limiter
	qreal real_dy_for_1st_point = 0.0;
	qreal real_dy_for_2nd_point = 0.0;
	canMove1stPointY(dy, real_dy_for_1st_point);
	canMove2ndPointY(dy, real_dy_for_2nd_point);
	
	qreal final_movement = (dy <= 0.0) ? qMax(real_dy_for_1st_point, real_dy_for_2nd_point) : qMin(real_dy_for_1st_point, real_dy_for_2nd_point);
	
	// applique le mouvement au premier point
	if (has_prev_segment) {
		point1.ry() += final_movement;
		if (previous_segment -> isFirstSegment()) {
			new ConducerSegment(
				previous_segment -> point2,
				point1,
				previous_segment,
				this
			);
		} else previous_segment -> setSecondPoint(point1);
	}
	
	// applique le mouvement au second point
	if (has_next_segment) {
		point2.ry() += final_movement;
		if (next_segment -> isLastSegment()) {
			new ConducerSegment(
				point2,
				next_segment -> point1,
				this,
				next_segment
			);
		} else next_segment -> setFirstPoint(point2);
	}
}

/**
	Change le segment precedent
	@param ps Le nouveau segment precedent
*/
void ConducerSegment::setPreviousSegment(ConducerSegment *ps) {
	previous_segment = ps;
	if (hasPreviousSegment()) {
		if (previousSegment() -> nextSegment() != this) previousSegment() -> setNextSegment(this);
	}
}

/**
	Change le segment suivant
	@param ps Le nouveau segment suivant
*/
void ConducerSegment::setNextSegment(ConducerSegment *ns) {
	next_segment = ns;
	if (hasNextSegment()) {
		if (nextSegment() -> previousSegment() != this) nextSegment() -> setPreviousSegment(this);
	}
}

/// @return true si ce segment est un segment statique, cad un segment relie a une borne
bool ConducerSegment::isStatic() const {
	return(isFirstSegment() || isLastSegment());
}

/// @return true si ce segment est le premier du conducteur
bool ConducerSegment::isFirstSegment() const {
	return(!hasPreviousSegment());
}

/// @return true si ce segment est le dernier du conducteur
bool ConducerSegment::isLastSegment() const {
	return(!hasNextSegment());
}

/**
	@return Le segment precedent
*/
ConducerSegment *ConducerSegment::previousSegment()  const {
	return(previous_segment);
}

/**
	@return Le segment suivant
*/
ConducerSegment *ConducerSegment::nextSegment()  const {
	return(next_segment);
}

/**
	@return true si le segment est vertical, false sinon
*/
bool ConducerSegment::isVertical() const {
	return(point1.x() == point2.x());
}

/**
	@return true si le segment est horizontal, false sinon
*/
bool ConducerSegment::isHorizontal() const {
	return(point1.y() == point2.y());
}

/**
	@return le premier point du segment
*/
QPointF ConducerSegment::firstPoint() const {
	return(point1);
}

/**
	@return le second point du segment
*/
QPointF ConducerSegment::secondPoint() const {
	return(point2);
}

/**
	Permet de changer la position du premier point du segment
	@param p La nouvelle position du premier point
*/
void ConducerSegment::setFirstPoint(const QPointF &p) {
	point1 = p;
}

/**
	Permet de changer la position du second point du segment
	@param p La nouvelle position du second point
*/
void ConducerSegment::setSecondPoint(const QPointF &p) {
	point2 = p;
}

/**
	@return true si le segment a un segment precedent, false sinon
*/
bool ConducerSegment::hasPreviousSegment() const {
	return(previous_segment != NULL);
}

/**
	@return true si le segment a un segment suivant, false sinon
*/
bool ConducerSegment::hasNextSegment() const {
	return(next_segment != NULL);
}

/**
	@return Le centre du rectangle delimitant le conducteur
*/
QPointF ConducerSegment::middle() const {
	return(
		QPointF(
			(point1.x() + point2.x()) / 2.0,
			(point1.y() + point2.y()) / 2.0
		)
	);
}

/**
	@return La longueur du conducteur
*/
qreal ConducerSegment::length() const {
	if (isHorizontal()) {
		return(secondPoint().x() - firstPoint().x());
	} else {
		return(secondPoint().y() - firstPoint().y());
	}
}
