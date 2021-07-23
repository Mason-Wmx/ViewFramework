
#include "stdafx.h"
#include "AppStyle.h"
#include "Application.h"

using namespace SIM;

QString AppStyle::GetStyleSheet( void )
{
	// Ref: http://contingencycoder.wordpress.com/2013/07/04/quick-tip-customize-qt-tooltips-with-style-sheets/
	// Ref: http://developer.nokia.com/community/wiki/Archived:Creating_a_gradient_background_for_a_QPushButton_with_style_sheet

	const std::string filename = "Light.qss";
	std::string filepath = Application::GetFileInStandardLocation(filename, "styles");
	QString path = QString::fromStdString(filepath);

	QFile file(path);
	file.open(QFile::ReadOnly);

	QString result = QLatin1String(file.readAll());
	file.close();

	return result;
}

std::vector<QString> AppStyle::GetFontPaths( void )
{
	std::vector<QString> paths;

	// Known issue:
	// Bold fonts and italic fonts have the same font name as the regular ones.
	// So they cannot be added to QFontDatabase if the regular fonts are added. No matter the adding order.

	std::vector<std::string> fonts;
	// from the finest to the thickest
	fonts.push_back("Artifakt Element Hair.ttf");
	fonts.push_back("Artifakt Element Hair Italic.ttf");
	fonts.push_back("Artifakt Element Thin.ttf");
	fonts.push_back("Artifakt Element Thin Italic.ttf");
	fonts.push_back("Artifakt Element Light.ttf");
	fonts.push_back("Artifakt Element Light Italic.ttf");
	fonts.push_back("Artifakt Element Regular.ttf");
	fonts.push_back("Artifakt Element Italic.ttf");
	fonts.push_back("Artifakt Element Book.ttf");
	fonts.push_back("Artifakt Element Book Italic.ttf");
	fonts.push_back("Artifakt Element Medium.ttf");
	fonts.push_back("Artifakt Element Medium Italic.ttf");
	fonts.push_back("Artifakt Element Bold.ttf");
	fonts.push_back("Artifakt Element Bold Italic.ttf");
	fonts.push_back("Artifakt Element Black.ttf");
	fonts.push_back("Artifakt Element Black Italic.ttf");
	fonts.push_back("Artifakt Element Heavy.ttf");
	fonts.push_back("Artifakt Element Heavy Italic.ttf");
	for each (auto font in fonts)
	{
		std::string filepath = Application::GetFileInStandardLocation(font, "fonts\\Artifakt\\Element WEB\\TTF");
		QString path = QString::fromStdString(filepath);
		paths.push_back(path);
	}
	
	fonts.clear();
	fonts.push_back("Artifakt Legend Hair.ttf");
	fonts.push_back("Artifakt Legend Hair Italic.ttf");
	fonts.push_back("Artifakt Legend Thin.ttf");
	fonts.push_back("Artifakt Legend Thin Italic.ttf");
	fonts.push_back("Artifakt Legend Light.ttf");
	fonts.push_back("Artifakt Legend Light Italic.ttf");
	fonts.push_back("Artifakt Legend Regular.ttf");
	fonts.push_back("Artifakt Legend Italic.ttf");
	fonts.push_back("Artifakt Legend Book.ttf");
	fonts.push_back("Artifakt Legend Book Italic.ttf");
	fonts.push_back("Artifakt Legend Medium.ttf");
	fonts.push_back("Artifakt Legend Medium Italic.ttf");
	fonts.push_back("Artifakt Legend Bold.ttf");
	fonts.push_back("Artifakt Legend Bold Italic.ttf");
	fonts.push_back("Artifakt Legend Black.ttf");
	fonts.push_back("Artifakt Legend Black Italic.ttf");
	fonts.push_back("Artifakt Legend Heavy.ttf");
	fonts.push_back("Artifakt Legend Heavy Italic.ttf");
	for each (auto font in fonts)
	{
		std::string filepath = Application::GetFileInStandardLocation(font, "fonts\\Artifakt\\Legend WEB\\TTF");
		QString path = QString::fromStdString(filepath);
		paths.push_back(path);
	}

	return paths;
} 