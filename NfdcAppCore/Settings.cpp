//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Settings.h"

//-----------------------------------------------------------------------------

#include "vtkExtAmbientOcclusionPass.h"

//-----------------------------------------------------------------------------

#if defined(WIN32) || defined(WIN64)
#define DEFAULT_UI_STYLE "WindowsVista"
#endif

#if defined(__APPLE__)
#define DEFAULT_UI_STYLE "Fusion"
#endif

#if defined(LINUX)
#define DEFAULT_UI_STYLE "GTK+"
#endif

//-----------------------------------------------------------------------------

QString Settings::style()
{
	QSettings settings;
	return  settings.value("style", DEFAULT_UI_STYLE).toString();
}

void Settings::style(const QString& s)
{
	QSettings settings;
	settings.setValue("style", s);
}

//-----------------------------------------------------------------------------

QPoint Settings::position()
{
	QSettings settings;
	return settings.value("pos", QPoint(50, 50)).toPoint();
}

void Settings::position(const QPoint& p)
{
	QSettings settings;
	settings.setValue("pos", p);
}

//-----------------------------------------------------------------------------

QSize Settings::size()
{
	QSettings settings;
	return settings.value("size", QSize(1024, 768)).toSize();
}

void Settings::size(const QSize& s)
{
	QSettings settings;
	settings.setValue("size", s);
}

//-----------------------------------------------------------------------------

QString Settings::units()
{
	QSettings settings;
	return settings.value("units", "metric").toString();
}

void Settings::units(const QString& s)
{
	QSettings settings;
	settings.setValue("units", s);
}

//-----------------------------------------------------------------------------

QString Settings::theme()
{
	QSettings settings;
	return settings.value("theme", "Style-Default").toString();
}

void Settings::theme(const QString& s)
{
	QSettings settings;
	settings.setValue("theme", s);
}

//-----------------------------------------------------------------------------

bool Settings::highlight()
{
	QSettings settings;
	return settings.value("highlighting", true).toBool();
}

//-----------------------------------------------------------------------------

void Settings::highlight(bool b)
{
	QSettings settings;
	settings.setValue("highlighting", b);
}

//-----------------------------------------------------------------------------

QByteArray Settings::state()
{
	QSettings settings;
	return settings.value("uistate").toByteArray();
}

void Settings::state(QByteArray ba)
{
	QSettings settings;
	settings.setValue("uistate", ba);
}

//-----------------------------------------------------------------------------

QSize Settings::iconSize()
{
	QSettings settings;
	return settings.value("tb-icons", QSize(32, 32)).toSize();
}

void Settings::iconSize(const QSize& s)
{
	QSettings settings;
	settings.setValue("tb-icons", s);
}

//-----------------------------------------------------------------------------

Qt::ToolButtonStyle Settings::buttonStyle()
{
	QSettings settings;
	return Qt::ToolButtonStyle(settings.value("tb-style", (int)Qt::ToolButtonTextUnderIcon).toInt());
}

void Settings::buttonStyle(Qt::ToolButtonStyle s)
{
	QSettings settings;
	settings.setValue("tb-style", (int)s);
}

//-----------------------------------------------------------------------------

bool Settings::labels()
{
	QSettings settings;
	return settings.value("tb-labels", true).toBool();
}

void Settings::labels(bool b)
{
	QSettings settings;
	settings.setValue("tb-labels", b);
}

//-----------------------------------------------------------------------------

bool Settings::collapsed()
{
	QSettings settings;
	return settings.value("tb-collapsed", false).toBool();
}

void Settings::collapsed(bool b)
{
	QSettings settings;
	settings.setValue("tb-collapsed", b);
}

//-----------------------------------------------------------------------------

QStringList Settings::recentFiles()
{
	QSettings settings;
	return settings.value("recentFileList").toStringList();
}

void Settings::recentFiles(const QStringList& sl)
{
	QSettings settings;
	settings.setValue("recentFileList", sl);
}

//-----------------------------------------------------------------------------

bool Settings::ao()
{
	QSettings settings;
	return settings.value("ao", true).toBool();
}

void Settings::ao(bool b)
{
	QSettings settings;
	settings.setValue("ao", b);
}

//-----------------------------------------------------------------------------

int Settings::aoRings()
{
	QSettings settings;
	return settings.value("ao-rings", VTKEXTAMBIENTOCCLUSIONPASS_RINGS_DEFAULT).toInt();
}

void Settings::aoRings(int i)
{
	QSettings settings;
	settings.setValue("ao-rings", i);
}

//-----------------------------------------------------------------------------

int Settings::aoSamples()
{
	QSettings settings;
	return  settings.value("ao-samples", VTKEXTAMBIENTOCCLUSIONPASS_SAMPLES_DEFAULT).toInt();
}

void Settings::aoSamples(int i)
{
	QSettings settings;
	settings.setValue("ao-samples", i);
}

//-----------------------------------------------------------------------------

double Settings::aoNoise()
{
	QSettings settings;
	return settings.value("ao-noise", VTKEXTAMBIENTOCCLUSIONPASS_NOISE_DEFAULT).toDouble();
}

void Settings::aoNoise(double d)
{
	QSettings settings;
	settings.setValue("ao-noise", d);
}

//-----------------------------------------------------------------------------

double Settings::aoGradient()
{
	QSettings settings;
	return settings.value("ao-gradient", VTKEXTAMBIENTOCCLUSIONPASS_GRADIENT_DEFAULT).toDouble();
}

void Settings::aoGradient(double d)
{
	QSettings settings;
	settings.setValue("ao-gradient", d);
}

//-----------------------------------------------------------------------------

QString Settings::lastDirectory()
{
	auto documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) ;
	QSettings settings;
	return settings.value("last-directory",documents).toString();
}

void Settings::lastDirectory(const QString& s)
{
	QSettings settings;
	settings.setValue("last-directory", s);
}

//-----------------------------------------------------------------------------

