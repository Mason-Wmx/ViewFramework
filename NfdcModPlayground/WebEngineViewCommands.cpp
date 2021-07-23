
#include "stdafx.h"
#include "WebEngineViewCommands.h"
#include "../NfdcAppCore/Document.h"
#include "../NfdcAppCore/EmbeddedWebView.h"

using namespace SIM;

WebEngineViewCommand::WebEngineViewCommand(Application& app) : ContinuousCommand(app)
{
}

constexpr const char * Html =
R"html(
<!doctype html>
<html lang="en">
    <head>
        <meta charset="utf-8">
    </head>
    <body>
    </body>
</html>
)html";

constexpr const char * Script =
R"js(
    if (!window.NfdcContext) {
        window.NfdcContext = {};
    }
    window.NfdcContext.DisplayContext = function() {
        var element = document.createElement("P");
        element.innerText = new Date().toString()+ "\t\t\t" + JSON.stringify(window.NfdcContext);
        element.style.width = "100%";
        document.body.appendChild(element);
    };
    window.NfdcContext.DisplayContext();
    window.NfdcContext.ForgeAuthTokenRefreshed = function(authToken) {
        window.NfdcContext.DisplayContext();
    };
)js";

bool WebEngineViewCommand::Execute()
{
	OnStart();

    auto dlg = new QDialog();
	SetDialog(dlg);
    dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    dlg->setWindowTitle("WebView");

    auto layout = new QVBoxLayout();
    auto view = new EmbeddedWebView(GetDocument()->GetView(), dlg);
    GetDocument()->GetView().RegisterObserver(*view);
    layout->addWidget(view);
    dlg->setLayout(layout);

    QObject::connect(dlg, &QDialog::rejected, [this]() {
        this->OnEnd();
    });

	dlg->show();

    QObject::connect(view, &QWebEngineView::loadFinished, [view](bool loaded) {
        view->page()->runJavaScript(Script);
    });

    view->Initialize();
    view->setHtml(Html);

	return true;
}

std::string WebEngineViewCommand::GetUniqueName()
{
	return Name;
}

const std::string WebEngineViewCommand::Name = "WebEngineViewTest";