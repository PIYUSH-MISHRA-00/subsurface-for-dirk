// SPDX-License-Identifier: GPL-2.0
#include "core/subsurface-string.h"
#include "qPrefDisplay.h"
#include "qPrefPrivate.h"

#include <QApplication>
#include <QFont>

static const QString group = QStringLiteral("Display");

QPointF qPrefDisplay::st_tooltip_position;
static const QPointF st_tooltip_position_default = QPointF(0,0);

QString qPrefDisplay::st_lastDir;
static const QString st_lastDir_default = "";

QString qPrefDisplay::st_theme;
static const QString st_theme_default = "Blue";

QString qPrefDisplay::st_UserSurvey;
static const QString st_UserSurvey_default = "";

QByteArray qPrefDisplay::st_mainSplitter;
static const QByteArray st_mainSplitter_default = "";

QByteArray qPrefDisplay::st_topSplitter;
static const QByteArray st_topSplitter_default = "";

QByteArray qPrefDisplay::st_bottomSplitter;
static const QByteArray st_bottomSplitter_default = "";

bool qPrefDisplay::st_maximized;
static bool st_maximized_default = false; 

QByteArray qPrefDisplay::st_geometry;
static const QByteArray st_geometry_default = 0;

QByteArray qPrefDisplay::st_windowState;
static const QByteArray st_windowState_default = 0;

int qPrefDisplay::st_lastState;
static int st_lastState_default = false; 

qPrefDisplay::qPrefDisplay(QObject *parent) : QObject(parent)
{
}
qPrefDisplay *qPrefDisplay::instance()
{
	static qPrefDisplay *self = new qPrefDisplay;
	return self;
}

void qPrefDisplay::loadSync(bool doSync)
{
	disk_animation_speed(doSync);
	disk_divelist_font(doSync);
	disk_font_size(doSync);
	disk_display_invalid_dives(doSync);
	disk_show_developer(doSync);
	if (!doSync) {
		load_tooltip_position();
		load_theme();
		load_UserSurvey();
		load_mainSplitter();
		load_topSplitter();
		load_bottomSplitter();
		load_maximized();
		load_geometry();
		load_windowState();
		load_lastState();

	}
}

void qPrefDisplay::set_divelist_font(const QString &value)
{
	QString newValue = value;
	if (value.contains(","))
		newValue = value.left(value.indexOf(","));

	if (newValue != prefs.divelist_font &&
	    !subsurface_ignore_font(qPrintable(newValue))) {
		qPrefPrivate::copy_txt(&prefs.divelist_font, value);
		disk_divelist_font(true);

		qApp->setFont(QFont(newValue));
		emit instance()->divelist_font_changed(value);
	}
}
void qPrefDisplay::disk_divelist_font(bool doSync)
{
	if (doSync)
		qPrefPrivate::propSetValue(group + "/divelist_font", prefs.divelist_font, default_prefs.divelist_font);
	else
		setCorrectFont();
}

void qPrefDisplay::set_font_size(double value)
{
	if (value != prefs.font_size) {
		prefs.font_size = value;
		disk_font_size(true);

		QFont defaultFont = qApp->font();
		defaultFont.setPointSizeF(prefs.font_size);
		qApp->setFont(defaultFont);
		emit instance()->font_size_changed(value);
	}
}
void qPrefDisplay::disk_font_size(bool doSync)
{
	if (doSync)
		qPrefPrivate::propSetValue(group + "/font_size", prefs.font_size, default_prefs.font_size);
	else
		setCorrectFont();
}

//JAN static const QString group = QStringLiteral("Animations");
HANDLE_PREFERENCE_INT(Display, "/animation_speed", animation_speed);

HANDLE_PREFERENCE_BOOL(Display, "/displayinvalid", display_invalid_dives);

HANDLE_PREFERENCE_BOOL(Display, "/show_developer", show_developer);

void qPrefDisplay::setCorrectFont()
{
	// get the font from the settings or our defaults
	// respect the system default font size if none is explicitly set
	QFont defaultFont = qPrefPrivate::propValue(group + "/divelist_font", prefs.divelist_font).value<QFont>();
	if (IS_FP_SAME(system_divelist_default_font_size, -1.0)) {
		prefs.font_size = qApp->font().pointSizeF();
		system_divelist_default_font_size = prefs.font_size; // this way we don't save it on exit
	}

	prefs.font_size = qPrefPrivate::propValue(group + "/font_size", prefs.font_size).toFloat();
	// painful effort to ignore previous default fonts on Windows - ridiculous
	QString fontName = defaultFont.toString();
	if (fontName.contains(","))
		fontName = fontName.left(fontName.indexOf(","));
	if (subsurface_ignore_font(qPrintable(fontName))) {
		defaultFont = QFont(prefs.divelist_font);
	} else {
		free((void *)prefs.divelist_font);
		prefs.divelist_font = copy_qstring(fontName);
	}
	defaultFont.setPointSizeF(prefs.font_size);
	qApp->setFont(defaultFont);

	prefs.display_invalid_dives = qPrefPrivate::propValue(group + "/displayinvalid", default_prefs.display_invalid_dives).toBool();
}

HANDLE_PROP_QSTRING(Display, "FileDialog/LastDir", lastDir);

HANDLE_PROP_QSTRING(Display, "Theme/currentTheme", theme);

HANDLE_PROP_QPOINTF(Display, "ProfileMap/tooltip_position", tooltip_position);

HANDLE_PROP_QSTRING(Display, "UserSurvey/SurveyDone", UserSurvey);

HANDLE_PROP_QBYTEARRAY(Display, "MainWindow/mainSplitter", mainSplitter);

HANDLE_PROP_QBYTEARRAY(Display, "MainWindow/topSplitter", topSplitter);

HANDLE_PROP_QBYTEARRAY(Display, "MainWindow/bottomSplitter", bottomSplitter);

HANDLE_PROP_BOOL(Display, "MainWindow/maximized", maximized);

HANDLE_PROP_QBYTEARRAY(Display, "MainWindow/geometry", geometry);

HANDLE_PROP_QBYTEARRAY(Display, "MainWindow/windowState", windowState);

HANDLE_PROP_INT(Display, "MainWindow/lastState", lastState);
