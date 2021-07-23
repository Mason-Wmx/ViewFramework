#pragma once

class vtkExtAutoCAM;
class vtkExtAmbientOcclusionPass;
class vtkExtSkyMapPass;
#include "stdafx.h"
#include "export.h"

#include "subview.h"
#include "QtWidgets"
#include "QGraphicsView"

#include "QVTKWidget2.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkRenderer.h"

#include "vtkExtCubeMap.h"
#include <atomic>

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>


#include "QtWidgets"
#include "QGraphicsView"
#include "QResizeEvent"

#include "QVTKWidget2.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkRenderer.h"
#include "QVTKInteractorAdapter.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkAbstractWidget.h"
#include "vtkCommand.h"
#include "vtkScalarBarActor.h"

#include "vtkExtCubeMap.h"
#include "vtkExtRendererAutoCAM.h"

#include "vtkOrientationMarkerWidget.h"

#include "MVC.h"
#include "ViewActor.h"
#include "../NfdcDataModel/Model.h"

#include "stdafx.h"
#include "ModelEvents.h"
#include "ViewData.h"

#include <headers/AppInterfaces/IViewFactoryRegistry.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#if defined(OPENVR_ENABLED)
class vtkRenderingOpenVRObjectFactory;
#endif

class vtkAreaPicker;
class vtkExtractSelectedFrustum;
class vtkProp3DCollection;
class vtkPlanes;
class vtkPoints;
namespace SIM
{
	class DocView;
	class ViewFactory;
    class ModulesLoadedEvent;
	class vtkExtSimInteractorStyleAutoCAM;
	
	class NFDCAPPCORE_EXPORT View3D :
		public QGraphicsView,
        public SubView,
        public IViewFactoryRegistry
	{
		Q_OBJECT

    public:
        typedef ObjectUniqueKey ActorsMapKey;
        typedef std::vector<std::shared_ptr<ViewActor>> ActorsMapValue;
        typedef std::map<ActorsMapKey, ActorsMapValue> ActorsMap;

	public: //statics
		// Creating 3D view, require initializing OpenGL which may fail (2017-01-19 we have confirmed cases for failing). For that reason we have special methods that create View3D (or return nullptr) when initialization (OpenGL) fail.
		static std::unique_ptr<View3D> CreateView3D(DocView& docView);
		static std::unique_ptr<View3D> CreateView3D(DocView& docView, const QList<QGraphicsProxyWidget*> & additionalWidgets);

		// Set "showWarnings" to true when you want to notify user about inability of OpenGL initialization
		// "parent" is required only if "showWarnings" set to true. 
		// "parent" is needed to show QMessageBox in the center of active window for this application (avoid problems with big and multiple monitors). 
		// Set "initDummyWindow" to true to initialize dummy window that will be destroyed. Set "initDummyWindow" to false to initialize current window. 
		// true is returned when OpenGL was correctly initialized, false otherwise.
		static bool InitOpenGL(bool showWarnings, QWidget *parent, bool initDummyWindow /*= false*/); 
		static bool InitOpenGL(); //return true when OpenGL was correctly initialized. false otherwise


	protected:
		View3D(DocView& docView);

	public:
		~View3D(void);

        // implement IFactoryRegistry
        virtual View3D & GetView3D() override;
        virtual void RegisterViewFactory(std::shared_ptr<ViewFactory> factory) override;

		// observer
		virtual void Notify(Event& ev) override;

    //helpers
    void setupCSMarker();

		//dry
		vtkRenderWindow* getRenderWindow () 
		{
			if ( !_Widget . get () )			
				return NULL ;

			return _Widget -> GetRenderWindow () ;
		}
		void ambientOcclusion ( bool state ) ;
		bool ambientOcclusion () ;
		void updateAmbientOcclusion ( bool repaint = false ) ;
		void interactorEvent ( QEvent* event ) ;		
		vtkExtAutoCAM* autoCAM () ;
		void applyRenderProps ( vtkActor& actor, RenderProps& props ) ;
		
		//DC
		DocView& GetView(){ return _View; }
		SIM::ViewSettings& GetSettings();
		DocModel& GetModel();
		int* GetSize();
		double GetRealHeight();		
		vtkSmartPointer<vtkRenderer> GetRenderer(int id = 0) const;
		vtkCamera* GetActiveCamera();		
		const std::vector<std::shared_ptr<ViewActor>>& GetActors() { return _actors; }
		void AddActor(vtkSmartPointer<vtkProp> actor, int rendererIndex = 0);
		void RemoveActor(vtkSmartPointer<vtkProp> actor, int rendererIndex = 0);
		ViewFactory* GetFactory(std::string type);
		void SaveScreenshot(const QString &path, const QSize &resolution = QSize(), const QColor &backgroundColor = QColor());

        void ChangeLight();

		QSignalMapper _signalMapper;
	#if defined(OPENVR_ENABLED)
		void BeginVR();
		void EndVR();
		bool InOpenVRMode() { return mOpenVREnabled; }
		void UpdateOpenVR();
	#endif
signals:
		void closing () ;
		void resized () ;
		void commandNotify () ;

	private slots:
		void renderPropsValue ( double ) ;
		void renderPropsValue ( int ) ;
		void renderPropsColor () ;
		void renderPropsColorEdge () ;

	protected:
		void drawBackground(QPainter* p, const QRectF& vtkNotUsed(r));
		void drawLogo(QPainter* p);

		// Window events
		void closeEvent(QCloseEvent *event);
		void resizeEvent(QResizeEvent *event);	
		void moveEvent(QMoveEvent *event);
		void showEvent(QShowEvent *event);

		// Keyboard events
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);

		// Mouse events
		void mouseMoveEvent(QMouseEvent *event);

#ifndef QT_NO_WHEELEVENT
		void wheelEvent(QWheelEvent *event);
		void ZoomChanged(bool force = false);
#endif

		void enterEvent(QEvent* event);
		void leaveEvent(QEvent* event);

		void focusInEvent(QFocusEvent* event);
		void focusOutEvent(QFocusEvent* event);

		void dragEnterEvent(QDragEnterEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);
		void dragLeaveEvent(QDragLeaveEvent* event);

		void mouseDoubleClickEvent(QMouseEvent *event);

	private:
		void InitializeFactories();
		void AddObject(Object& element);
		ViewFactory* GetFactory(Object& element, std::string type = "");
		ViewFactory* GetFactory(ViewActor& viewActor);
		ViewActor* GetViewActor(Object& element, EMode mode, std::string type = "");
		ViewActor* GetViewActor(ObjectId objId, std::string type);
		void SetViewActor(ViewActor& viewActor, vtkSmartPointer<vtkProp> &actor, ObjectUniqueKey obj, EMode mode);
		std::shared_ptr<ViewActor> CreateViewActor(Object& element,EMode mode, std::string type = "");
		std::vector<std::shared_ptr<ViewActor>>& GetActors(EMode mode);
		void ZoomAll();
		void SetCamera();

		void OnObjectsUpdated(ObjectsUpdatedEvent & elUpdatedEv);
		void OnObjectsAdded(ObjectsAddedEvent & elAddedEv);
		void OnObjectsRemoved(ObjectsRemovedEvent & elRemovedEv);
		void OnModelReloaded();
		void OnSelectionChange(bool force=false);
		void OnPreselectionChange(PreselectionChangedEvent & preselEv);
		void OnUpdateVisibilityFilter();
		void OnUpdateSelectionFilter();
        void OnHighlightsChanged();
		void OnPropertiesApplied();
		void OnSelectionContextChanged(SelectionContextChangedEvent& selContextEv);
		void OnObjectsVisibilityChanged(ObjectsVisibilityChangedEvent& objVisChangeEv);

		void Clear(bool clearSelection);
		void HideAll();
		void ClearSelection();
		void ClearPreselection();
		ObjectId PickObject(EMode mode, bool addToSelection = false);
		void PickAreaCallbackFunction(EMode mode);
		void SetCursor(bool forceArrow = false);
		bool IsViewActorValid(ViewActor* viewActor, Object& element, std::string type = "");
		void SaveCameraSettings();
		void LoadCameraSettings();
		void UpdateObjViewActorsMap(ViewActor& viewActor);
		ViewActor* GetOrCreateViewActorForObject(ObjectId objectId, ViewActor* suggestedViewActor, std::string type = "");
		void AddViewActorToCollection(std::shared_ptr<ViewActor>& actor,EMode mode);
		void RemoveActorsOfType(std::string type,ObjectUniqueKey system);
		void BoundModified();
		void RebuildViewActor(ViewActor& viewActor);
		void ClearDescriptions();
		void HideAllExcept(std::string type);
		void AnalyzeAreaSelection(EMode mode);
		void StopPreselectionThread();
		void RebuildViewActorCellBoundCache(ViewActor& actor);
		void RebuildViewActorsCellBoundCache();
		void UpdateRotationCenter();

		//Dry
		void setupRenderPasses ( vtkRenderWindow* window ) ;
		void createShader () ;
		void createLighting () ;
		void setCubeMap ( const std::string& name ) ;
		void render () ;
		void resetClippingRange () ;
		void updateClippingRange () ;
		int popupMenu ( int id ) ;
		void showCursor ( bool visible ) ;
		void commandCallBack () ;
		bool ribbonCallBack();
		
		//members
		std::atomic<bool> _ResetClippingRange ;
		QGLContext* _Ctx ;
		std::auto_ptr<QVTKWidget2> _Widget;
		vtkSmartPointer<vtkRenderWindowInteractor> _RenderWindowInteractor ;
    vtkSmartPointer<vtkOrientationMarkerWidget> _orientationWidget;
		vtkExtSimInteractorStyleAutoCAM* _RenderInteractorStyleAutoCAM;
		std::auto_ptr<QVTKInteractorAdapter> _InteractorAdapter;
		vtkSmartPointer<vtkRenderWindow> _RenderWindow;
		std::string _CubeMapName ;
		vtkSmartPointer<vtkExtCubeMap> _CubeMap ;
		vtkSmartPointer<vtkObject> _RenderPass ;
		vtkExtAmbientOcclusionPass* _AOPass ;
		vtkExtSkyMapPass* _SkyPass;
		std::function<void ()> _RenderPropUpdate ;
		std::function<void (int)> _RenderPropColor ;
		std::unordered_set<ObjectId> _selection;
		std::unordered_set<ObjectId> _preselection;
		std::map<ObjectUniqueKey,vtkSmartPointer<vtkActor2D>> _descriptions;
		std::unordered_set<ObjectUniqueKey> _dedicatedActorsForElements;	
		std::map<int,double> _opacityCache;
		unsigned _Id ;
		QImage _Logo ;
		DocView& _View;		
		std::map<std::string,std::shared_ptr<ViewFactory>> _factories;
		std::vector<std::shared_ptr<ViewActor>> _actors;
		std::vector<std::shared_ptr<ViewActor>> _selectedActors;
		std::map<vtkProp*,ViewActor*> _actorsMap;
		std::map<std::string,std::map<ObjectUniqueKey,ViewActor*>> _objecViewActorMap;
		int _zoomHeight;
		double _parScale;
		bool _firstGeometryLoaded = false;
		int _defaultRenderer = 0;
		int _renderersCount = 2;

		int _areaPreselectionThreadIteration = 0;
        std::thread _areaPreselectionThread;
        bool _areaPreselectionThreadIsRunning = false;
		volatile bool _stopAreaPreselection = false;
		int _areaSelectionMin[2], _areaSelectionMax[2];
		bool _areaSelectionRespectBoundCells;
		std::mutex _preselectionMutex;
		std::vector<vtkSmartPointer<vtkAreaPicker>> _areaPickers;	
		bool _dblClick;
#if defined(OPENVR_ENABLED)
		vtkSmartPointer<vtkRenderWindow> mSavedRenderWindow;
		vtkSmartPointer<vtkRenderer> mSavedRenderer;
		vtkSmartPointer<vtkRenderWindowInteractor> mSavedRenderWindowInteractor;
		vtkRenderingOpenVRObjectFactory *mOpenVRObjectFactory;
		bool mOpenVREnabled;
		void EnableOpenVRObjectFactories(bool enable);
		void setupRenderPassesVR(vtkRenderWindow* window);
#endif // defined(OPENVR_ENABLED)
	};

}


