#include "stdafx.h"
#include "View3D.h"
#include "AppEvents.h"

#include "vtkQImageToImageSource.h"
// #include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty2D.h"
#include "vtkBoxWidget2.h"
#include "vtkAreaPicker.h"
#include "vtkBoxRepresentation.h"
#include "vtkCellPicker.h"
#include "vtkQuadricDecimation.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkLightsPass.h"
#include "vtkOpaquePass.h"
#include "vtkTranslucentPass.h"
#include "vtkVolumetricPass.h"
#include "vtkOverlayPass.h"
#include "vtkRenderPassCollection.h"
#include "vtkSequencePass.h"
#include "vtkLightKit.h"
#include "vtkBoundingBox.h"
#include "vtkTubeFilter.h"
#include "vtkGlyph3D.h"
#include "vtkRendererCollection.h"
#include <vtkLabelPlacementMapper.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkStringArray.h>
#include <vtkExtSkyMapPass.h>
#include "vtkExtractGeometry.h"
#include "vtkExtRenderWindowAutoCAM.h"
#include "vtkExtCameraPassAutoCAM.h"
#include "vtkExtMeshReader.h"
#include "vtkExtMeshWriter.h"
#include "vtkExtVBOPolyDataMapper.h"
#include "vtkExtVBODataSetMapper.h"
#include "vtkLabeledDataMapper.h"
#include "vtkExtractGeometry.h"
#include "vtkExtractSelectedFrustum.h"
#include "vtkSelectionNode.h"
#include "vtkProp3DCollection.h"
#include "vtkUnstructuredGrid.h"
#include "ViewFactory.h"
#include "GeometryViewFactory.h"
#include "vtkPlanes.h"
#include "Factory.h"
#include "IAppConfig.h"
#include "DocModel.h"
#include "DocView.h"
#include "Document.h"
#include "ViewEvents.h"
#include "GeometryScene.h"
#include "vtkSphereSource.h"
#include "MainThreadDispatcher.h"
#include "DocCommand.h"
#include "WidgetHelper.h"
#include "ContextMenu.h"
#include <vtkExtractPolyDataGeometry.h>

#include "vtkAxesActor.h"
#include "vtkTextProperty.h"
#include "vtkCaptionActor2D.h"

#include "vtkWindowToImageFilter.h"
#include "vtkExtImageWriter.h"
#include "vtkImageFlip.h"
#include "vtkCellTreeLocator.h"

#if defined(OPENVR_ENABLED)
#include "vtkOpenVRRenderWindow.h"
#include "vtkOpenVRRenderer.h"
#include "vtkOpenVRRenderPasses.h"
#include "vtkOpenVRRenderWindowInteractor.h"
#include "vtkRenderingOpenVRObjectFactory.h"
#include "vtkOverrideInformationCollection.h"
#include "vtkOpenVRCamera.h"
#include "vtkOpenGLPolyDataMapper.h"
#endif // defined(OPENVR_ENABLED)

#include <thread>
namespace STB {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
} // name

using namespace SIM;

class vtkExtAutoCAMSettingsEx : public vtkExtAutoCAMSettings
{
public:
	vtkExtAutoCAMSettingsEx(const char* company, const char* application)
		: mSettings(company, application)
	{
	}

	virtual void setValue(const char* key, const char* value)
	{
		mSettings.setValue(key, value);
	}

	virtual void setValue(const char* key, int value)
	{
		mSettings.setValue(key, value);
	}

	virtual void setValue(const char* key, float value)
	{
		mSettings.setValue(key, value);
	}

	virtual const char* getValue(const char* key, const char* defaultValue)
	{
		mResult = mSettings.value(key, defaultValue).toString().toStdString();
		return mResult.c_str();
	}

	virtual int getValue(const char* key, int defaultValue)
	{
		return mSettings.value(key, defaultValue).toInt();
	}

	virtual float getValue(const char* key, float defaultValue)
	{
		return mSettings.value(key, defaultValue).toFloat();
	}

private:
	QSettings mSettings;
	std::string mResult;
};

class WaitCursor
{
public:
	WaitCursor()
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
	}
	~WaitCursor()
	{
		QApplication::restoreOverrideCursor();
	}
};

namespace GeometryStrings
{
	static const std::string model = "model";

	// Render Settings
	static const std::string opacity = "Render-Opacity";
	static const std::string diffuse = "Render-Diffuse";
	static const std::string ambient = "Render-Ambient";
	static const std::string specular = "Render-Specular";
	static const std::string power = "Render-Power";
	static const std::string type = "Render-Type";
	static const std::string edgeWidth = "Render-Edge-Width";
	static const std::string noise = "Render-Noise";
	static const std::string cubeMap = "Render-CubeMap";
	static const std::string colorR = "Render-Color-R";
	static const std::string colorG = "Render-Color-G";
	static const std::string colorB = "Render-Color-B";
	static const std::string edges = "Render-Edges";
	static const std::string edgeR = "Render-Edge-R";
	static const std::string edgeG = "Render-Edge-G";
	static const std::string edgeB = "Render-Edge-B";

}; // namespace GeometryStrings

class vtkPickCommand : public vtkCommand
{
public:
	static vtkPickCommand* New()
	{
		return new vtkPickCommand;
	}

	vtkTypeMacro(vtkPickCommand, vtkCommand);

	vtkPickCommand()
	{
	}

	~vtkPickCommand()
	{
	}

	virtual void Execute(vtkObject *caller, unsigned long eventId, void*)
	{
		if (Pick(eventId))
		{
			SetAbortFlag(true);
		}
	}

	std::function<bool(unsigned long eventId)> Pick;
};



//static 
bool SIM::View3D::InitOpenGL(bool showWarnings, QWidget *parent, bool initDummyWindow = false)
{
	bool wasInitSuccesful = false;

	if (initDummyWindow)
	{
		// To check if appropriate OpenGL version is available, a dummy window/widget is created
		//    and OpenGL initialization take place.
		// Initialization in View3D class is a little different, so it might happen that verification
		//    pass while creating real View3D will fail and otherwise.
		// 2017-01-23: It was decided to do not create dummy View3D object, because it require Application
		//    and Document objects. Latter ones create (or will create in future) and register a lot of things 
		//    and eg. in case of singletons there is place for potential problems.

		QGraphicsView view;

		QGLFormat format = QGLFormat::defaultFormat();
		QGLContext* GLcontext = new QGLContext(format);
		std::unique_ptr<QVTKWidget2> widget(new QVTKWidget2(GLcontext, &view));

		// We want the OpenGL context current before we create AutoCAM, as it will generate textures
		widget->makeCurrent();

		view.setViewport(widget.get());
		view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
		view.setScene(new GeometryScene(&view));

		wasInitSuccesful = SIM::View3D::InitOpenGL();
	}
	else
	{
		wasInitSuccesful = SIM::View3D::InitOpenGL();
	}

    auto config = SIM::Application::RegisterAppConfig();

	if (showWarnings && !wasInitSuccesful)
		QMessageBox::critical(parent, config->application(), tr("Your graphics card does not meet the minimal requirements needed to run this software.\n"
			"Updating the driver of yours graphics card may resolve the issue.\n"
			"Check the manufacturer's website for the latest software."), QMessageBox::Ok);

	return wasInitSuccesful;
}



//static
bool SIM::View3D::InitOpenGL()
{
	//init OpenGL
	if (::glewInit() != GLEW_OK)
		return false;

	{ // OpenGL version Verification: 
		//
		//  glTexBuffer() require min 3.1 OpenGL Version, so we have to check if we have it.
		//						For now I believe glTexBuffer() is the method that require the highest version
		//	2017-01-19 Andrzej: I believe we should not allow to create View3D object (operator new) if we don't have minimum requirements satisfied.
		//											This allow to program start up correctly, browse results etc. but not see model in 3D.
		//											Also we avoid throwing unnecessary exceptions in this program
		const unsigned char* glver = glGetString(GL_VERSION);
		glver; //currently not used, but preserve for debugging since glGetIntegerv(GL_MAJOR_VERSION, ...) doesn't return value for older versions.

		GLint majVers = 0, minVers = 0;// , profile = 0, flags = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majVers); //this method return 0 for OpenGL version 2.1.2, but return correct value for 4.5
		glGetIntegerv(GL_MINOR_VERSION, &minVers); //this method return 0 for OpenGL version 2.1.2, but return correct value for 4.5

																							 //glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
																							 //glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
																							 //if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
																							 //if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) 
																							 //if (flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)

		if ((majVers < 3) || ((majVers == 3) && (minVers < 1)))
			return false;
	}

	return true;
}


SIM::View3D::View3D(DocView& docView) :_View(docView)
#if defined(OPENVR_ENABLED)
, mOpenVREnabled(false)
, mOpenVRObjectFactory(nullptr)
#endif // defined(OPENVR_ENABLED)
{
	_zoomHeight = -1;
	_parScale = 0;
	_dblClick = false;
}

//static
std::unique_ptr<View3D> SIM::View3D::CreateView3D(DocView& docView)
{
	std::unique_ptr<View3D> view(new View3D(docView));
	
	if (view.get() == nullptr)
		return nullptr;

#if defined(OPENVR_ENABLED)
	view->EnableOpenVRObjectFactories(false);
#endif // defined(OPENVR_ENABLED)

	//vtkObject::SetGlobalWarningDisplay(false);

	view->setAttribute(Qt::WA_DeleteOnClose);

	auto config = Factory::Instancer<IAppConfig>();
	assert(config);

	view->setWindowIcon(QIcon(config->icon()));

	QGLFormat format = QGLFormat::defaultFormat();
	QGLContext* GLcontext = new QGLContext(format);
	view->_Widget = std::auto_ptr<QVTKWidget2>(new QVTKWidget2(GLcontext, view.get()));

	// We want the OpenGL context current before we create AutoCAM, as it will generate textures
	view->_Widget->makeCurrent();

	view->setViewport(view->_Widget.get());
	view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	view->setScene(new GeometryScene(view.get()));

	bool bOpenGLInitOK = InitOpenGL(true /*showWarnings*/, view.get(), false /*initDummyWindow*/);
	if (!bOpenGLInitOK)
		return nullptr;
	
	auto rendererAutoCam = vtkSmartPointer<vtkExtRendererAutoCAM>::New();	
	rendererAutoCam->SetLayer(1);
	rendererAutoCam->ResetCamera();

	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(1, 1, 1);
	renderer->SetBackground2(1, 1, 1);
	renderer->SetGradientBackground(true);
	renderer->SetLayer(0);
	renderer->SetActiveCamera(rendererAutoCam->GetActiveCamera());

	// Pass scaling to AutoCAM
	auto scale = view->_Widget->windowHandle()->devicePixelRatio();
	vtkExtRenderWindowAutoCAM::SetDPIScale(scale);

	// Create render window
	vtkSmartPointer<vtkExtRenderWindowAutoCAM> renderWindow = vtkSmartPointer<vtkExtRenderWindowAutoCAM>::New();
	renderWindow->SetNumberOfLayers(view->_renderersCount+1);
	view->_RenderWindow = renderWindow;
	auto autoCAM = renderWindow->GetAutoCAM();
	if (autoCAM)
	{
		float ttColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		autoCAM->setToolTipColor(ttColor);
		autoCAM->SetNavBarAnimation(false);
		autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_NAVBAR, false);
		autoCAM->perspective(false);
	}

	// Setup callbacks render, menu, cursor
	std::function< void() > renderCallBack = std::bind(&SIM::View3D::render, view.get());
	renderWindow->SetRenderCallBack(renderCallBack);

	std::function< int(int) > menuCallBack = std::bind(&SIM::View3D::popupMenu, view.get(), std::placeholders::_1);
	renderWindow->SetMenuCallBack(menuCallBack);

	std::function< void(bool) > cursorCallBack = std::bind(&SIM::View3D::showCursor, view.get(), std::placeholders::_1);
	renderWindow->SetCursorCallBack(cursorCallBack);

	std::function< void() > commandCallBack = std::bind(&SIM::View3D::commandCallBack, view.get());
	renderWindow->SetCommandCallBack(commandCallBack);

	std::function< bool() > ribbonCallBack = std::bind(&SIM::View3D::ribbonCallBack, view.get());
	renderWindow->SetRibbonCallBack(ribbonCallBack);

	// Set interator
	auto renderWindowInteractor = vtkSmartPointer<vtkExtRenderWindowInteractorAutoCAM>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	view->_RenderInteractorStyleAutoCAM = vtkExtSimInteractorStyleAutoCAM::New();
	view->_RenderInteractorStyleAutoCAM->SetTurntableRotation(docView.GetParent().GetApplication().GetModel().GetTurntableRotation());
	renderWindowInteractor->SetInteractorStyleAutoCAM(view->_RenderInteractorStyleAutoCAM);
	view->_RenderInteractorStyleAutoCAM->SetCurrentRenderer(rendererAutoCam);
	renderWindowInteractor->SetRenderDelegate(renderCallBack);
	view->_RenderWindowInteractor = renderWindowInteractor;
	// Add render window to widget
	view->_Widget->SetRenderWindow(renderWindow);
	view->_Widget->GetRenderWindow()->AddRenderer(renderer);
	for (int i = 1; i < view->_renderersCount; i++)
	{
		auto renderer2 = vtkSmartPointer<vtkRenderer>::New();
		renderer2->SetLayer(1);
		renderer2->SetActiveCamera(rendererAutoCam->GetActiveCamera());
		view->_Widget->GetRenderWindow()->AddRenderer(renderer2);
	}
	view->_Widget->GetRenderWindow()->AddRenderer(rendererAutoCam);
	view->_Widget->GetRenderWindow()->SetSwapBuffers(0);
	view->_Widget->setAutoBufferSwap(true);

	view->_InteractorAdapter.reset(new QVTKInteractorAdapter(view.get()));

	// Set pre-render callback to bind cube map, render skybox
	renderWindow->GetAutoCAM()->SetPreRenderCallBack([pV = view.get()]()
	{
		if (pV->_CubeMap)
		{
			pV->_CubeMap->Bind(GL_TEXTURE1);
		}
	});
	
	// Set post-render callback to enable/disable vtk widgets
	renderWindow->GetAutoCAM()->SetPostRenderCallBack([pV = view.get()]()
	{
	});

	// Selection picker
	auto selectionPicker = vtkSmartPointer<vtkPickCommand>::New();
	selectionPicker->Pick = [pV = view.get()](unsigned long eventId) -> bool {
			if (pV->_dblClick)
			{
				pV->_dblClick = false;
				return false;
			}
			if (pV->_RenderInteractorStyleAutoCAM->IsWindowSelectionMode())
			{
				pV->PickAreaCallbackFunction(EMode::Selection);
				return true;
			}
			else
			{
				return pV->PickObject(EMode::Selection) != NullObjectKey;
			}
	};

	view->_RenderInteractorStyleAutoCAM->AddObserver(vtkCommand::EndPickEvent, selectionPicker, 0);
	
	// Context menu picker
	auto contextMenuPicker = vtkSmartPointer<vtkPickCommand>::New();
	contextMenuPicker->Pick = [pV = view.get()](unsigned long eventId) -> bool {
		ContextMenu menu(pV);
		SIM::Application& app = pV->GetModel().GetParent().GetApplication();

		// clear picked object and get new one
		ObjectId id = pV->PickObject(EMode::Regular);

		// get current selection and check if something is selected or picked
		auto selected = pV->GetModel().GetSelectedObjects();
		if (selected.size() == 0 && id == NullObjectKey)
		{

			auto addActionToMenu = [&](const std::string& cmdName) {

				auto cmd = dynamic_cast<SelectionModeCommand*>(app.GetController().GetCommand(cmdName));
				if (!cmd)
					return;

                if (pV->GetModel().IsSelectionContextValid(cmd->GetSelectionContext()))
                {
                    auto pAction = new ContextMenuAction(cmd->GetLabel(), &menu);
                    pAction->setCheckable(true);
                    WidgetHelper::CreateActionForCommand(*cmd, false, true, &pV->_signalMapper, (QObject*)&app.GetMainWindow(), pAction);
                    if (pAction)
                    {
                        pAction->setChecked(cmd->GetSelectionContext() == pV->GetModel().GetSelectionContext());
                        menu.addAction(pAction);
                    }
                }
			};

			addActionToMenu(GeneralSelectionModeCommand::Name);
			addActionToMenu(SurfaceSelectionModeCommand::Name);
			addActionToMenu(EdgeSelectionModeCommand::Name);
			addActionToMenu(NodeSelectionModeCommand::Name);

			menu.exec(QCursor::pos());

			return true;
		}

		// if some object is picked, check if exists in current selection
		// if exists keep the selection, if not exists select picked object
		if (id != NullObjectKey)
		{
			bool exists = false;
			for (auto object : selected)
				if (object->GetId() == id)
				{
					exists = true;
					break;
				}
			if (!exists)
			{
				pV->GetModel().SetSelection(id);
				selected = pV->GetModel().GetSelectedObjects();
			}
		}

		// prepare commands and build menu
		
		std::vector<std::string> commands;
		std::map<SIM::ViewFactory*, bool> factories;

		for (auto obj : selected)
		{
			auto viewFactory = pV->GetFactory(*obj->GetMainObject());

			if (!viewFactory)
				continue;

			if (factories.find(viewFactory) != factories.end())
				continue;

			factories[viewFactory] = true;

			auto cmds = viewFactory->GetContextCommands(*obj.get());
			for (auto cmd : cmds)
			{
				std::vector<std::string>::iterator c = commands.begin();
				for (; c != commands.end(); c++)
					if (*c == cmd)
						break;
				if (c == commands.end())
					commands.push_back(cmd);
			}
		}

		if (commands.size() == 0)
			return false;

		for (auto cmdName : commands)
		{
			if (cmdName.length() && *cmdName.begin() == '-')
			{
				cmdName.erase(cmdName.begin());

				auto action = menu.addSeparator();
				action->setDisabled(true);
				action->setSeparator(true);
			}

			auto cmd = app.GetController().GetCommand(cmdName);
			if (!cmd)
				continue;

			auto pAction = WidgetHelper::CreateActionForCommand(*cmd, true, false, &pV->_signalMapper, (QObject*)&app.GetMainWindow(), new  ContextMenuAction(cmd->GetLabel(), &menu));

			menu.addAction(pAction);
		}


		menu.exec(QCursor::pos());

		return true;
	};

	QObject::connect(&view->_signalMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), [pV = view.get()](const QString& command)
	{
		std::string s = command.toStdString();
		pV->GetModel().GetParent().GetApplication().GetController().Notify(ExecuteCommandEvent(s, pV->GetModel().GetParent().GetApplication().GetView(), "View3D"));;
	});

	view->_RenderInteractorStyleAutoCAM->AddObserver(USER_EVENT_CONTEXT_MENU, contextMenuPicker, 0);

	// Preselection picker
	auto preselectionPicker = vtkSmartPointer<vtkPickCommand>::New();
	preselectionPicker->Pick = [pV = view.get()](unsigned long eventId) -> bool {
		if (pV->_RenderInteractorStyleAutoCAM->IsWindowSelectionMode())
		{
			pV->PickAreaCallbackFunction(EMode::Preselection);
			return true;
		}
		else
		{
			return pV->PickObject(EMode::Preselection) != NullObjectKey;
		}
		return false;
	};

	view->_RenderWindowInteractor->AddObserver(vtkCommand::MouseMoveEvent, preselectionPicker, 0);
	view->setupRenderPasses(renderWindow);
	view->createLighting();
	view->InitializeFactories();

  	return view;
}

void SIM::View3D::setupCSMarker() {
  _orientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  for (int i = 0; i < 3; i++) {
    vtkCaptionActor2D *caption = i == 0 ? axes->GetXAxisCaptionActor2D() : (i == 1 ? axes->GetYAxisCaptionActor2D() : axes->GetZAxisCaptionActor2D());
    vtkProperty *shaftProperty = i == 0 ? axes->GetXAxisShaftProperty() : (i == 1 ? axes->GetYAxisShaftProperty() : axes->GetZAxisShaftProperty());
    vtkTextProperty *textProperty = caption->GetCaptionTextProperty();
    textProperty->ItalicOff();
    textProperty->SetShadowOffset(0, 0);
    double r, g, b;
    shaftProperty->GetColor(r, g, b);
    textProperty->SetColor(r, g, b);
  }
  _orientationWidget->SetOrientationMarker(axes);
  _orientationWidget->SetInteractor(_RenderWindowInteractor);
  _orientationWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  _orientationWidget->SetEnabled(1);
  _orientationWidget->InteractiveOff();
}

std::unique_ptr<View3D> SIM::View3D::CreateView3D(DocView& docView, const QList<QGraphicsProxyWidget*> & additionalWidgets)
{
	std::unique_ptr<View3D> view = CreateView3D(docView);
	auto pScene = view->scene();
	if (pScene)
	{
		for (auto widget : additionalWidgets)
			pScene->addItem(widget);

		for (auto item : view->items())
		{
			item->setFlag(QGraphicsItem::ItemIsMovable, true);
			item->setFlag(QGraphicsItem::ItemIsFocusable, true);
			item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		}
	}

	return view;
}

SIM::View3D::~View3D(void)
{
    StopPreselectionThread();
}

void SIM::View3D::closeEvent(QCloseEvent *event)
{
	assert(_Widget.get());
	assert(_Widget->GetRenderWindow());

	_Widget->GetRenderWindow()->MakeCurrent();

	// Fire closing signal!
	emit closing();

	if (_CubeMap)
	{
		_CubeMap = NULL;
	}

	// Remove actors
	if (_RenderWindow && _RenderWindow->GetRenderers()->GetNumberOfItems() > 0)
	{
		Clear(false);
	}

	if (_RenderPass)
	{
		vtkRenderPass::SafeDownCast(_RenderPass)->ReleaseGraphicsResources(_Widget->GetRenderWindow());
		_RenderPass = NULL;
	}

	// Shut down autocam
	auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(_Widget->GetRenderWindow());
	if (window)
	{
		auto config = Factory::Instancer<IAppConfig>();
		assert(config);

		// Sync AutoCAM
		auto ac = window->GetAutoCAM();
		if (ac)
		{
			vtkExtAutoCAMSettingsEx settings(config->company(), config->settings());
			ac->saveSettings(&settings);
		}

		window->ShutDown();
	}

	// Remove observer
	/*if ( mModel )
	{
		mModel -> erase ( *this  ) ;
		mModel = NULL ;
	}*/

	_Widget->SetRenderWindow(NULL);
}

void SIM::View3D::resetClippingRange()
{
	_ResetClippingRange = true;
}

void SIM::View3D::updateClippingRange()
{
    if (_ResetClippingRange)
    {
        GetRenderer()->ResetCameraClippingRange();
        _ResetClippingRange = false;
    }
}

void SIM::View3D::render()
{
	if (_Widget.get())
	{
		_Widget->update();
	}
}

void SIM::View3D::drawLogo(QPainter* p)
{
	if (_Logo.isNull())
	{
		auto config = Factory::Instancer<IAppConfig>();
		assert(config);
		if (config)
		{
			_Logo = QImage(config->logo());
		}
	}
	if (!_Logo.isNull() && p)
	{
		int logoWidth = _Logo.width();
		int logoHeight = _Logo.height();

		int x = width() - logoWidth;
		int y = height() - logoHeight;
		p->setRenderHint(QPainter::Antialiasing);
		p->drawImage(x, y, _Logo);
	}
}

void SIM::View3D::drawBackground(QPainter* p, const QRectF& vtkNotUsed(r))
{
	assert(p);
	if (p == nullptr)
	{
		return;
	}

	p->beginNativePainting();
	{
		auto renderWindow = _Widget->GetRenderWindow();
		if (renderWindow)
		{
			renderWindow->PushState();
			{
				renderWindow->OpenGLInitState();
				updateClippingRange();
				renderWindow->Render();
			}
			renderWindow->PopState();
		}
		//drawLogo ( p ) ;
	}
	p->endNativePainting();
}

void SIM::View3D::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	if (scene())
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));

	int width = event->size().width();
	int height = event->size().height();

	// Strange event size
	if (width != 160 && height != 160)
	{
		_Widget->GetRenderWindow()->SetSize(width, height);

		_InteractorAdapter->ProcessEvent(event, _Widget->GetInteractor());
	}

	ZoomChanged(true);

	render();

	event->accept();
}

void SIM::View3D::showEvent(QShowEvent *event)
{
	QGraphicsView::showEvent(event);
	emit resized();
	event->accept();
}

void SIM::View3D::moveEvent(QMoveEvent *event)
{
	QGraphicsView::moveEvent(event);
	render();
}

void SIM::View3D::interactorEvent(QEvent* event)
{
	if (_InteractorAdapter.get() && _Widget.get())
	{
		// Linux X11 conflict with Qt
#if defined(KeyRelease)
#undef KeyRelease
#endif

		if (event->type() == QEvent::KeyRelease)
		{
			if (QKeyEvent* key = dynamic_cast<QKeyEvent*> (event))
			{
				auto text = key->text().toLower();
				if (text.length() && text[0] == 'o')
				{
					ambientOcclusion(!ambientOcclusion());
				}
				event->accept();

				return;
		}
	}
		if (_InteractorAdapter->ProcessEvent(event, _Widget->GetInteractor()))
		{
			render();
			event->accept();
		}
}
}

void SIM::View3D::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);
	//ZoomChanged();
}

void SIM::View3D::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key::Key_Delete)
		NotifyObservers(ExecuteCommandEvent(DeleteCommand::Name, *this));

	QGraphicsView::keyPressEvent(event);
	SetCursor();
}

void SIM::View3D::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsView::keyReleaseEvent(event);
	SetCursor();
}

#ifndef QT_NO_WHEELEVENT
void SIM::View3D::wheelEvent(QWheelEvent *event)
{
	QGraphicsView::wheelEvent(event);;
	ZoomChanged();
}
#endif

void SIM::View3D::enterEvent(QEvent* event)
{
	QGraphicsView::enterEvent(event);
	SetCursor();
}

void SIM::View3D::leaveEvent(QEvent* event)
{
	QGraphicsView::leaveEvent(event);
	SetCursor();
	GetModel().ClearPreselection();
}

void SIM::View3D::focusInEvent(QFocusEvent* event)
{
	QGraphicsView::focusInEvent(event);
	SetCursor();
}

void SIM::View3D::focusOutEvent(QFocusEvent* event)
{
	QGraphicsView::focusOutEvent(event);
	SetCursor();
}

void SIM::View3D::dragEnterEvent(QDragEnterEvent* event)
{
	QGraphicsView::dragEnterEvent(event);
	interactorEvent(event);
}

void SIM::View3D::dragMoveEvent(QDragMoveEvent* event)
{
	QGraphicsView::dragMoveEvent(event);
	interactorEvent(event);
}

void SIM::View3D::dragLeaveEvent(QDragLeaveEvent* event)
{
	QGraphicsView::dragLeaveEvent(event);
	interactorEvent(event);
}

void SIM::View3D::mouseDoubleClickEvent(QMouseEvent * event)
{
	_dblClick = true;
	QGraphicsView::mouseDoubleClickEvent(event);
	_View.NotifyObservers(ExecuteCommandEvent(EditCommand::Name, *this));
}

static void checkMenu(QAction* action, bool state)
{
	if (action)
	{
		action->setCheckable(true);
		action->setChecked(state);
	}
}

int SIM::View3D::popupMenu(int id)
{
	vtkExtAutoCAM* autoCAM = NULL;

	auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(_Widget->GetRenderWindow());
	if (!window)
		return 0;

	autoCAM = window->GetAutoCAM();
	if (!autoCAM)
		return 0;

	ContextMenu menu(this);

	switch (id)
	{
	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_INTERIOR:
	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_EXTERIOR:
	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_FULL:
	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_2D:
	{
		checkMenu(menu.addContextMenuAction("Mini Navigation Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_NAV_SW)));
		checkMenu(menu.addContextMenuAction("Mini View Object Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_VIEW_OBJECT_SW)));
		checkMenu(menu.addContextMenuAction("Mini Tour Building Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_TOUR_BUILDING_SW)));
		menu.addSeparator();
		checkMenu(menu.addContextMenuAction("Full Navigation Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_NAV_SW)));
		checkMenu(menu.addContextMenuAction("View Object Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_VIEW_OBJECT_SW)));
		checkMenu(menu.addContextMenuAction("Tour Building Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_TOUR_BUILDING_SW)));
		menu.addSeparator();
		menu.addContextMenuAction("Go Home");
		menu.addContextMenuAction("Fit to Window");
		menu.addContextMenuAction("Restore Original Center");
		menu.addSeparator();
		menu.addContextMenuAction("Close Wheel");
	} break;

	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_VIEWCUBE:
	{
		menu.addContextMenuAction("Home");
		//menu . addSeparator () ;
		//checkMenu ( menu . addAction ( "Orthographic" ), ( autoCAM -> Checked ( vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORTHOGRAPHIC ) ) ) ;
		//checkMenu ( menu . addAction ( "Perspective"  ), ( autoCAM -> Checked ( vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_PERSPECTIVE ) ) ) ;
		menu.addSeparator();
		menu.addContextMenuAction("Set Home View");
		menu.addContextMenuAction("Set Up Direction");
		menu.addContextMenuAction("Zoom to Fit");

		
		menu.addSeparator();
		checkMenu(menu.addContextMenuAction("Turntable rotation", true), this->_RenderInteractorStyleAutoCAM->GetTurntableRotation());
		checkMenu(menu.addContextMenuAction("Free rotation", true), !this->_RenderInteractorStyleAutoCAM->GetTurntableRotation());
		
	} break;


	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_WHEEL:
	{
		checkMenu(menu.addContextMenuAction("Mini Navigation Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_NAV_SW)));
		checkMenu(menu.addContextMenuAction("Mini View Object Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_VIEW_OBJECT_SW)));
		checkMenu(menu.addContextMenuAction("Mini Tour Building Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_TOUR_BUILDING_SW)));
		menu.addSeparator();
		checkMenu(menu.addContextMenuAction("Full Navigation Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_NAV_SW)));
		checkMenu(menu.addContextMenuAction("View Object Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_VIEW_OBJECT_SW)));
		checkMenu(menu.addContextMenuAction("Tour Building Wheel"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_TOUR_BUILDING_SW)));
	}
	break;

	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_ORBIT:
	{
		checkMenu(menu.addContextMenuAction("Orbit"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT)));
		checkMenu(menu.addContextMenuAction("Orbit Constrainted"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT_CONSTRAINED)));
	}
	break;

	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_ZOOM:
	{
		checkMenu(menu.addContextMenuAction("Zoom"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM)));
		checkMenu(menu.addContextMenuAction("Zoom (Fit all)"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_FIT)));
		checkMenu(menu.addContextMenuAction("Zoom (Selected)"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_SELECTED)));
		checkMenu(menu.addContextMenuAction("Zoom (Window)"), (autoCAM->Checked(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_WINDOW)));
	}
	break;

	case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_CUSTOMIZE:
	{
		checkMenu(menu.addContextMenuAction("ViewCube"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_VIEWCUBE)));
		checkMenu(menu.addContextMenuAction("SteeringWheels"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_STEERINGWHEELS)));
		checkMenu(menu.addContextMenuAction("Pan"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_PAN)));
		checkMenu(menu.addContextMenuAction("Zoom"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM)));
		checkMenu(menu.addContextMenuAction("Orbit"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT)));
		checkMenu(menu.addContextMenuAction("Look At"), (autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_LOOKAT)));
	}
	break;
	}

	if (menu.isEmpty() == false)
	{
		QPoint globalPos = QCursor::pos();
		QAction* action = menu.exec(globalPos);
		if (action)
		{
			std::string command = action->text().toStdString();
			switch (id)
			{
			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_INTERIOR:
			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_EXTERIOR:
			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_FULL:
			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_SW_2D:
			{
				if ("Mini Navigation Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_NAV_SW);
				}
				else if ("Mini View Object Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_VIEW_OBJECT_SW);
				}
				else if ("Mini Tour Building Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_TOUR_BUILDING_SW);
				}
				else if ("Full Navigation Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_NAV_SW);
				}
				else if ("View Object Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_VIEW_OBJECT_SW);
				}
				else if ("Tour Building Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_TOUR_BUILDING_SW);
				}
				else if ("Go Home" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_HOME);
				}
				else if ("Fit to Window" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_FIT);
				}
				else if ("Restore Original Center" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_CENTER);
				}
				else if ("Close Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_CLOSE);
				}
			} break;

			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_VIEWCUBE:
			{
				if ("Home" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_HOME);
				}
				else if ("Orthographic" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORTHOGRAPHIC);
				}
				else if ("Perspective" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_PERSPECTIVE);
				}
				else if ("Set Home View" == command)
				{
					autoCAM->SetHomeView(GetRenderer()->GetActiveCamera());
				}
				else if ("Set Up Direction" == command)
				{
					autoCAM->SetUpDirection();
				}
				else if ("Zoom to Fit" == command)
				{
					ZoomAll();
				}
				else if ("Turntable rotation" == command)
				{
					this->_RenderInteractorStyleAutoCAM->SetTurntableRotation(true);
					this->GetView().GetParent().GetApplication().GetModel().SetTurntableRotation(true);
				}
				else if ("Free rotation" == command)
				{
					this->_RenderInteractorStyleAutoCAM->SetTurntableRotation(false);
					this->GetView().GetParent().GetApplication().GetModel().SetTurntableRotation(false);
				}
			} break;


			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_WHEEL:
			{
				if ("Mini Navigation Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_NAV_SW);
				}
				else if ("Mini View Object Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_VIEW_OBJECT_SW);
				}
				else if ("Mini Tour Building Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_MINI_TOUR_BUILDING_SW);
				}
				else if ("Full Navigation Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_NAV_SW);
				}
				else if ("View Object Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_VIEW_OBJECT_SW);
				}
				else if ("Tour Building Wheel" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_FULL_TOUR_BUILDING_SW);
				}
			}
			break;

			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_ORBIT:
			{
				if ("Orbit" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT);
				}
				else if ("Orbit Constrainted" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT_CONSTRAINED);
				}
			}
			break;

			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_ZOOM:
			{
				if ("Zoom" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM);
				}
				else if ("Zoom (Fit all)" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_FIT);
				}
				else if ("Zoom (Selected)" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_SELECTED);
				}
				else if ("Zoom (Window)" == command)
				{
					autoCAM->Command(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_WINDOW);
				}
			}
			break;

			case vtkExtAutoCAM::MenuID::VTKAUTOCAM_MENU_CUSTOMIZE:
			{
				if ("ViewCube" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_VIEWCUBE);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_VIEWCUBE, !visible);
				}
				else if ("SteeringWheels" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_STEERINGWHEELS);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_STEERINGWHEELS, !visible);
				}
				else if ("Pan" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_PAN);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_PAN, !visible);
				}
				else if ("Zoom" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM, !visible);
				}
				else if ("Orbit" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ORBIT, !visible);
				}
				else if ("Look At" == command)
				{
					bool visible = autoCAM->Visible(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_LOOKAT);
					autoCAM->Show(vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_LOOKAT, !visible);
				}
			}
			break;
			}
		}
	}

	commandCallBack();
	return true;
}

void SIM::View3D::showCursor(bool visible)
{
	if (visible)
	{
		SetCursor();
	}
	else
	{
		_Widget->setCursor(Qt::BlankCursor);
	}
}

void SIM::View3D::createLighting()
{
	if (!_RenderWindow)
		return;

	// All lights off!
	GLfloat ambient[] = { 0, 0, 0, 1 };
	GLfloat zero[] = { 0, 0, 0, 0 };

	GLint maxLights;
	::glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
	for (int i = 0; i < maxLights; i++)
	{
		::glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
		::glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, zero);
		::glLightfv(GL_LIGHT0 + i, GL_SPECULAR, zero);
	}

	SIM::ViewSettings& settings = SIM::View3D::GetSettings();
	_RenderWindow->GetRenderers()->InitTraversal();
	while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
	{
		auto lightKit = vtkSmartPointer<vtkLightKit>::New();
		lightKit->MaintainLuminanceOff();
		lightKit->SetKeyLightIntensity(settings.GetKeyLightIntensity());
		lightKit->SetKeyLightWarmth(settings.GetKeyLightWarmth());
		lightKit->SetFillLightWarmth(settings.GetFillLightWarmth());
		lightKit->SetHeadLightWarmth(settings.GetHeadLightWarmth());
		lightKit->SetBackLightWarmth(settings.GetBackLightWarmth());
		lightKit->AddLightsToRenderer(renderer);
	}
}

void SIM::View3D::ChangeLight()
{
    if (!_RenderWindow)
        return;

	SIM::ViewSettings& settings = SIM::View3D::GetSettings();
    _RenderWindow->GetRenderers()->InitTraversal();
    while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
    {
		renderer->RemoveAllLights();
        auto lightKit = vtkSmartPointer<vtkLightKit>::New();
		lightKit->MaintainLuminanceOff();
        lightKit->SetKeyLightIntensity(settings.GetKeyLightIntensity());
        lightKit->SetKeyLightWarmth(settings.GetKeyLightWarmth());
        lightKit->SetFillLightWarmth(settings.GetFillLightWarmth());
        lightKit->SetHeadLightWarmth(settings.GetHeadLightWarmth());
        lightKit->SetBackLightWarmth(settings.GetBackLightWarmth());
		lightKit->AddLightsToRenderer(renderer);
    }
}

void SIM::View3D::createShader()
{
	//TODO
}

void SIM::View3D::setupRenderPasses(vtkRenderWindow* window)
{
    if (!window)
        return;

#if defined(OPENVR_ENABLED)
    if (vtkOpenVRRenderWindow::SafeDownCast(window))
    {
        setupRenderPassesVR(window);
        return;
    }
#endif

    // Now we override the default rendering path
    auto cameraPass = vtkSmartPointer<vtkCameraPass>::New();
    {
        // Basically default path, minus overlay
        auto skyPass = vtkSmartPointer<vtkExtSkyMapPass>::New();
        auto lightsPass = vtkSmartPointer<vtkLightsPass>::New();
        auto opaquePass = vtkSmartPointer<vtkOpaquePass>::New();
        auto volumePass = vtkSmartPointer<vtkVolumetricPass>::New();
        auto translucentPass = vtkSmartPointer<vtkTranslucentPass>::New();

        auto passes = vtkSmartPointer<vtkRenderPassCollection>::New();
        passes->AddItem(skyPass);
        passes->AddItem(lightsPass);
        passes->AddItem(opaquePass);
        passes->AddItem(volumePass);
        passes->AddItem(translucentPass);

        auto sequencePass = vtkSmartPointer<vtkSequencePass>::New();
        sequencePass->SetPasses(passes);

        cameraPass->SetDelegatePass(sequencePass);

        // Set weak pointer
        //_SkyPass = skyPass;
    }

    // AutoCAM rendering path, enables camera update, pre/post rendering
    auto renderSequencePass = vtkSmartPointer<vtkSequencePass>::New();
    auto acPass = vtkSmartPointer<vtkExtCameraPassAutoCAM>::New();
    {
        // Load resource
        QImage qimage(":/" VTKEXTAMBIENTOCCLUSIONPASS_IMAGE_DEFAULT);

        // Convert resource to VTK
        auto image = vtkSmartPointer<vtkQImageToImageSource>::New();
        image->SetQImage(&qimage);
        image->Update();

        auto shaderSource = QtExtHelpers::loadResource(":/" VTKEXTAMBIENTOCCLUSIONPASS_SHADER_DEFAULT);
        std::string source((char*)shaderSource.data(), shaderSource.size());

        // Cheap ambient occlusion pass
        auto ambientOcclusion = vtkSmartPointer<vtkExtAmbientOcclusionPass>::New();
        ambientOcclusion->SetRings(VTKEXTAMBIENTOCCLUSIONPASS_RINGS_DEFAULT);
        ambientOcclusion->SetSamples(VTKEXTAMBIENTOCCLUSIONPASS_SAMPLES_DEFAULT);
        ambientOcclusion->SetNoise(VTKEXTAMBIENTOCCLUSIONPASS_NOISE_DEFAULT);
        ambientOcclusion->SetGradient(VTKEXTAMBIENTOCCLUSIONPASS_GRADIENT_DEFAULT);
        ambientOcclusion->SetImage(image->GetOutput());
        ambientOcclusion->SetShaderSource(source.c_str());
        ambientOcclusion->SetDelegatePass(cameraPass);

        // Set weak pointer
        _AOPass = ambientOcclusion;

        // Default overlay pass
        auto overlayPass = vtkSmartPointer<vtkOverlayPass>::New();

        auto passes = vtkSmartPointer<vtkRenderPassCollection>::New();
        passes->AddItem(ambientOcclusion);
        passes->AddItem(overlayPass);

        renderSequencePass->SetPasses(passes);

        acPass->SetDelegatePass(renderSequencePass);
    }

    auto windowAutoCam = vtkExtRenderWindowAutoCAM::SafeDownCast(window);

    if (windowAutoCam)
    {
        acPass->SetAutoCAM(windowAutoCam->GetAutoCAM());

        _RenderPass = acPass;

        _RenderWindow->GetRenderers()->InitTraversal();
        while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
        {
            if (vtkExtRendererAutoCAM::SafeDownCast(renderer))
                vtkOpenGLRenderer::SafeDownCast(renderer)->SetPass(acPass);
            else
                vtkOpenGLRenderer::SafeDownCast(renderer)->SetPass(renderSequencePass);
        }
    }

}

void SIM::View3D::ambientOcclusion(bool state)
{
	if (_AOPass)
	{
		_AOPass->SetEnabled(state);
	}

	render();
}

bool SIM::View3D::ambientOcclusion()
{
	if (_AOPass)
	{
		return _AOPass->GetEnabled();
	}
	return false;
}

void SIM::View3D::updateAmbientOcclusion(bool repaint)
{
	if (_AOPass)
	{
		auto config = Factory::Instancer<IAppConfig>();
		assert(config);

		QSettings settings(config->company(), config->settings());

		{
			auto rings = _AOPass->GetRings();
			auto value = settings.value("ao-rings", rings).toInt();
			if (value != rings)
				_AOPass->SetRings(value);
		}

		{
			auto samples = _AOPass->GetSamples();
			auto value = settings.value("ao-samples", samples).toInt();
			if (value != samples)
				_AOPass->SetSamples(value);
		}

		{
			auto noise = _AOPass->GetNoise();
			auto value = settings.value("ao-noise", noise).toFloat();
			if (value != noise)
				_AOPass->SetNoise(value);
		}

		{
			auto gradient = _AOPass->GetGradient();
			auto value = settings.value("ao-gradient", gradient).toFloat();
			if (value != gradient)
				_AOPass->SetGradient(value);
		}

		ambientOcclusion(settings.value("ao", ambientOcclusion()).toBool());
	}

	if (repaint)
	{
		render();
	}
}

void SIM::View3D::commandCallBack()
{
	emit commandNotify();
}

bool SIM::View3D::ribbonCallBack()
{
	auto p = mapFromGlobal(QCursor::pos());

	foreach(QGraphicsItem *item, items())
	{
		const QRectF rect = item->boundingRect();
		if (rect.contains(p.x(), p.y()))
			return true;
	}
	return false;
}

vtkExtAutoCAM* SIM::View3D::autoCAM()
{
	// Sync AutoCAM
	auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(getRenderWindow());
	assert(window);
	if (!window)
		return 0;

	auto ac = window->GetAutoCAM();
	assert(ac);
	return ac;
}

void SIM::View3D::applyRenderProps(vtkActor& actor, RenderProps& props)
{
	vtkProperty* actorProps = actor.GetProperty();
	actorProps->SetOpacity(props.opacity);
	actorProps->SetDiffuse(props.diffuse);
	actorProps->SetAmbient(props.ambient);
	actorProps->SetSpecular(props.specular);
	actorProps->SetSpecularPower(props.power);

	actorProps->ShadingOff();

	switch (props.type)
	{
	case RenderType::Flat: actorProps->SetInterpolationToFlat(); break;
	case RenderType::Gouraud: actorProps->SetInterpolationToGouraud(); break;
	case RenderType::BlinnPhong: actorProps->SetInterpolationToPhong(); break;
	}

	if (props.setColor)
		actorProps->SetColor(props.color.R, props.color.G, props.color.B);

	actorProps->SetEdgeVisibility(props.edges);

	if (props.setColor)
		actorProps->SetEdgeColor(props.edgeColor.R, props.edgeColor.G, props.edgeColor.B);

	actorProps->SetLineWidth(props.edgeWidth);
}

void SIM::View3D::setCubeMap(const std::string& name)
{
	if (_CubeMapName == name && _CubeMap)
		return;

	struct image
	{
		GLenum id;
		std::string name;
	};

	image images[] =
	{
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, "posx.png" },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "negx.png" },
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "posy.png" },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "negy.png" },
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "posz.png" },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "negz.png" }
	};

	if (!name.empty())
	{
		// Load cubemap
		auto cubeMap = vtkSmartPointer<vtkExtCubeMap>::New();
		for (size_t i = 0; i < sizeof(images) / sizeof(images[0]); i++)
		{
			// Get resource name
			QString imageName = QString(":/NfdcAppCore/cubemaps/%1/%2").arg(name.c_str()).arg(images[i].name.c_str());

			// Load resource binary buffer
			QResource imageResource(imageName);

			// Using STB load image from memory
			int width = 0;
			int height = 0;
			int components = 0;
			unsigned char* pixels = (unsigned char*)STB::stbi_load_from_memory((STB::stbi_uc*) imageResource.data(), (int)imageResource.size(), &width, &height, &components, 3);
			if (width && height && components && pixels)
			{
				// Set image in cubemap, i.e. send to OpenGL GPU memory.
				cubeMap->SetImage(images[i].id, width, height, pixels, (components == 3 ? GL_RGB : GL_RGBA));
				// Remove pixels
				free(pixels);
			}
			else if (pixels)
			{
				free(pixels);
			}

		}

		// Update cubemap
		_CubeMap = cubeMap;
	}
	else
	{
		// Remove cube map
		_CubeMap = NULL;
	}

	// Store cubemap name
	_CubeMapName = name;
}

void SIM::View3D::renderPropsValue(double)
{
	if (_RenderPropUpdate)
		_RenderPropUpdate();
}

void SIM::View3D::renderPropsValue(int)
{
	if (_RenderPropUpdate)
		_RenderPropUpdate();
}

void SIM::View3D::renderPropsColor()
{
	if (_RenderPropColor)
		_RenderPropColor(0);
}

void SIM::View3D::renderPropsColorEdge()
{
	if (_RenderPropColor)
		_RenderPropColor(1);
}

View3D & View3D::GetView3D()
{
	return *this;
}

void SIM::View3D::RegisterViewFactory(std::shared_ptr<ViewFactory> factory)
{
	std::unordered_set<std::string>& types = factory->GetObjectTypes();

	for (auto i = types.begin(); i != types.end(); ++i)
		_factories.insert(std::make_pair(*i, factory));
}

void SIM::View3D::InitializeFactories()
{
	RegisterViewFactory(std::make_shared<GeometryViewFactory>(*this));
}

void SIM::View3D::AddObject(Object & element)
{
	if (!element.HasGeometry() || _factories.find(element.GetTypeName()) == _factories.end())
		return;

	auto viewActor = GetOrCreateViewActorForObject(element.GetId(), nullptr);

	if (viewActor)
	{
		auto& viewFactory = _factories[viewActor->GetType()];
		auto actor = viewFactory->AddObjectToActorGeometry(element.GetId(), *viewActor);

		if (actor)
		{
			if (!viewActor->GetAddedToView())
			{
				AddActor(actor, viewActor->GetRenderer());
				viewActor->SetAddedToView(true);
				SetViewActor(*viewActor, actor, element.GetId(), EMode::Regular);
			}
			else
			{
				_objecViewActorMap[viewActor->GetType()][element.GetId()] = viewActor;
			}

            RebuildViewActorCellBoundCache(*viewActor);
		}
	}
}

void SIM::View3D::Notify(Event& ev)
{
	EventSwitch es(ev);
	// general events
	es.Case<ObjectsAddedEvent>(std::bind(&View3D::OnObjectsAdded, this, std::placeholders::_1));
	es.Case<ObjectsRemovedEvent>(std::bind(&View3D::OnObjectsRemoved, this, std::placeholders::_1));
	es.Case<ObjectsUpdatedEvent>(std::bind(&View3D::OnObjectsUpdated, this, std::placeholders::_1));

	// DocModel events
	es.EnableIf<DocModelEvent>([this](auto & docModelEv)
	{
		return &docModelEv.GetModel() == &GetModel();
	});
	es.Case<ModelReloadedEvent>(std::bind(&View3D::OnModelReloaded, this));
	es.Case<SelectionChangedEvent>(std::bind(&View3D::OnSelectionChange, this, false));
	es.Case<PreselectionChangedEvent>(std::bind(&View3D::OnPreselectionChange, this, std::placeholders::_1));
	es.Case<SelectionContextChangedEvent>(std::bind(&View3D::OnSelectionContextChanged, this, std::placeholders::_1));

	es.Case<VisibilityFilterChangedEvent>(std::bind(&View3D::OnUpdateVisibilityFilter, this));
	es.Case<SelectionFilterChangedEvent>(std::bind(&View3D::OnUpdateSelectionFilter, this));
	es.Case<HighlightsChangedEvent>(std::bind(&View3D::OnHighlightsChanged, this));
	es.Case<ObjectsVisibilityChangedEvent>(std::bind(&View3D::OnObjectsVisibilityChanged, this, std::placeholders::_1));
	
	es.Case<PreviewChangedEvent>([this](PreviewChangedEvent& previewChanged) {
		this->render();
	});
	
	// DocView events
	es.EnableIf<DocViewEvent>();
	es.Case<AutoCAMExecuteCommandEvent>([this](AutoCAMExecuteCommandEvent & autocamCommandEv)
	{
		if (&autocamCommandEv.GetDocument() == &_View.GetParent())
		{
			auto command = autocamCommandEv.GetCommand();
			if (command != vtkExtAutoCAM::VTKAUTOCAM_CMD_NONE)
			{
                if (command == vtkExtAutoCAM::VTKAUTOCAM_CMD_ZOOM_FIT)
                    ZoomAll();
                else
                {
                    auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(getRenderWindow());
                    assert(window && window->GetAutoCAM());
                    if (!window || !window->GetAutoCAM())
                        return;

                    window->GetAutoCAM()->Command(command);
                }
			}
		}
	});
}

SIM::ViewFactory* SIM::View3D::GetFactory(Object& element, std::string type)
{
	auto typeName = type.empty() ? typeid(element).name() : type;

	if (_factories.find(typeName) != _factories.end())
		return _factories[typeName].get();

	return nullptr;
}

ViewFactory* SIM::View3D::GetFactory(ViewActor& viewActor)
{
	if (_factories.find(viewActor.GetType()) == _factories.end())
		return nullptr;

	return _factories[viewActor.GetType()].get();
}

void SIM::View3D::ZoomAll()
{
	auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(_Widget->GetRenderWindow());
	if (!window)
		return;

	auto autoCAM = window->GetAutoCAM();
	if (!autoCAM)
		return;

	auto camera = GetRenderer()->GetActiveCamera();
	auto modelViewMatrix = camera->GetModelViewTransformMatrix();

	vtkBoundingBox sceneBox;
	bool boundSet = false;
	for (auto i = _actors.begin(); i != _actors.end(); i++)
	{
		if (!i->get()->GetActor())
			continue;

		ViewActorIterator& it = i->get()->GetActorIterator();

		for (int j = 0; j < it.GetCount(); j++)
		{
			auto actor = it.GetItemAsActor(j);
			if (actor && actor->GetVisibility())
			{
				auto mapper = dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());

				if (!mapper)
					continue;

				auto data = mapper->GetInput();

				if (!data)
					continue;

				auto vtkpoints = data->GetPoints();

				if (!vtkpoints)
					continue;

				for (int i = 0; i < vtkpoints->GetNumberOfPoints();i++)
				{
					auto pt = vtkpoints->GetPoint(i);
					double pt4[4] = { pt[0], pt[1], pt[2], 0 };
					auto pointTmp = modelViewMatrix->MultiplyDoublePoint(pt4);

					sceneBox.AddPoint(pointTmp);
				}
				boundSet = true;
			}
		}

	}

	if (!boundSet)
		return;

	double center[4];
	sceneBox.GetCenter(center);
	center[3] = 0;

	auto matInv = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(modelViewMatrix, matInv);
	auto centerGlobal = matInv->MultiplyDoublePoint(center);

	internal::Vec3d focal;
	internal::Vec3d eyePos;
	camera->GetFocalPoint(focal.ptr());
	camera->GetPosition(eyePos.ptr());
	auto dx = focal - eyePos;
	dx.normalize();

	double len = sceneBox.GetMaxLength() * 2;

	auto size = GetSize();
	if (autoCAM->perspective())
	{
		double l = std::max(sceneBox.GetLength(0), sceneBox.GetLength(1));

		if (size[1] > size[0])
		{
			l *= (1.2 * size[1]) / size[0];
		}
		len = sceneBox.GetLength(2) + l;
	}

	internal::Vec3d eye = internal::Vec3d(centerGlobal[0], centerGlobal[1], centerGlobal[2]) - dx*len;

	GetRenderer()->ResetCamera();
	camera->SetFocalPoint(centerGlobal[0], centerGlobal[1], centerGlobal[2]);
	camera->SetPosition(eye.ptr());

	if (!autoCAM->perspective())
	{
		if (size[0] > 0)
		{
			double parallelScale1 = sceneBox.GetLength(1);
			double parallelScale2 = sceneBox.GetLength(0)*size[1] / size[0];
			double parallelScale = 0.7*std::max(parallelScale1, parallelScale2);
			camera->SetParallelScale(parallelScale);
		}
	}

	camera->Modified();
	autoCAM->UpdateCamera(camera);

	resetClippingRange();
	ZoomChanged(true);
	render();
}


void SIM::View3D::SetCamera()
{
	auto window = vtkExtRenderWindowAutoCAM::SafeDownCast(_Widget->GetRenderWindow());
	if (window)
	{
		GetRenderer()->ResetCamera();
		auto camera = GetRenderer()->GetActiveCamera();
		camera->SetFocalPoint(0, 0, 0);
		camera->SetPosition(1, 1, 0);
		camera->SetViewUp(0, 0, 1);
		window->SetFrontDirection(0, 1, 0);
		window->SetUpDirection(camera);
	
		Vec3 dv = Vec3(1, -1, 1);

		vtkBoundingBox sceneBox;
		bool boundSet = false;
		for (auto i = _actors.begin(); i != _actors.end(); i++)
		{
			if (!i->get()->GetActor())
				continue;

			vtkProp* actor = i->get()->GetActor().Get();
			if (actor && actor->GetVisibility())
			{
				sceneBox.AddBounds(actor->GetBounds());
				boundSet = true;
			}
		}

        if (!boundSet)
        {
            double bb[6] = { 0,1,0,1,0,1 };
            sceneBox.AddBounds(bb);
            boundSet = true;
        }

		if (boundSet)
		{
			Vec3 center, max, min;
			double len = sceneBox.GetMaxLength();
			sceneBox.GetCenter(center.ptr());
			sceneBox.GetMaxPoint(max.x, max.y, max.z);
			sceneBox.GetMinPoint(min.x, min.y, min.z);

			dv.normalize();
			Vec3 eye = center + dv*len;
			Vec3 up = dv.cross(Vec3(0, 0, 1)).cross(dv);

			camera->SetFocalPoint(center.ptr());
			camera->SetPosition(eye.ptr());
			camera->SetViewUp(up.ptr());
		}

        ZoomAll();
		UpdateRotationCenter();
		window->SetHomeView(camera);
		window->GetAutoCAM()->Command(vtkExtAutoCAM::VTKAUTOCAM_CMD_HOME);
	}
}

void SIM::View3D::OnObjectsUpdated(ObjectsUpdatedEvent & elUpdatedEv)
{
	auto& objects = elUpdatedEv.GetObjects();

	std::set<ViewActor*> actorsToRebuild;
	std::set<Object*> regenerated;

    bool isGeometry = false;
	for (auto objModified : objects)
	{
		if (!objModified.get())
			continue;

        if (dynamic_cast<Geometry*>(objModified.get()))
            isGeometry = true;

		auto obj = objModified->GetMainObject();

		if (regenerated.find(obj) != regenerated.end())
			continue;

		auto viewActor = GetViewActor(*obj, EMode::Regular);
		if (viewActor != nullptr)
		{
			ViewFactory* factory = GetFactory(*obj);

			if (!factory)
				continue;

			if (factory->RegenerateViewActorOnUpdate(elUpdatedEv.GetContext()))
			{
				actorsToRebuild.insert(viewActor);
                regenerated.insert(obj);
			}
			else
			{
				factory->Update(*viewActor, objModified->GetId(), elUpdatedEv.GetContext());
			}
		}
	}

	for (auto vA : actorsToRebuild)
	{
		RebuildViewActor(*vA);
	}

    if (isGeometry && elUpdatedEv.GetContext() == "NewGeometryLoaded")
    {
        BoundModified();
        if (!_firstGeometryLoaded)
        {
            _firstGeometryLoaded = true;
            SetCamera();
        }
        else
        {
            ZoomAll();
        }
    }

	OnSelectionChange(true);
}

void SIM::View3D::OnObjectsAdded(ObjectsAddedEvent & elAddedEv)
{
	auto& objects = elAddedEv.GetObjects();

    bool isGeometry = false;



	for (auto obj : objects)
	{
		if (obj->HasGeometry())
			AddObject(*obj.get());

        if (dynamic_cast<Geometry*>(obj.get()))
            isGeometry = true;
	}

    if (isGeometry)
    {
        BoundModified();
        if (!_firstGeometryLoaded)
        {
            _firstGeometryLoaded = true;
            SetCamera();
        }
        else
        {
            ZoomAll();
        }
    }
    else
    {
        BoundModified();
	    render();
    }
}

void SIM::View3D::OnObjectsRemoved(ObjectsRemovedEvent & elRemovedEv)
{
	auto& objects = elRemovedEv.GetObjects();
	std::unordered_set<ViewActor*> actorsToRebuild;

	for (auto obj : objects)
	{
            auto viewActor = GetViewActor(*obj->GetMainObject(), EMode::Regular);
            if (viewActor != nullptr)
            {
                if(obj->IsMainObject())
                    viewActor->RemoveObject(obj->GetId());
                actorsToRebuild.insert(viewActor);
            }
	}

	for (auto viewActor : actorsToRebuild)
	{
		RebuildViewActor(*viewActor);
	}
	BoundModified();
	render();
}

void SIM::View3D::Clear(bool clearSelection)
{
	if (clearSelection)
	{
		ClearPreselection();
		ClearSelection();
	}
	for (auto it = _actors.begin(); it != _actors.end(); ++it)
	{
		std::shared_ptr<ViewActor>& actor = *it;
		if (!actor->GetActor())
			continue;
		GetRenderer(actor->GetRenderer())->RemoveActor(actor->GetActor().Get());
		// _Renderer->BoundsModified();
	}
	_actors.clear();
	_actorsMap.clear();
	_dedicatedActorsForElements.clear();
	_descriptions.clear();
	_objecViewActorMap.clear();
}


void SIM::View3D::ClearSelection()
{
	for (auto id : _selection)
	{
		auto objPtr = GetModel().GetObjectById(id);

		if (objPtr) {
			auto obj = objPtr->GetMainObject();
			ViewFactory* factory = GetFactory(*obj);

			if (!factory)
				continue;

			if (!factory->CreateNewGeometryForSelection())
			{
				ViewActor* globalActor = GetViewActor(*obj, EMode::Regular);

				if (globalActor != nullptr)
					factory->SetColor(*globalActor, id, EMode::Regular, objPtr->GetContext());
			}
		}
	}

	for (auto it = _selectedActors.begin(); it != _selectedActors.end(); ++it)
	{
		std::shared_ptr<ViewActor>& actor = *it;
		if (!actor->GetActor())
			continue;

		GetRenderer(actor->GetRenderer())->RemoveActor(actor->GetActor().Get());
		BoundModified();
	}

	_selectedActors.clear();
	_selection.clear();
}

void SIM::View3D::OnModelReloaded()
{
	Clear(true);

	ViewActor* lastActor = nullptr;

	auto model = GetModel().GetStorage().GetObjects();

	for (auto it = model.begin(); it != model.end(); ++it) 
    {
		auto mapItem = *it;
		lastActor = GetOrCreateViewActorForObject(mapItem.first, lastActor);

        if (dynamic_cast<Geometry*>(mapItem.second.get()))
        {
            _firstGeometryLoaded = true;
        }
	}

	for (auto & viewActor : _actors)
	{
		if (!viewActor->GetAddedToView())
		{
			RebuildViewActor(*viewActor.get());
		}
	}

    SetCamera();
}

ViewActor* SIM::View3D::GetViewActor(Object& element, EMode mode, std::string type)
{
	auto typeName = type.empty() ? typeid(element).name() : type;

	auto id = element.GetId();
	auto itObj = _objecViewActorMap[typeName].find(id);
	if (itObj != _objecViewActorMap[typeName].end())
		return itObj->second;

	std::vector<std::shared_ptr<ViewActor>>& actors = GetActors(mode);

	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		std::shared_ptr<ViewActor>& actor = *it;

		if (IsViewActorValid(actor.get(), element, type))
			return actor.get();
	}

	return nullptr;
}

ViewActor * SIM::View3D::GetViewActor(ObjectId objId, std::string type)
{
	auto itObj = _objecViewActorMap[type].find(objId);
	if (itObj != _objecViewActorMap[type].end())
		return itObj->second;

	return nullptr;
}

bool SIM::View3D::IsViewActorValid(ViewActor* viewActor, Object& elem, std::string type)
{
	Object* element = elem.GetMainObject();
	auto typeName = type.empty() ? typeid(*element).name() : type;

	if (viewActor->GetType() == typeName)
	{
		ViewFactory* factory = GetFactory(*element, type);
		if (factory && factory->IsViewActorValid(*element, *viewActor))
		{
			return viewActor;
		}
	}

	return false;
}

std::shared_ptr<ViewActor> SIM::View3D::CreateViewActor(Object& element, EMode mode, std::string type)
{
	auto factory = GetFactory(element, type);

	if (factory)
	{
		auto typeName = type.empty() ? element.GetTypeName() : type;
		std::shared_ptr<ViewActor> viewActor = factory->CreateViewActor();
		viewActor->SetType(typeName);
		factory->SetViewActorData(element, *viewActor.get());
		viewActor->SetRenderer(factory->GetRenderer(*viewActor.get()) + _defaultRenderer);
		AddViewActorToCollection(viewActor, mode);
		return viewActor;
	}

	return std::shared_ptr<ViewActor>();
}

void SIM::View3D::SetViewActor(ViewActor& viewActor, vtkSmartPointer<vtkProp> &actor, ObjectUniqueKey obj, EMode mode)
{
	viewActor.SetActor(actor);

	if (mode == Regular)
	{
		_actorsMap[actor.Get()] = &viewActor;

		if (obj != NullObjectKey)
			_objecViewActorMap[viewActor.GetType()][obj] = &viewActor;
	}
}

std::vector<std::shared_ptr<ViewActor>> & SIM::View3D::GetActors(EMode mode)
{
	if (mode == Selection)
		return _selectedActors;

	return _actors;
}

void SIM::View3D::OnSelectionChange(bool force)
{
	if (!force && GetModel().CompareSelection(_selection) || !GetModel().GetHighlights())
		return;

	ClearSelection();

	_selection = GetModel().GetSelection();

	for (auto id : _selection)
	{
		auto objPtr = GetModel().GetObjectById(id);

		if (objPtr) {
			Object& obj = *objPtr->GetMainObject();
			std::string context = objPtr->GetContext();
			ViewFactory* factory = GetFactory(obj);

			if (!factory)
				continue;

			if (factory->CreateNewGeometryForSelection())
			{
				ViewActor* globalActor = GetViewActor(obj, EMode::Selection);

				if (globalActor == nullptr)
				{
					globalActor = CreateViewActor(obj, EMode::Selection).get();
				}

				vtkSmartPointer<vtkProp> actor = factory->AddObjectToActorGeometry(id, *globalActor);

				if (!globalActor->GetActor())
				{
					SetViewActor(*globalActor, actor, id, EMode::Selection);					
					ViewActorIterator iterator(actor);
					iterator.ForEachVtkActor([this](vtkActor& vtkact, int) {
						vtkact.GetProperty()->SetColor(0, 1, 0);
						applyRenderProps(vtkact, GetSettings().GetSelectionProps());							
					});
					AddActor(actor, globalActor->GetRenderer());
					BoundModified();
				}
			}
			else
			{
				ViewActor* globalActor = GetViewActor(obj, EMode::Regular);

				if (globalActor != nullptr)
					factory->SetColor(*globalActor, id, EMode::Selection, context);
			}
		}
	}

	render();
}

ObjectId SIM::View3D::PickObject(EMode mode, bool addToSelection)
{
	if (!_RenderWindowInteractor)
		return NullObjectKey;

	vtkSmartPointer<vtkPolyData> selection;

	auto nothing = []()
	{
	};

	auto callBackWrapper = [this]()
	{
		QtExtHelpers::QOverrideCursor::restore();

		// Trigger button release!
		this->_RenderWindowInteractor->LeftButtonReleaseEvent();

		QApplication::setOverrideCursor(Qt::CrossCursor);
	};

	// Update and Notify
	auto done = [&, this](std::function<void()> action)
	{
		if (mode == Selection)
		{
			Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
			if (!keyMod.testFlag(Qt::ControlModifier) && !keyMod.testFlag(Qt::ShiftModifier))
			{
				GetModel().ClearSelection();
			}

			NotifyObservers(MouseClickEvent(*this, _View.GetParent(), PickData(NullObjectKey, -1, Point3(), Point3())));
		}
		else
		{
			GetModel().ClearPreselection();

			NotifyObservers(MouseMoveEvent(*this, _View.GetParent(), PickData(NullObjectKey, -1, Point3(), Point3())));
		}
		action();
	};

	// Get event position
	const int* eventPosition = _RenderWindowInteractor->GetEventPosition();
	if (!eventPosition)
	{
		done(nothing);
		return NullObjectKey;
	}

	vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
	coordinate->SetCoordinateSystemToDisplay();
	coordinate->SetValue(eventPosition[0], eventPosition[1], 0);
	
	
	Point3 normal;
	vtkIdType pickedIndex;
	Point3 position;
	double smallestLen = 0;
	ObjectId id = NullObjectKey;
	Vec3 crossPoint;
	for(int rendererId = _renderersCount - 1; rendererId >= 0; rendererId--)
	{
		vtkRenderer* renderer = GetRenderer(rendererId);
		double* world = coordinate->GetComputedWorldValue(renderer);

		double* eyePos = GetActiveCamera()->GetPosition();
		double center[3];
		GetActiveCamera()->GetFocalPoint(center);
		Vec3 eyePositionVec(eyePos[0], eyePos[1], eyePos[2]);
		Vec3 centerVec(center);
		Vec3 dx = centerVec - eyePositionVec;
		dx.normalize();
		dx *= GetModel().GetGlobalBounds().GetLength();
		eyePositionVec.x = world[0];
		eyePositionVec.y = world[1];
		eyePositionVec.z = world[2];
		centerVec = eyePositionVec + dx;		
        
		
		// Do cell pick
		int tolerancePx = 0;
		vtkSmartPointer<vtkPicker>  picker;

		if (GetModel().GetSelectionContext() == SelectionContext::Node)
		{
			picker = vtkSmartPointer<vtkExtSimPointPicker>::New();
			tolerancePx = 20;
		}
		else if (GetModel().GetSelectionContext() == SelectionContext::Edge)
		{
			picker = vtkSmartPointer<vtkExtSimCellPicker>::New();

			tolerancePx = 10;
		}
		else
		{
			picker = vtkSmartPointer<vtkExtSimCellPicker>::New();
		}

        vtkExtSimCellPicker* cellPicker = vtkExtSimCellPicker::SafeDownCast(picker.Get());


        for (auto & viewActor : _actors)
        {
            if (viewActor->GetAddedToView() && viewActor->GetActor() && viewActor->GetRenderer() == rendererId)
            {
                if (viewActor->GetActor()->GetVisibility())
                {
                    auto& viewFactory = _factories[viewActor->GetType()];

                    if (viewFactory->OwnPickMechanism())
                    {
                        ObjectId res;
                        Vec3 pt;
                        if (viewFactory->Pick(*viewActor.get(), eyePositionVec, centerVec, pt, res))
                        {
                            double l1 = (eyePositionVec - pt).length();
                            if (smallestLen > l1 || id == NullObjectKey)
                            {
                                smallestLen = l1;
                                id = res;
                                crossPoint = pt;
                            }
                        }
                    }
                    else
                    {
                        if (cellPicker)
                        {
                            viewActor->GetActorIterator().ForEachVtkActor([&](vtkActor& vtkAct, int index) {
                                auto locator = viewActor->GetLocator(index);
                                if(locator)
                                    cellPicker->AddLocator(locator);                               
                            });
                        }
                    }
                }
            }
        }

		double diag = std::sqrt(renderer->GetSize()[0] * renderer->GetSize()[0] + renderer->GetSize()[1] * renderer->GetSize()[1]);

		if (diag > 0)
		{
			double tolerance = tolerancePx / diag;
			picker->SetTolerance(tolerance);
		}

		bool pickResult = picker->Pick(eventPosition[0], eventPosition[1], 0, renderer);

		if (!pickResult && id == NullObjectKey)
		{
			continue;
		}

		if (pickResult)
		{
			vtkIdType actorIndex = -1;

			vtkProp* actor = picker->GetAssembly();

			if (!actor)
				actor = picker->GetActor();

			if (_actorsMap.find(actor) != _actorsMap.end())
			{
				auto viewActor = _actorsMap[actor];

				vtkExtSimCellPicker* cellPicker = vtkExtSimCellPicker::SafeDownCast(picker.Get());
				if (cellPicker)
				{
					pickedIndex = cellPicker->GetCellId();
					if (pickedIndex != -1)
					{
						if (cellPicker->GetSelectedActor())
						{
							actorIndex = viewActor->GetActorIterator().GetIndex(cellPicker->GetSelectedActor());
							normal = Point3(cellPicker->GetPickNormal()[0], cellPicker->GetPickNormal()[1], cellPicker->GetPickNormal()[2]);
						}
					}

				}
				else
				{
					vtkExtSimPointPicker* pointPicker = vtkExtSimPointPicker::SafeDownCast(picker);
					if (pointPicker)
					{
						pickedIndex = pointPicker->GetPointId();

						if (pickedIndex != -1)
						{
							if (pointPicker->GetSelectedActor())
							{
								actorIndex = viewActor->GetActorIterator().GetIndex(pointPicker->GetSelectedActor());
							}
						}
					}
				}

				if (actorIndex != -1)
				{
					auto pickedId = viewActor->GetObjectOfCell(pickedIndex, actorIndex, GetModel().GetSelectionContext());
					Vec3 pickedPos(picker->GetPickPosition()[0], picker->GetPickPosition()[1], picker->GetPickPosition()[2]);
					double l1 = (eyePositionVec - pickedPos).length();
					if (smallestLen > l1 || id == NullObjectKey)
					{
						smallestLen = l1;
						id = pickedId;

						position = Point3(picker->GetPickPosition()[0], picker->GetPickPosition()[1], picker->GetPickPosition()[2]);
					}
				}
			}
		}

		if (id != NullObjectKey)
		{
			break;
		}
	}

	if (id != NullObjectKey)
	{
		if (mode == EMode::Selection)
		{
			Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
			if (keyMod.testFlag(Qt::ControlModifier) || addToSelection)
			{
				GetModel().AddToSelection(id);
			}
			else if (keyMod.testFlag(Qt::ShiftModifier))
			{
				GetModel().RemoveFromSelection(id);
			}
			else
			{
				GetModel().SetSelection(id);
			}


			NotifyObservers(MouseClickEvent(*this, _View.GetParent(), PickData(id, pickedIndex, position, normal)));
		}
		else if (mode == EMode::Preselection)
		{
			GetModel().SetPreselection(id);
			NotifyObservers(MouseMoveEvent(*this, _View.GetParent(), PickData(id, pickedIndex, position, normal)));
		}
	}
	else
	{
		done(nothing);
		return NullObjectKey;
	}

	return id;
}

void SIM::View3D::AnalyzeAreaSelection(EMode mode)
{
		std::unordered_set<ObjectId> selection;
		auto frustum =  vtkSmartPointer<vtkPlanes>::New();
		std::vector<vtkObject*> selectedAct;
		bool frustumSet = false;
		{
			std::lock_guard<std::mutex> lock(this->_preselectionMutex);		
			for (auto& areaPicker : _areaPickers)
			{
				auto props = areaPicker->GetProp3Ds();

				for (vtkIdType i = 0; i < props->GetNumberOfItems(); i++)
				{
					auto prop = props->GetItemAsObject(i);

					if (prop)
					{
						selectedAct.push_back(prop);
					}
				}
				if (!frustumSet)
				{
					frustumSet = true;
					frustum->SetPoints(areaPicker->GetFrustum()->GetPoints());
					frustum->SetNormals(areaPicker->GetFrustum()->GetNormals());
				}
			}
		}

		for (auto & viewActor : _actors)
		{
			if (viewActor->GetAddedToView() && viewActor->GetActor())
			{
				if (viewActor->GetActor()->GetVisibility())
				{
					auto& viewFactory = this->_factories[viewActor->GetType()];

					if (viewFactory->OwnPickMechanism())
					{
						viewFactory->PickArea(*viewActor.get(), _areaSelectionMin, _areaSelectionMax, _areaSelectionRespectBoundCells, frustum, selection);
					}
				}
			}
		}

		
		for (auto selprop : selectedAct)
		{
			vtkProp* prop = vtkProp::SafeDownCast(selprop);

			if (prop)
			{
				if (_actorsMap.find(prop) != _actorsMap.end())
				{
					auto viewActor = this->_actorsMap[prop];

					std::unordered_set<ObjectId> exluded;

					for (int k = 0;k < viewActor->GetActorIterator().GetCount(); k++)
					{
						auto actor = viewActor->GetActorIterator().GetItemAsActor(k);

						if(actor && actor->GetPickable())
						{
							auto data = actor->GetMapper()->GetInput();

							auto objects = viewActor->GetOrderedObjects(k, GetModel().GetSelectionContext());
							auto cells = viewActor->GetOrderedCells(k, GetModel().GetSelectionContext());

							if (objects && cells)
							{
								auto ext = vtkSmartPointer<vtkExtSimExtractSelectedFrustum>::New();
								ext->SetFrustum(frustum);
								ext->SetInputData(data);
								ext->SetRespectBoundCells(_areaSelectionRespectBoundCells);
								ext->PreserveTopologyOn();
								ext->SetFieldType(vtkSelectionNode::CELL);
								ext->SetContainingCells(!_areaSelectionRespectBoundCells);

								if (mode == EMode::Preselection)
								{
									ext->SetCellCache(&viewActor->GetCellCache(k));
								}
								ext->Update();

								auto polydata = vtkPolyData::SafeDownCast(ext->GetOutput());
								auto ar1 = polydata->GetCellData()->GetArray("vtkInsidedness");

								for (int i = 0; i < objects->size(); i++)
								{
									auto id = objects->at(i);

									if (exluded.find(id) != exluded.end() && !_areaSelectionRespectBoundCells)
										continue;

									if (i * 2 + 1 >= cells->size())
										break;

									int cellStart = cells->at(i * 2);
									int cellEnd = cells->at(i * 2 + 1);

									bool ok = false;
									for (int j = cellStart; j <= cellEnd; j++)
									{
										if (ar1->GetTuple1(j) > 0)
										{
											ok = true;
											if (_areaSelectionRespectBoundCells)
												break;
										}
										else
										{
											ok = false;
											if (!_areaSelectionRespectBoundCells)
												break;
										}
									}
									if (ok)
									{
										selection.insert(objects->at(i));
									}
									else
									{
										exluded.insert(id);

										if (!_areaSelectionRespectBoundCells)
											selection.erase(id);
									}
								}
							}
						}
					}

				}
			}
		}


		if (mode == EMode::Selection)
		{
			Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
			if (keyMod.testFlag(Qt::ControlModifier))
			{
			this->GetModel().AddToSelection(selection);
			}
			else if (keyMod.testFlag(Qt::ShiftModifier))
			{
			this->GetModel().RemoveFromSelection(selection);
			}
			else
			{
			this->GetModel().SetSelection(selection);
			}
		}
		else
		{
			{
				MainThreadDispatcher::Post([this, selection]()
				{
					auto selectionCopy = selection;
					this->GetModel().SetPreselection(selectionCopy);
				});
			}
		}
	
}

void SIM::View3D::StopPreselectionThread()
{
    if (_areaPreselectionThreadIsRunning)
    {
        _stopAreaPreselection = true;
        if (_areaPreselectionThread.joinable())
        {
            _areaPreselectionThread.join();
        }
        _areaPreselectionThreadIsRunning = false;
    }
}

void SIM::View3D::RebuildViewActorCellBoundCache(ViewActor & actor)
{
	actor.GetActorIterator().ForEachVtkActor([&](vtkActor& vtkAct, int index) {

		auto mapper = vtkAct.GetMapper();
		double ar[6];

		if (mapper)
		{			
			vtkDataSet* dataSet = vtkAct.GetMapper()->GetInput();

			if (dataSet)
			{
				auto count = dataSet->GetNumberOfCells();

				for (vtkIdType i = 0; i < count; i++)
				{
					dataSet->GetCellBounds(i, ar);
					actor.SetCellBound(i, ar, index);
				}

                vtkPolyData* polyData = vtkPolyData::SafeDownCast(dataSet);

                if (polyData && polyData->GetNumberOfLines() == 0 && vtkAct.GetProperty()->GetRepresentation() != VTK_POINTS)
                {
                    auto locator = vtkSmartPointer<vtkCellTreeLocator>::New();
                    locator->SetDataSet(dataSet);
                    locator->BuildLocator();
                    actor.SetLocator(index, locator);
                }
                else
                {
                    actor.SetLocator(index, nullptr);
                }
			}
		}
	});

}


void SIM::View3D::RebuildViewActorsCellBoundCache()
{
	for (auto actor : _actors)
	{
		RebuildViewActorCellBoundCache(*actor.get());
	}
}

void SIM::View3D::UpdateRotationCenter()
{
	auto pt = (GetModel().GetGlobalBounds().GetMin() + GetModel().GetGlobalBounds().GetMax()) / 2;
	_RenderInteractorStyleAutoCAM->SetRotationCenter(pt.x, pt.y, pt.z);
}

void SIM::View3D::PickAreaCallbackFunction(EMode mode)
{	
	{
		std::lock_guard<std::mutex> lock(this->_preselectionMutex);	
		
		_RenderInteractorStyleAutoCAM->GetMinMax(_areaSelectionMin, _areaSelectionMax, _areaSelectionRespectBoundCells);
		_RenderWindow->GetRenderers()->InitTraversal();
		_areaPickers.clear();
		while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
		{
			auto areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
			_areaPickers.push_back(areaPicker);
			areaPicker->AreaPick(_areaSelectionMin[0], _areaSelectionMin[1], _areaSelectionMax[0], _areaSelectionMax[1], renderer);
		}

        if (mode == EMode::Preselection)
        {
            _areaPreselectionThreadIteration++;
        }
	}

	if (mode == EMode::Selection)
	{
		StopPreselectionThread();
		AnalyzeAreaSelection(mode);
	}
	else
	{
		if (!_areaPreselectionThreadIsRunning)
		{
            _areaPreselectionThreadIteration = 0;
			auto loop = [&, this](EMode mode)
			{
                int iteration = this->_areaPreselectionThreadIteration-1;
				for (;;)
				{
					if (!_stopAreaPreselection)
					{
                        if (iteration != this->_areaPreselectionThreadIteration)
                        {
                            iteration = this->_areaPreselectionThreadIteration;
                            AnalyzeAreaSelection(mode);
                        }
					}
					else
					{
						break;
					}
				}
			};
            _stopAreaPreselection = false;
            _areaPreselectionThreadIsRunning = true;
			_areaPreselectionThread = std::thread(loop, mode);
		}
	}

	render();
}

void SIM::View3D::SetCursor(bool forceArrow)
{
	auto interactorStyle = vtkExtInteractorStyleAutoCAM::SafeDownCast(_RenderWindowInteractor->GetInteractorStyle());
	if (interactorStyle)
	{
		switch (interactorStyle->GetState())
		{
		case VTKIS_ROTATE:
			_Widget->setCursor(QCursor(QPixmap(":/NfdcAppCore/images/sworbit.png")));
			return;

		case VTKIS_PAN:
			_Widget->setCursor(QCursor(QPixmap(":/NfdcAppCore/images/swpan.png")));
			return;

		case VTKIS_DOLLY:
			_Widget->setCursor(QCursor(QPixmap(":/NfdcAppCore/images/swzoom.png")));
			return;

		default:
			_Widget->setCursor(Qt::ArrowCursor);
			break;
		}
	}

	Qt::KeyboardModifiers keyMod = QApplication::queryKeyboardModifiers();
	if (forceArrow)
	{
		_Widget->setCursor(Qt::ArrowCursor);
	}
	else if (keyMod.testFlag(Qt::ControlModifier))
	{
		_Widget->setCursor(QCursor(QPixmap(":/NfdcAppCore/images/cursor_plus.png")));
	}
	else if (keyMod.testFlag(Qt::ShiftModifier))
	{
		_Widget->setCursor(QCursor(QPixmap(":/NfdcAppCore/images/cursor_minus.png")));
	}
	else
	{
		_Widget->setCursor(Qt::ArrowCursor);
	}
}

void SIM::View3D::HideAll()
{
	for (auto it = _actors.begin(); it != _actors.end(); ++it)
	{
		std::shared_ptr<ViewActor>& actor = *it;
		if (actor->GetAddedToView() && actor->GetActor())
		{
			actor->GetActor()->VisibilityOff();
		}
	}
}

void SIM::View3D::SaveCameraSettings()
{
	//if(_activeSystem == NullObjectKey)
	//	return;

	//auto camera = _Renderer -> GetActiveCamera () ;

	//CameraSettings cs;
	//cs.eye = internal::Vec3d(camera->GetPosition()[0],camera->GetPosition()[1],camera->GetPosition()[2]);
	//cs.center = internal::Vec3d(camera->GetFocalPoint()[0],camera->GetFocalPoint()[1],camera->GetFocalPoint()[2]);
	//cs.up = internal::Vec3d(camera->GetViewUp()[0],camera->GetViewUp()[1],camera->GetViewUp()[2]);

	//GetModel().SetCameraSettings(_activeSystem,cs);
}

void SIM::View3D::LoadCameraSettings()
{
}

void SIM::View3D::AddViewActorToCollection(std::shared_ptr<ViewActor>& actor, EMode mode)
{
	GetActors(mode).push_back(actor);
}

ViewActor* SIM::View3D::GetOrCreateViewActorForObject(ObjectId objectId, ViewActor* suggestedViewActor, std::string type)
{
	auto obj = GetModel().GetObjectById(objectId);

	if (obj && obj->HasGeometry())
	{
		Object& item = *obj;
		ViewFactory* factory = GetFactory(item, type);

		if (factory != nullptr)
		{
			ViewActor* globalActor = nullptr;

			if (suggestedViewActor && IsViewActorValid(suggestedViewActor, item, type))
				globalActor = suggestedViewActor;
			else
				globalActor = GetViewActor(item, EMode::Regular, type);

			if (globalActor == nullptr)
			{
				globalActor = CreateViewActor(item, EMode::Regular, type).get();
				suggestedViewActor = globalActor;
			}
			else
			{
				suggestedViewActor = globalActor;
			}

			globalActor->AddObject(objectId);
		}
	}
	else
	{
		return nullptr;
	}

	return suggestedViewActor;
}

void SIM::View3D::RemoveActorsOfType(std::string type, ObjectUniqueKey system)
{
	for (auto iact = _actors.begin(); iact != _actors.end();)
	{
		std::shared_ptr<ViewActor>& actor = *iact;

		if (actor->GetType() == type)
		{
			GetRenderer(actor->GetId())->RemoveActor(actor->GetActor());
			// _Renderer->BoundsModified();
			_actorsMap.erase(actor->GetActor());

			for (auto id : actor->GetObjects())
				_objecViewActorMap[actor->GetType()].erase(id);

			iact = _actors.erase(iact);
		}
		else
		{
			iact++;
		}

	}
}

void SIM::View3D::BoundModified()
{
	UpdateRotationCenter();
	_RenderWindow->GetRenderers()->InitTraversal();
	while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
	{
		auto autoCamRenderer = vtkExtRendererAutoCAM::SafeDownCast(renderer);

		if(autoCamRenderer)
			autoCamRenderer->BoundsModified();
	}
}

void SIM::View3D::RebuildViewActor(ViewActor& viewActor)
{
	auto factory = GetFactory(viewActor);

	if (!factory)
		return;

	_objecViewActorMap[viewActor.GetType()].clear();
	if (viewActor.GetActor())
	{
		_actorsMap.erase(viewActor.GetActor().Get());
		GetRenderer(viewActor.GetRenderer())->RemoveActor(viewActor.GetActor().Get());
	}
	viewActor.ClearBeforeRebuild();
	auto actor = factory->GenerateActorForViewActor(viewActor);
	SetViewActor(viewActor, actor, NullObjectKey, EMode::Regular);

	if (viewActor.GetActor())
	{
		AddActor(viewActor.GetActor(), viewActor.GetRenderer());
		BoundModified();

		viewActor.SetAddedToView(true);
	}
	else
	{
		viewActor.SetAddedToView(false);
	}

	RebuildViewActorCellBoundCache(viewActor);
	UpdateObjViewActorsMap(viewActor);
}

void SIM::View3D::OnPropertiesApplied()
{
	OnSelectionChange(true);
}

void SIM::View3D::OnSelectionContextChanged(SelectionContextChangedEvent & selContextEv)
{
	for (auto actor : _actors)
	{
		auto factory = GetFactory(*actor);

		if (factory)
		{
			factory->SelectionContextChanged(*actor, selContextEv.GetContext());
		}
	}
	render();
}

void SIM::View3D::OnObjectsVisibilityChanged(ObjectsVisibilityChangedEvent & objVisChangeEv)
{
	std::set<ViewActor*> actorsToRebuild;
	for (auto idChanged : objVisChangeEv.GetObjects())
	{
		std::unordered_set<ObjectId> objects;

		GetModel().GetStorage().GetAllChildren(idChanged, objects, true);
		for(auto id: objects)
		{
			auto objChanged = GetModel().GetObjectById(id);
			if (objChanged)
			{
				auto obj = objChanged->GetMainObject();

				auto viewActor = GetViewActor(*obj, EMode::Regular);
				if (viewActor != nullptr)
				{
					actorsToRebuild.insert(viewActor);
				}
			}
		}
	}

	for (auto vA : actorsToRebuild)
	{
		RebuildViewActor(*vA);
	}

    OnSelectionChange(true);
	
	render();
}

void SIM::View3D::OnPreselectionChange(PreselectionChangedEvent & preselEv)
{
	if (preselEv.GetOrigin() != this)
	{
		ClearPreselection();

		if (!GetModel().GetHighlights())
			return;

		_preselection = GetModel().GetPreselection();
		for (auto id : _preselection)
		{
			auto objPtr = GetModel().GetObjectById(id);

			if (objPtr) {
				std::string context = objPtr->GetContext();
				auto obj = objPtr->GetMainObject();
				ViewFactory* factory = GetFactory(*obj);

				if (!factory)
					continue;

				if (!factory->CreateNewGeometryForSelection())
				{
					ViewActor* globalActor = GetViewActor(*obj, EMode::Regular);
					if (globalActor != nullptr)
						factory->SetColor(*globalActor, id, EMode::Preselection, context);
				}
			}
		}
		_preselection = GetModel().GetPreselection();
		render();
	}

}

void SIM::View3D::OnUpdateVisibilityFilter()
{
	for (auto actor : _actors)
	{
		RebuildViewActor(*actor.get());
	}
}

void SIM::View3D::OnUpdateSelectionFilter()
{
	for (auto actor : _actors)
	{
		if (actor->GetActor())
		{
			auto filter = GetModel().GetSelectionFilter();
			if (!filter || filter->CheckViewActor(*actor.get()))
			{
				auto factory = GetFactory(*actor);
				if (factory)
					factory->SelectionContextChanged(*actor, GetModel().GetSelectionContext());
			}
			else
			{
				actor->GetActorIterator().ForEachVtkActor([this](vtkActor& actor, int index) {
					actor.SetPickable(false);
				});
			}
		}
	}
	OnSelectionChange(true);
}

void View3D::OnHighlightsChanged()
{
	if (GetModel().GetHighlights())
	{
		OnPreselectionChange(PreselectionChangedEvent(GetModel()));
		OnSelectionChange(true);
	}
	else
	{
		ClearPreselection();
		ClearSelection();
	}
}

void SIM::View3D::ClearPreselection()
{
	for (auto id : _preselection)
	{
		auto objPtr = GetModel().GetObjectById(id);

		if (objPtr) {
			auto obj = objPtr->GetMainObject();
			ViewFactory* factory = GetFactory(*obj);

			if (!factory)
				continue;

			if (!factory->CreateNewGeometryForSelection())
			{
				ViewActor* globalActor = GetViewActor(*obj, EMode::Regular);

				if (globalActor != nullptr)
				{
					if (_selection.find(id) == _selection.end())
					{
						factory->SetColor(*globalActor, id, EMode::Regular, objPtr->GetContext());
					}
					else
					{
						factory->SetColor(*globalActor, id, EMode::Selection, objPtr->GetContext());
					}
				}
			}
		}
	}

	//refresh selection if context of selection is different than object context (otherwise it may be overriden)
	for (auto selId : _selection)
	{
		auto objPtr = GetModel().GetObjectById(selId);

		if (objPtr && objPtr->GetContext() != GetModel().GetSelectionContext())
		{
			OnSelectionChange(true);
			break;
		}
	}
	_preselection.clear();
}

void SIM::View3D::HideAllExcept(std::string type)
{
	for (auto iact = _actors.begin(); iact != _actors.end();iact++)
	{
		std::shared_ptr<ViewActor>& actor = *iact;

		if (!actor->GetActor())
			continue;

		if (actor->GetType() != type)
		{
			actor->GetActor()->VisibilityOff();
		}
		else
		{
			actor->GetActor()->VisibilityOn();
		}
	}
}

void SIM::View3D::ClearDescriptions()
{
	//for (auto it = _descriptions.begin();it != _descriptions.end();++it)
		//_Renderer->RemoveActor2D(it->second);

	//_descriptions.clear();
}

SIM::ViewSettings& SIM::View3D::GetSettings()
{
	return GetModel().GetViewSettings();
}

DocModel& SIM::View3D::GetModel()
{
	return _View.GetParent().GetModel();
}

void SIM::View3D::ZoomChanged(bool force)
{
	auto renderer = GetRenderer();
	int* size = renderer->GetRenderWindow()->GetSize();
	int height = (renderer->GetActiveCamera()->GetParallelProjection()) ? size[1] : -1;
	double parScale = renderer->GetActiveCamera()->GetParallelScale();

	if (force || height != _zoomHeight || !MathUtils::IsEqual(parScale, _parScale))
	{
		_zoomHeight = height;
		_parScale = parScale;
		for (auto& viewActor : _actors)
		{
			auto& viewFactory = _factories[viewActor->GetType()];
			viewFactory->Zoomed(*viewActor, renderer->GetActiveCamera()->GetParallelScale(), height);
		}
	}
}

int* SIM::View3D::GetSize()
{
	return GetRenderer()->GetRenderWindow()->GetSize();
}


double SIM::View3D::GetRealHeight()
{
	return GetRenderer()->GetActiveCamera()->GetParallelScale();
}

vtkSmartPointer<vtkRenderer> SIM::View3D::GetRenderer(int id) const 
{ 
	if (id >= _renderersCount || id < 0)
		id = _defaultRenderer;

	return vtkRenderer::SafeDownCast(_RenderWindow->GetRenderers()->GetItemAsObject(id + _defaultRenderer));
}

vtkCamera* SIM::View3D::GetActiveCamera()
{
	return GetRenderer()->GetActiveCamera();
}

void SIM::View3D::AddActor(vtkSmartPointer<vtkProp> actor, int rendererIndex)
{
	GetRenderer(rendererIndex)->AddActor(actor);
}

void SIM::View3D::RemoveActor(vtkSmartPointer<vtkProp> actor, int rendererIndex)
{
	GetRenderer(rendererIndex)->RemoveActor(actor);
}

ViewFactory * SIM::View3D::GetFactory(std::string type)
{
	if (_factories.find(type) != _factories.end())
		return _factories[type].get();

	return nullptr;
}

void SIM::View3D::SaveScreenshot(const QString &path, const QSize &resolution, const QColor &backgroundColor)
{
	auto renderer = GetRenderer(0);
	if (!renderer)
		return;

    auto originalPass = renderer->GetPass();
    renderer->SetPass(vtkRenderPass::SafeDownCast(_RenderPass));

	double originalBackground[3], originalBackground2[3];
	renderer->GetBackground(originalBackground);
	renderer->GetBackground(originalBackground2);

	auto originalWindow = renderer->GetRenderWindow();

	// Construct offscreen window with required size and color
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();

	window->SetOffScreenRendering(true);
	window->AddRenderer(renderer);

	if (resolution.isEmpty())
		window->SetSize(originalWindow->GetSize());
	else
		window->SetSize(resolution.width(), resolution.height());

	if (backgroundColor.isValid()) {
		renderer->SetBackground(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF());
		renderer->SetBackground2(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF());
	}

	// Get image filter, from back buffer
	auto windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowToImageFilter->SetInput(window);
	windowToImageFilter->SetInputBufferTypeToRGBA();
	windowToImageFilter->SetReadFrontBuffer(false);
	windowToImageFilter->Update();

	// Set original parameters
    renderer->SetPass(originalPass);
	window->RemoveRenderer(renderer);
	renderer->SetRenderWindow(originalWindow);
	renderer->SetBackground(originalBackground);
	renderer->SetBackground2(originalBackground2);

	// Invert Y axis
	auto flipper = vtkSmartPointer<vtkImageFlip>::New();
	flipper->SetInputData(windowToImageFilter->GetOutput());
	flipper->SetFilteredAxis(1);
	flipper->Update();

	// Write output
	auto writer = vtkSmartPointer<vtkExtImageWriter>::New();
	writer->SetInputData(flipper->GetOutput());
	writer->SetFileName(path.toStdString().c_str());
	writer->Update();
}

void SIM::View3D::UpdateObjViewActorsMap(ViewActor& viewActor)
{
	for (auto id : viewActor.GetObjects())
		_objecViewActorMap[viewActor.GetType()][id] = &viewActor;
}

#if defined(OPENVR_ENABLED)
void SIM::View3D::setupRenderPassesVR(vtkRenderWindow* window)
{
	auto cameraPass = vtkSmartPointer<vtkOpenVRCameraPass>::New();
	// Now we override the default rendering path
	{

		// Basically default path, minus overlay
		auto clearPass = vtkSmartPointer<vtkOpenVRClearPass>::New();
		//auto skyPass = vtkSmartPointer<vtkExtSkyMapPass>::New();
		auto lightsPass = vtkSmartPointer<vtkLightsPass>::New();
		auto opaquePass = vtkSmartPointer<vtkOpaquePass>::New();
		auto volumePass = vtkSmartPointer<vtkVolumetricPass>::New();
		auto translucentPass = vtkSmartPointer<vtkTranslucentPass>::New();

		auto passes = vtkSmartPointer<vtkRenderPassCollection>::New();
		passes->AddItem(clearPass);
		//passes->AddItem(skyPass);
		passes->AddItem(lightsPass);
		passes->AddItem(opaquePass);
		passes->AddItem(volumePass);
		passes->AddItem(translucentPass);

		auto defaultPass = vtkSmartPointer<vtkSequencePass>::New();
		defaultPass->SetPasses(passes);

		// Load resource
		QImage qimage(":/" VTKEXTAMBIENTOCCLUSIONPASS_IMAGE_DEFAULT);

		// Convert resource to VTK
		auto image = vtkSmartPointer<vtkQImageToImageSource>::New();
		image->SetQImage(&qimage);
		image->Update();

		auto shaderSource = QtExtHelpers::loadResource(":/" VTKEXTAMBIENTOCCLUSIONPASS_SHADER_DEFAULT);
		std::string source((char*)shaderSource.data(), shaderSource.size());

		// Cheap ambient occlusion pass
		auto ambientOcclusion = vtkSmartPointer<vtkExtAmbientOcclusionPass>::New();
		ambientOcclusion->SetRings(VTKEXTAMBIENTOCCLUSIONPASS_RINGS_DEFAULT);
		ambientOcclusion->SetSamples(VTKEXTAMBIENTOCCLUSIONPASS_SAMPLES_DEFAULT);
		ambientOcclusion->SetNoise(VTKEXTAMBIENTOCCLUSIONPASS_NOISE_DEFAULT);
		ambientOcclusion->SetGradient(VTKEXTAMBIENTOCCLUSIONPASS_GRADIENT_DEFAULT);
		ambientOcclusion->SetImage(image->GetOutput());
		ambientOcclusion->SetShaderSource(source.c_str());
		ambientOcclusion->SetDelegatePass(defaultPass);

		// Set weak pointer
		_AOPass = ambientOcclusion;
		//_SkyPass = skyPass;

		// Default overlay pass
		auto overlayPass = vtkSmartPointer<vtkOverlayPass>::New();

		passes = vtkSmartPointer<vtkRenderPassCollection>::New();
		passes->AddItem(ambientOcclusion);
		passes->AddItem(overlayPass);

		auto postPass = vtkSmartPointer<vtkSequencePass>::New();
		postPass->SetPasses(passes);

		cameraPass->SetDelegatePass(postPass);
	}

	_RenderPass = cameraPass;

	_RenderWindow->GetRenderers()->InitTraversal();
	while (auto renderer = _RenderWindow->GetRenderers()->GetNextItem())
	{
		vtkOpenGLRenderer::SafeDownCast(renderer)->SetPass(cameraPass);
	}
}
#endif // defined(OPENVR_ENABLED)


#if defined(OPENVR_ENABLED)
void SIM::View3D::UpdateOpenVR()
{
	auto rwi = vtkOpenVRRenderWindowInteractor::SafeDownCast(_RenderWindowInteractor);
	auto rw = vtkOpenVRRenderWindow::SafeDownCast(_RenderWindow);
	if (rwi && rw)
	{
		rwi->DoOneEvent(rw, GetRenderer());
		_Widget->repaint();
	}
}
#endif // defined(OPENVR_ENABLED)

#if defined(OPENVR_ENABLED)
void SIM::View3D::BeginVR()
{
	if (mOpenVREnabled)
		return;

	// enable class overrides
	EnableOpenVRObjectFactories(true);

	// save screen render objects
	mSavedRenderer = GetRenderer();
	mSavedRenderWindow = _RenderWindow;
	mSavedRenderWindowInteractor = _RenderWindowInteractor;

	// create Open VR renderer
	auto renderer = vtkSmartPointer<vtkOpenVRRenderer>::New();

	// add all actors
	auto actor_collection = mSavedRenderer->GetActors();
	actor_collection->InitTraversal();
	auto actor = actor_collection->GetNextActor();
	while (actor)
	{
		renderer->AddActor(actor);
		actor = actor_collection->GetNextActor();
	}
	renderer->SetBackground(0, 0, 0);
	renderer->SetBackground2(1, 1, 1);
	renderer->SetGradientBackground(1);

	// create OpenVR render window and interactor
	_RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	_RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	_RenderWindowInteractor->SetRenderWindow(_RenderWindow);

	// add window to widget
	auto generic_window = vtkGenericOpenGLRenderWindow::SafeDownCast(_RenderWindow);
	_Widget->SetRenderWindow(generic_window);
	_Widget->GetRenderWindow()->AddRenderer(renderer);
	_Widget->setAutoBufferSwap(true);

	// Setup render passes and lighting
	setupRenderPasses(_RenderWindow);
	createLighting();

	renderer->ResetCamera();

	mOpenVREnabled = true;
}
#endif // defined(OPENVR_ENABLED)

#if defined(OPENVR_ENABLED)
void SIM::View3D::EndVR()
{
	if (!mOpenVREnabled)
		return;

	// put the old window back
	auto generic_window = vtkGenericOpenGLRenderWindow::SafeDownCast(mSavedRenderWindow);
	_Widget->SetRenderWindow(generic_window);
	_Widget->GetRenderWindow()->AddRenderer(mSavedRenderer);
	_Widget->GetRenderWindow()->SetSwapBuffers(0);
	_Widget->setAutoBufferSwap(true);

	// remove all actors
	auto actor_collection = GetRenderer()->GetActors();
	actor_collection->InitTraversal();
	auto actor = actor_collection->GetNextActor();
	while (actor)
	{
		GetRenderer()->RemoveActor(actor);
		actor = actor_collection->GetNextActor();
	}

	// swap the saved objects
	_RenderWindowInteractor = mSavedRenderWindowInteractor;
	mSavedRenderWindowInteractor = nullptr;

	_RenderWindow = mSavedRenderWindow;
	mSavedRenderWindow = nullptr;

	mSavedRenderer = nullptr;

	setupRenderPasses(_RenderWindow);

	// disable class overrides
	EnableOpenVRObjectFactories(false);

	mOpenVREnabled = false;
}
#endif // defined(OPENVR_ENABLED)


#if defined(OPENVR_ENABLED)
void SIM::View3D::EnableOpenVRObjectFactories(bool enable)
{
	// enable/disable the default classes
	vtkOverrideInformation *oi;
	vtkOverrideInformationCollection  *oic = vtkOverrideInformationCollection::New();
	vtkObjectFactory::GetOverrideInformation("vtkRenderWindow", oic);
	for (oic->InitTraversal(); oi = oic->GetNextItem(); )
	{
		oi->GetObjectFactory()->SetAllEnableFlags(!enable, "vtkRenderWindow");
	}
	vtkObjectFactory::GetOverrideInformation("vtkCamera", oic);
	for (oic->InitTraversal(); oi = oic->GetNextItem(); )
	{
		oi->GetObjectFactory()->SetAllEnableFlags(!enable, "vtkCamera");
	}
	vtkObjectFactory::GetOverrideInformation("vtkRenderWindowInteractor", oic);
	for (oic->InitTraversal(); oi = oic->GetNextItem(); )
	{
		oi->GetObjectFactory()->SetAllEnableFlags(!enable, "vtkRenderWindowInteractor");
	}
	vtkObjectFactory::GetOverrideInformation("vtkRenderer", oic);
	for (oic->InitTraversal(); oi = oic->GetNextItem(); )
	{
		oi->GetObjectFactory()->SetAllEnableFlags(!enable, "vtkRenderer");
	}

	if (enable)
	{
		mOpenVRObjectFactory = vtkRenderingOpenVRObjectFactory::New();
		vtkObjectFactory::RegisterFactory(mOpenVRObjectFactory);
	}
	else if (mOpenVRObjectFactory != nullptr)
	{
		// disable the OpenVR class overrides (they get enabled in the factory constructor)
		vtkOverrideInformation *oi;
		vtkOverrideInformationCollection  *oic = vtkOverrideInformationCollection::New();
		vtkObjectFactory::GetOverrideInformation("vtkOpenVRRenderWindow", oic);
		for (oic->InitTraversal(); oi = oic->GetNextItem(); )
		{
			oi->GetObjectFactory()->SetAllEnableFlags(false, "vtkOpenVRRenderWindow");
		}
		vtkObjectFactory::GetOverrideInformation("vtkOpenVRCamera", oic);
		for (oic->InitTraversal(); oi = oic->GetNextItem(); )
		{
			oi->GetObjectFactory()->SetAllEnableFlags(false, "vtkOpenVRCamera");
		}
		vtkObjectFactory::GetOverrideInformation("vtkOpenVRRenderWindowInteractor", oic);
		for (oic->InitTraversal(); oi = oic->GetNextItem(); )
		{
			oi->GetObjectFactory()->SetAllEnableFlags(false, "vtkOpenVRRenderWindowInteractor");
		}
		vtkObjectFactory::GetOverrideInformation("vtkOpenVRRenderer", oic);
		for (oic->InitTraversal(); oi = oic->GetNextItem(); )
		{
			oi->GetObjectFactory()->SetAllEnableFlags(false, "vtkOpenVRRenderer");
		}

		// unregister and delete the object factory
		vtkObjectFactory::UnRegisterFactory(mOpenVRObjectFactory);
		mOpenVRObjectFactory->Delete();
		mOpenVRObjectFactory = nullptr;
	}
}


#endif // defined(OPENVR_ENABLED)

