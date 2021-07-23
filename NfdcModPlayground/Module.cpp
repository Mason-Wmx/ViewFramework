#include "stdafx.h"
#include "Module.h"

#include "DummyViewFactory.h"
#include "DummyTreeFactory.h"
#include "DummyCommand.h"
#include "DummyObject.h"
#include "RibbonExampleCommand.h"
#include "WebEngineViewCommands.h"
#include "../NfdcAppCore/mainwindow.h"
#include "../NfdcAppCore/BrowserTreeCommand.h"
#include "../NfdcAppCore/InfoCenterItem.h"
#include "../NfdcAppCore/InfoCenterMenuItem.h"
#include "../NfdcAppCore/ViewEvents.h"
using namespace SIM;
using namespace SIM::AppModules;


typedef NfdcModPlayground ModuleImplementation;
#include <headers/AppModules/AppModuleImp.h>


void NfdcModPlayground::RegisterViewFactories(IViewFactoryRegistry & factoryRegistry)
{
    auto & view3d = factoryRegistry.GetView3D();

    factoryRegistry.RegisterViewFactory(std::make_shared<DummyViewFactory>(view3d));
	factoryRegistry.RegisterViewFactory(std::make_shared<DummyGlyphViewFactory>(view3d));
	factoryRegistry.RegisterViewFactory(std::make_shared<DummyGroupViewFactory>(view3d));
}

void NfdcModPlayground::RegisterCommands(ICommandRegistry & cmdRegistry)
{
    try
    {
        auto & app = dynamic_cast<Application&>(GetApplication());
        if (app.IsDevMode())
        {
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandSurfaceMode>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandSelectionFilter>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandVisibilityFilter>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandCreateOnSurface>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandFreezeSelection>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandBlockEmptySelection>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandCreateOnSurfaceTop>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandGlyph>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandCreateGroupOnSurface>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandGenerateRandomEdges>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandEdgeMode>(app));
			cmdRegistry.RegisterCommand(std::make_shared<DummyKeepInCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandGenerateRandomNodes>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandNodeMode>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandBaseDialgStyle>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandControlsUnits>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCommandMDSMaterial>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCheckboxCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyCheckboxSecondCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyToggle01Command>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyToggle02Command>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyToggle03Command>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyToggle04Command>(app));
            cmdRegistry.RegisterCommand(std::make_shared<WebEngineViewCommand>(app));
			cmdRegistry.RegisterCommand(std::make_shared<GenerateFieldNormalsCommand>(app));
			cmdRegistry.RegisterCommand(std::make_shared<GeneratePointNormalsCommand>(app));
			cmdRegistry.RegisterCommand(std::make_shared<GenerateNodeNormalsCommand>(app));
			cmdRegistry.RegisterCommand(std::make_shared<GenerateEdgeNormalsCommand>(app));
			cmdRegistry.RegisterCommand(std::make_shared<NormalsCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<NormalsCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DebugMode6SocketsCommand>(app));
            cmdRegistry.RegisterCommand(std::make_shared<DummyProgressCommand>(app));  //DC-1960

            cmdRegistry.RegisterEditCommand(typeid(DummyObject).name(), std::make_shared<DummyCommandEdit>(app));


			cmdRegistry.RegisterCommand(std::make_shared<DummyCommandInfoCenter>(app));

        }
    }
    catch(const std::bad_cast& e)
    {
    }
}

void NfdcModPlayground::RegisterRibbonUI(IRibbon & ribbon)
{
    auto & app = GetApplication();
    if (app.IsDevMode())
    {   
        std::string bar = "PLAYGROUND";
        ribbon.SetBar(40, bar, "Playground");

        int pos = 0;

        std::string grp = "DUMMY";
        pos += 10;
        if (ribbon.SetGroup(bar, pos, grp, "Dummy") == 0)
        {
            ribbon.AddCommand(bar, grp, DummyCommand::Name);
            ribbon.AddCommand(bar, grp, DummyCommandFreezeSelection::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandBlockEmptySelection::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandSurfaceMode::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandSelectionFilter::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandVisibilityFilter::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandCreateOnSurface::Name, false);
			ribbon.AddCommand(bar, grp, DummyCommandCreateOnSurfaceTop::Name, false);
			ribbon.AddCommand(bar, grp, DummyCommandGlyph::Name, false);
			ribbon.AddCommand(bar, grp, DummyCommandCreateGroupOnSurface::Name, false);
			ribbon.AddCommand(bar, grp, DummyKeepInCommand::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandGenerateRandomNodes::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandBaseDialgStyle::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandControlsUnits::Name, false);
            ribbon.AddCommand(bar, grp, DummyCommandMDSMaterial::Name, false);
        }

		grp = "NORMALS";
		pos += 10;
		if (ribbon.SetGroup(bar, pos, grp, "Normals") == 0)
		{
			ribbon.AddLayout(bar, grp, IRibbon::LayoutDropDownButtonNonSwitchable, "dropDownNormals");
			ribbon.AddCommand(bar, grp, NormalsCommand::Name);
			ribbon.AddCommand(bar, grp, GeneratePointNormalsCommand::Name);
			ribbon.AddCommand(bar, grp, GenerateFieldNormalsCommand::Name);
			ribbon.AddCommand(bar, grp, GenerateNodeNormalsCommand::Name);
			ribbon.AddCommand(bar, grp, GenerateEdgeNormalsCommand::Name);
		}

        grp = "WEBVIEW";
        pos += 10;
        if (ribbon.SetGroup(bar, pos, grp, "Web view") == 0)
        {
            ribbon.AddCommand(bar, grp, WebEngineViewCommand::Name);
        }

		grp = "BROWSER";
		pos += 10;
		if (ribbon.SetGroup(bar, pos, grp, "Browser") == 0)
		{
			ribbon.AddCommand(bar, grp, ToggleBrowserTreeCommand::Name);
		}

        grp = "CHECKBOX";
        pos += 10;
        if (ribbon.SetGroup(bar, pos, grp, "CheckBox") == 0)
        {
            ribbon.AddLayout(bar, grp, IRibbon::LayoutVertical);
            ribbon.AddCheckbox(bar, grp, DebugMode6SocketsCommand::Name, false);
        }

        /*grp = "CHECKBOXES";
        pos += 10;
        if (ribbon.SetGroup(bar, pos, grp, "CheckBoxes") == 0)
        {
            ribbon.AddLayout(bar, grp, IRibbon::LayoutVertical);
            ribbon.AddCheckbox(bar, grp, DummyCheckboxCommand::Name, false);
            ribbon.AddCheckbox(bar, grp, DummyCheckboxSecondCommand::Name, false);
        }

        grp = "TOGGLE";
        pos += 10;
        if (ribbon.SetGroup(bar, pos, grp, "Toggle Group") == 0)
        {
            ribbon.AddLayout(bar, grp, IRibbon::LayoutToggleGrid);
            ribbon.AddToggle(bar, grp, DummyToggle01Command::Name, 0, 0);
            ribbon.AddToggle(bar, grp, DummyToggle02Command::Name, 0, 1);
            ribbon.AddToggle(bar, grp, DummyToggle03Command::Name, 1, 0);
            ribbon.AddToggle(bar, grp, DummyToggle04Command::Name, 1, 1);
        }
        grp = "PARAMETERS";
        pos += 10;

		grp = "OPTIMIZATION";
		pos += 10;*/

    grp = "SELECTION";
    pos += 10;
    if (ribbon.SetGroup(bar, pos, grp, "Selection") == 0)
    {
      ribbon.AddLayout(bar, grp, IRibbon::LayoutDropDownButton, "dropDownSelectionMode");
      ribbon.AddCommand(bar, grp, GeneralSelectionModeCommand::Name, false, true);
      ribbon.AddCommand(bar, grp, BodySelectionModeCommand::Name);
      ribbon.AddCommand(bar, grp, SurfaceSelectionModeCommand::Name);
      ribbon.AddCommand(bar, grp, EdgeSelectionModeCommand::Name);
      ribbon.AddCommand(bar, grp, NodeSelectionModeCommand::Name);
    }
	}
}

void NfdcModPlayground::RegisterTreeFactories(ITreeFactoryRegistry & factoryRegistry)
{
	try
	{
		auto & app = dynamic_cast<Application&>(GetApplication());
		if(app.IsDevMode())
		{
			auto & tree = factoryRegistry.GetBrowserTree();
			factoryRegistry.RegisterTreeFactory(std::make_shared<DummyTreeFactory>(tree));
		}
	}
	catch (const std::bad_cast& e)
	{
	}
}

class ProgressInfoCenterItem : public InfoCenterItem
{
public:
    ProgressInfoCenterItem(Application& app, std::string name, int index) :App(app), InfoCenterItem(index, name )
    {

    }
    virtual QWidget* GetWidget()
    {
        if (!_widget)
        {
            auto action = new QAction( &App.GetMainWindow() );
            _widget = QtExtHelpers::button(action, QSize(16, 16), Qt::ToolButtonTextOnly);


            _widget->setObjectName("BtnProgress");

            QObject::connect(action, &QAction::triggered, [this]()
            {
                _widget->setStyleSheet("background: transparent;"
                                       "image: url(:/NfdcModPlayground/images/progressPressed_ico.png);"
                                       "border-style: none");
                
                 App.GetController().RunCommand(DummyProgressCommand::Name);
            });

        }
        return _widget;
    }
    virtual void Notify(Event& ev)
    {

    }

    Application& App;
};




class DummyInfoCenterItem : public InfoCenterItem 
{
public:
    DummyInfoCenterItem(Application& app, int index) :App(app), Counter(0), InfoCenterItem(index)
    {

    }
    virtual QWidget* GetWidget() 
    { 
        if (!_widget)
        {
            auto action = new QAction(QObject::tr("IC2"), &App.GetMainWindow());
            _widget = QtExtHelpers::button(action, QSize(36, 16), Qt::ToolButtonTextOnly);
            _widget->setStyleSheet("min-width : 80px");
            QObject::connect(action, &QAction::triggered, [this]()
            {
                QMessageBox::information(&App.GetMainWindow(), "Info center 2", QString("This is Info Center Button. Number of events: %1").arg(Counter), QMessageBox::Ok);
            });
        }
        return _widget; 
    }
    virtual void Notify(Event& ev)
    {
        Counter++;
    }

    Application& App;
    int Counter;
};

void SIM::AppModules::NfdcModPlayground::RegisterInfoCenterItems(IInfoCenter & infoCenter)
{
	auto app = dynamic_cast<Application*>(&GetApplication());
	if (app && app->IsDevMode())
	{
		//Info center command
        infoCenter.AddItem(std::make_shared<InfoCenterItem>(DummyCommandInfoCenter::Name, 5555));

		//Info center button
		auto action = new QAction(QObject::tr("IC"), &app->GetMainWindow());
		auto button = QtExtHelpers::button(action, QSize(36, 16), Qt::ToolButtonTextOnly);
		button->setStyleSheet("min-width : 80px");
		QObject::connect(action, &QAction::triggered, [this,app]()
		{
			QMessageBox::information(&app->GetMainWindow(), "Info center", "This is Info Center Button", QMessageBox::Ok);
		});

        infoCenter.AddItem(std::make_shared<InfoCenterItem>(button, 5554));

        //Custom class
        infoCenter.AddItem(std::make_shared<DummyInfoCenterItem>(*app, 5556));

        /*
        //progress
		auto actionProgress = new QAction( &app->GetMainWindow() );
        auto buttonProgress = QtExtHelpers::button(actionProgress, QSize(16, 16), Qt::ToolButtonIconOnly);
        //buttonProgress->setStyleSheet("min-width : 100px");
        buttonProgress->setObjectName("BtnProgress");

        //std::string icon = ":/NfdcModMaterials/images/download_ico.png";
        //buttonProgress->setIcon( QIcon(icon.c_str()) );

        QObject::connect(actionProgress, &QAction::triggered, [this, app]()
        {
            app->GetController().RunCommand(DummyProgressCommand::Name);
        });
        
        infoCenter.AddItem(std::make_shared<InfoCenterItem>(buttonProgress, 5551));
        */

        infoCenter.AddItem(std::make_shared<ProgressInfoCenterItem>(*app, DummyProgressCommand::Name, 0));


        //Info center menu item
        
        auto menuItem = std::make_shared<InfoCenterMenuItem>(*app, QIcon(":/NfdcAppCore/images/new-16x16.png"), true, 5558);

        auto menuAction = new QAction(QObject::tr("IC2"), menuItem->GetMenu());
        QObject::connect(menuAction, &QAction::triggered, [app]()
        {
            QMessageBox::information(&app->GetMainWindow(), "Info center Menu Item", QString("Menu Test"), QMessageBox::Ok);
        });
        menuItem->AddAction(menuAction);
        menuItem->AddSeparator();
        menuItem->AddStandardCommandAction(DummyCommandInfoCenter::Name, true, true);
        menuItem->AddStandardCommandAction(DummyCommandInfoCenter::Name, false, true);
        infoCenter.AddItem(menuItem);
	}
}
