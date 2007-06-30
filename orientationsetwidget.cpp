#include "orientationsetwidget.h"

OrientationSetWidget::OrientationSetWidget(QWidget *parent) : QWidget(parent) {
	
	default_radios = new QButtonGroup(this);
	
	#define MK_COMBO_BOX(a) a##_orientation = new QComboBox();\
	a##_orientation -> addItem(QIcon(":/ico/allowed.png"), tr("Possible"), "y");\
	a##_orientation -> addItem(QIcon(":/ico/forbidden.png"),tr("Impossible"), "n");\
	connect(a##_orientation, SIGNAL(activated(int)), this, SLOT(updateOrientationSet()));\
	a##_default = new QRadioButton();
	
	// 4 combo box, 4 boutons radios
	MK_COMBO_BOX(north)
	MK_COMBO_BOX(east)
	MK_COMBO_BOX(south)
	MK_COMBO_BOX(west)
	
	#undef MK_COMBO_BOX
	
	default_radios -> addButton(north_default, QET::North);
	default_radios -> addButton(east_default,  QET::East);
	default_radios -> addButton(south_default, QET::South);
	default_radios -> addButton(west_default,  QET::West);
	connect(default_radios, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(slot_defaultChanged(QAbstractButton *)));
	
	// petites icones symbolisant les orientations
	QLabel *north_pixmap = new QLabel(tr("Nord :"));
	north_pixmap -> setPixmap(QPixmap(":/ico/north.png"));
	QLabel *east_pixmap = new QLabel(tr("Est :"));
	east_pixmap -> setPixmap(QPixmap(":/ico/east.png"));
	QLabel *south_pixmap = new QLabel(tr("Sud :"));
	south_pixmap -> setPixmap(QPixmap(":/ico/south.png"));
	QLabel *west_pixmap = new QLabel(tr("Ouest :"));
	west_pixmap -> setPixmap(QPixmap(":/ico/west.png"));
	
	QGridLayout *qgl = new QGridLayout(this);
	qgl -> addWidget(new QLabel(tr("Par d\351faut")),   0, 3);
	qgl -> addWidget(north_pixmap,                      1, 0);
	qgl -> addWidget(new QLabel(tr("Nord :")),          1, 1);
	qgl -> addWidget(north_orientation,                 1, 2);
	qgl -> addWidget(north_default,                     1, 3, Qt::AlignHCenter);
	qgl -> addWidget(east_pixmap,                       2, 0);
	qgl -> addWidget(new QLabel(tr("Est :")),           2, 1);
	qgl -> addWidget(east_orientation,                  2, 2);
	qgl -> addWidget(east_default,                      2, 3, Qt::AlignHCenter);
	qgl -> addWidget(south_pixmap,                      3, 0);
	qgl -> addWidget(new QLabel(tr("Sud :")),           3, 1);
	qgl -> addWidget(south_orientation,                 3, 2);
	qgl -> addWidget(south_default,                     3, 3, Qt::AlignHCenter);
	qgl -> addWidget(west_pixmap,                       4, 0);
	qgl -> addWidget(new QLabel(tr("Ouest :")),         4, 1);
	qgl -> addWidget(west_orientation,                  4, 2);
	qgl -> addWidget(west_default,                      4, 3, Qt::AlignHCenter);
	
	// les combobox s'etendent autant que possible
	qgl -> setColumnStretch(2, 1);
	
	// l'orientation par defaut par defaut est le Nord
	north_default -> setChecked(true);
	slot_defaultChanged(north_default);
}

OrientationSet OrientationSetWidget::orientationSet() const {
	return(ori);
}

void OrientationSetWidget::setOrientationSet(const OrientationSet &os) {
	ori = os;
	updateForm();
}

void OrientationSetWidget::slot_defaultChanged(QAbstractButton *button) {
	if      (button == north_default) north_orientation -> setCurrentIndex(0);
	else if (button == east_default)  east_orientation  -> setCurrentIndex(0);
	else if (button == south_default) south_orientation -> setCurrentIndex(0);
	else if (button == west_default)  west_orientation  -> setCurrentIndex(0);
	
	north_orientation -> setEnabled(button != north_default);
	east_orientation  -> setEnabled(button != east_default);
	south_orientation -> setEnabled(button != south_default);
	west_orientation  -> setEnabled(button != west_default);
	
	updateOrientationSet();
}

void OrientationSetWidget::updateOrientationSet() {
	ori.setNorth(!north_orientation -> currentIndex());
	ori.setEast (!east_orientation  -> currentIndex());
	ori.setSouth(!south_orientation -> currentIndex());
	ori.setWest (!west_orientation  -> currentIndex());
	ori.setDefaultOrientation(static_cast<QET::Orientation>(default_radios -> checkedId()));
}

void OrientationSetWidget::updateForm() {
	north_orientation -> setCurrentIndex(ori.north() ? 0 : 1);
	east_orientation  -> setCurrentIndex(ori.east()  ? 0 : 1);
	south_orientation -> setCurrentIndex(ori.south() ? 0 : 1);
	west_orientation  -> setCurrentIndex(ori.west()  ? 0 : 1);
	QRadioButton *default_button = NULL;
	switch(ori.defaultOrientation()) {
		case QET::North: default_button = north_default; break;
		case QET::East : default_button = east_default ; break;
		case QET::South: default_button = south_default; break;
		case QET::West : default_button = west_default ; break;
	}
	if (default_button != NULL) {
		default_button -> setChecked(true);
		slot_defaultChanged(default_button);
	}
}
