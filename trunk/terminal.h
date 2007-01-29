#ifndef BORNE_H
	#define BORNE_H
	#define TAILLE_BORNE 4
	#include <QtGui>
	#include <QtXml>
	class Conducer;
	class Element;
	class Diagram;
	/**
		Classe modelisant la � borne � d'un appareil, c'est-a-dire un
		branchement possible pour un Conducteur.
	*/
	class Terminal : public QGraphicsItem {
		public:
		// enum definissant l'orientation de la borne
		enum Orientation {Nord, Est, Sud, Ouest};
		
		// permet de caster un QGraphicsItem en Borne avec qgraphicsitem_cast
		enum { Type = UserType + 1002 };
    	virtual int type() const { return Type; }
		
		// constructeurs
		Terminal();
		Terminal(QPointF,      Terminal::Orientation, Element * = 0, Diagram * = 0);
		Terminal(qreal, qreal, Terminal::Orientation, Element * = 0, Diagram * = 0);
		
		// destructeur
		~Terminal();
		
		// implementation des methodes virtuelles pures de QGraphicsItem
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
		QRectF boundingRect() const;
		
		// methodes de manipulation des conducteurs lies a cette borne
		bool addConducer(Conducer *);
		void removeConducer(Conducer *);
		inline int nbConducers() const { return(liste_conducers.size()); }
		
		// methodes de lecture
		QList<Conducer *> conducers() const; 
		Terminal::Orientation orientation() const;
		inline QPointF amarrageConducer() const { return(mapToScene(amarrage_conducer)); }
		void updateConducer(QPointF = QPointF());
		
		// methodes relatives a l'import/export au format XML
		static bool valideXml(QDomElement  &);
		bool        fromXml  (QDomElement  &);
		QDomElement toXml    (QDomDocument &) const;
		
		protected:
		// methodes de gestion des evenements
		void hoverEnterEvent  (QGraphicsSceneHoverEvent *);
		void hoverMoveEvent   (QGraphicsSceneHoverEvent *);
		void hoverLeaveEvent  (QGraphicsSceneHoverEvent *);
		void mousePressEvent  (QGraphicsSceneMouseEvent *);
		void mouseMoveEvent   (QGraphicsSceneMouseEvent *);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
		
		private:
		// pointeur vers la QGraphicsScene de type Diagram (evite quelques casts en interne)
		Diagram *diagram_scene;
		// coordonnees des points d'amarrage
		QPointF amarrage_conducer;
		QPointF amarrage_elmt;
		// orientation de la borne
		Terminal::Orientation sens;
		// liste des conducers lies a cette borne
		QList<Conducer *> liste_conducers;
		// pointeur vers un rectangle correspondant au bounding rect ; permet de ne calculer le bounding rect qu'une seule fois ; le pointeur c'est parce que le compilo exige une methode const
		QRectF *br;
		Terminal *terminal_precedente;
		bool hovered;
		// methode initialisant les differents membres de la borne
		void initialise(QPointF, Terminal::Orientation);
		// differentes couleurs utilisables pour l'effet "hover"
		QColor couleur_hovered;
		QColor couleur_neutre;
		QColor couleur_autorise;
		QColor couleur_prudence;
		QColor couleur_interdit;
	};
#endif
