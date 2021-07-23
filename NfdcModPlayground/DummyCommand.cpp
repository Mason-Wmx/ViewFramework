
#include "stdafx.h"

#include "DummyCommand.h"
#include "DummyRequest.h"
#include "DummyObject.h"
#include "DummyViewFactory.h"
#include "DemoDialog.h"

#include "../NfdcAppCore/mainwindow.h"
#include "../NfdcDataModel/Model.h"
#include "../NfdcAppCore/DocModel.h"
#include "../NfdcAppCore/ViewEvents.h"
#include "../NfdcAppCore/ViewFactory.h"
#include "../NfdcAppCore/View3D.h"
#include "../NfdcAppCore/DocUnits.h"
#include "../NfdcAppCore/InfoCenterView.h"
#include "../NfdcCommonUI/UnitValueCommandInput.h"
#include "../NfdcAppCore/Request.h"

#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include "boost\filesystem\path.hpp" // for DummyCommandReadJSONMaterial
#include "../NfdcDataModel/MaterialsDataProvider.h"
#include "../NfdcModMaterials/MaterialsResourceLocator.h"
#include "../NfdcModMaterials/MaterialsMDSSynchronizer.h"
#include "../NfdcModMaterials/MaterialsProviderMDSLoader.h"
#include "../NfdcModMaterials/MainMaterialsDialog.h"

#include "../NfdcAppCore/ForgeMngr.h"
#include "../NfdcAppCore/DebugModeManager.h"
#include "../NfdcAppCore/DebugMode.h"


using namespace SIM;

vtkSmartPointer<vtkActor> CreateArrow(double point[3], double vector[3], double arrawLength, double width);

DummyCommand::DummyCommand(Application& app):ContinuousCommand(app)
{
}


DummyCommand::~DummyCommand(void)
{
}

bool SIM::DummyCommand::Execute()
{
  OnStart();

  double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 20.0;
  auto pos = (this->GetDocument()->GetModel().GetGlobalBounds().GetMin() + this->GetDocument()->GetModel().GetGlobalBounds().GetMin()) / 2.;
  GetApplication().GetActiveDocument()->GetController().ExecuteRequest(DummyRequest::Create(*GetApplication().GetActiveDocument().get(), pos.x, pos.y, pos.z, h, h));

  OnEnd();
  return true;
}

std::string SIM::DummyCommand::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommand::Name = "Dummy";


//*********************************

DummyCommandEdit::DummyCommandEdit(Application& app) :ContinuousCommand(app)
{
}


DummyCommandEdit::~DummyCommandEdit(void)
{
}

bool SIM::DummyCommandEdit::Execute()
{
  OnStart();
  auto selected = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();
  
  if (!selected.empty())
  {
    GetApplication().GetActiveDocument()->GetController().ExecuteRequest(std::shared_ptr<DummyEditRequest>(new DummyEditRequest(*GetApplication().GetActiveDocument().get(), *selected.begin(), 1, 1, 1, 2, 2)));
  }
  OnEnd();
  return true;
}



std::string SIM::DummyCommandEdit::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandEdit::Name = "DummyCommandEdit";


//*********************************

DummyCommandSurfaceMode::DummyCommandSurfaceMode(Application& app) :ContinuousCommand(app)
{
}


DummyCommandSurfaceMode::~DummyCommandSurfaceMode(void)
{
}

bool SIM::DummyCommandSurfaceMode::Execute()
{
  OnStart();
  GetApplication().GetActiveDocument()->GetModel().SetSelectionContextFilter(SelectionContext::Surface);
  GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Surface);

  QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
  SetDialog(dlg);
  dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
  dlg->setWindowTitle("Surface mode");

  QVBoxLayout* vLayout = new QVBoxLayout;
  QGridLayout *gridLayout = new QGridLayout;

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {

        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContextFilter();
        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
        this->OnEnd();
    });

  vLayout->addWidget(buttonBox);

  dlg->setLayout(vLayout);
  dlg->show();
  dlg->raise();

  return true;
}

std::string SIM::DummyCommandSurfaceMode::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandSurfaceMode::Name = "DummyCommandSurfaceMode";


//*********************************

DummyCommandSelectionFilter::DummyCommandSelectionFilter(Application& app) :ContinuousCommand(app)
{
}


DummyCommandSelectionFilter::~DummyCommandSelectionFilter(void)
{
}

bool SIM::DummyCommandSelectionFilter::Execute()
{
  OnStart();
  GetApplication().GetActiveDocument()->GetModel().SetSelectionFilter(std::make_shared<ViewActorTypeFilter>(typeid(DummyObject).name()));

  QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
  SetDialog(dlg);
  dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
  dlg->setWindowTitle("Select dummy object");

  QVBoxLayout* vLayout = new QVBoxLayout;
  QGridLayout *gridLayout = new QGridLayout;

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {
        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionFilter();
        this->OnEnd();
    });

  vLayout->addWidget(buttonBox);

  dlg->setLayout(vLayout);
  dlg->show();
  dlg->raise();

  return true;
}

std::string SIM::DummyCommandSelectionFilter::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandSelectionFilter::Name = "DummyCommandSelectionFilter";


//*********************************
DummyCommandBlockEmptySelection::DummyCommandBlockEmptySelection(Application& app) :ContinuousCommand(app)
{
}

SIM::DummyCommandBlockEmptySelection::~DummyCommandBlockEmptySelection(void)
{
}

bool SIM::DummyCommandBlockEmptySelection::Execute()
{
    OnStart();
    GetApplication().GetActiveDocument()->GetModel().SetAllowEmptySelection(false);

    QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
    SetDialog(dlg);
    dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    dlg->setWindowTitle("Block selection");

    QVBoxLayout* vLayout = new QVBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {
        this->GetApplication().GetActiveDocument()->GetModel().SetAllowEmptySelection(true);
        this->OnEnd();
    });

    vLayout->addWidget(buttonBox);

    dlg->setLayout(vLayout);
    dlg->show();
    dlg->raise();

    return true;
}


//*********************************
DummyCommandFreezeSelection::DummyCommandFreezeSelection(Application& app) :ContinuousCommand(app)
{
}

DummyCommandFreezeSelection::~DummyCommandFreezeSelection(void)
{
}

bool SIM::DummyCommandFreezeSelection::Execute()
{
    OnStart();
    GetApplication().GetActiveDocument()->GetModel().SetFreezeSelection(true);

    QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
    SetDialog(dlg);
    dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    dlg->setWindowTitle("Freeze selection");

    QVBoxLayout* vLayout = new QVBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {
        this->GetApplication().GetActiveDocument()->GetModel().SetFreezeSelection(false);
        this->OnEnd();
    });

    vLayout->addWidget(buttonBox);

    dlg->setLayout(vLayout);
    dlg->show();
    dlg->raise();

    return true;
}

//*********************************



DummyCommandCreateOnSurface::DummyCommandCreateOnSurface(Application& app) :ContinuousCommand(app)
{
}


DummyCommandCreateOnSurface::~DummyCommandCreateOnSurface(void)
{
}

bool SIM::DummyCommandCreateOnSurface::Execute()
{
  OnStart();
  GetApplication().GetActiveDocument()->GetModel().SetSelectionFilter(std::make_shared<ViewActorTypeFilter>(typeid(Geometry).name()));
  GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Surface);
  GetApplication().GetActiveDocument()->GetModel().SetHighlights(false);
  GetApplication().GetActiveDocument()->GetView().GetView3D().RegisterObserver(*this);

  QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
  SetDialog(dlg);
  dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
  dlg->setWindowTitle("Create obj on surface");

  QVBoxLayout* vLayout = new QVBoxLayout;
  QGridLayout *gridLayout = new QGridLayout;

  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

  double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
  DummyObject obj(0, 0, 0, h, h);
  DummyViewFactory* factory = dynamic_cast<DummyViewFactory*>(GetApplication().GetActiveDocument()->GetView().GetView3D().GetFactory(typeid(DummyObject).name()));
  _actor = vtkActor::SafeDownCast(factory->GenerateActorForObject(obj));
  _actor->SetPickable(false);
  GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(_actor);

    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {
        this->GetApplication().GetActiveDocument()->GetView().GetView3D().UnregisterObserver(*this);
        this->GetApplication().GetActiveDocument()->GetView().GetView3D().RemoveActor(_actor);
        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionFilter();
        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
        GetApplication().GetActiveDocument()->GetModel().SetHighlights(true);
        this->OnEnd();
    });

  vLayout->addWidget(buttonBox);

  dlg->setLayout(vLayout);
  dlg->show();
  dlg->raise();

  return true;
}

std::string SIM::DummyCommandCreateOnSurface::GetUniqueName()
{
  return Name;
}

void SIM::DummyCommandCreateOnSurface::Notify(Event & ev)
{
  MouseClickEvent* mce = dynamic_cast<MouseClickEvent*>(&ev);
  if (mce) {
    if (mce->GetData().object > -1) {
		double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
      GetApplication().GetActiveDocument()->GetController().ExecuteRequest(
        DummyRequest::Create(*GetApplication().GetActiveDocument().get(), mce->GetData().point.x, mce->GetData().point.y, mce->GetData().point.z, h, h, mce->GetData().object));
    }
  }

  MouseMoveEvent* mme = dynamic_cast<MouseMoveEvent*>(&ev);

  if (mme) {
    
    if (mme->GetData().object > -1) {
      _actor->VisibilityOn();
      _actor->SetPosition(mme->GetData().point.x, mme->GetData().point.y, mme->GetData().point.z);
      _actor->Modified();
    }
    else
    {
      _actor->VisibilityOff();
    }
  }

}

const std::string SIM::DummyCommandCreateOnSurface::Name = "DummyCommandCreateOnSurface";












DummyCommandCreateOnSurfaceTop::DummyCommandCreateOnSurfaceTop(Application& app) :ContinuousCommand(app)
{
}


DummyCommandCreateOnSurfaceTop::~DummyCommandCreateOnSurfaceTop(void)
{
}

bool SIM::DummyCommandCreateOnSurfaceTop::Execute()
{
	OnStart();
	GetApplication().GetActiveDocument()->GetModel().SetSelectionFilter(std::make_shared<ViewActorTypeFilter>(typeid(Geometry).name()));
	GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Surface);
	GetApplication().GetActiveDocument()->GetModel().SetHighlights(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().RegisterObserver(*this);

	QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
	SetDialog(dlg);
	dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
	dlg->setWindowTitle("Create obj on surface");

	QVBoxLayout* vLayout = new QVBoxLayout;
	QGridLayout *gridLayout = new QGridLayout;

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

	double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
	DummyObject obj(0, 0, 0, h, h);
	obj.renderer = 1;
	DummyViewFactory* factory = dynamic_cast<DummyViewFactory*>(GetApplication().GetActiveDocument()->GetView().GetView3D().GetFactory(typeid(DummyObject).name()));
	_actor = vtkActor::SafeDownCast(factory->GenerateActorForObject(obj));
	_actor->SetPickable(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(_actor, 1);

	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	connect(dlg, &QDialog::rejected, [this]() {
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().UnregisterObserver(*this);
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().RemoveActor(_actor, 1);
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionFilter();
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
		GetApplication().GetActiveDocument()->GetModel().SetHighlights(true);
		this->OnEnd();
	});

	vLayout->addWidget(buttonBox);

	dlg->setLayout(vLayout);
	dlg->show();
	dlg->raise();

	return true;
}

std::string SIM::DummyCommandCreateOnSurfaceTop::GetUniqueName()
{
	return Name;
}

void SIM::DummyCommandCreateOnSurfaceTop::Notify(Event & ev)
{
	MouseClickEvent* mce = dynamic_cast<MouseClickEvent*>(&ev);
	if (mce) {
		if (mce->GetData().object > -1) {
			double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
			std::shared_ptr<DummyObject> obj = std::make_shared<DummyObject>(mce->GetData().point.x, mce->GetData().point.y, mce->GetData().point.z, h, h, mce->GetData().object);
			obj->renderer = 1;
			std::unordered_set<std::shared_ptr<Object>> objs;
			objs.insert(obj);
		
			GetApplication().GetActiveDocument()->GetController().ExecuteRequest(std::shared_ptr<AddRequest>( new AddRequest(*this->GetDocument(),objs)));
		}
	}

	MouseMoveEvent* mme = dynamic_cast<MouseMoveEvent*>(&ev);

	if (mme) {

		if (mme->GetData().object > -1) {
			_actor->VisibilityOn();
			_actor->SetPosition(mme->GetData().point.x, mme->GetData().point.y, mme->GetData().point.z);
			_actor->Modified();
		}
		else
		{
			_actor->VisibilityOff();
		}
	}

}

const std::string SIM::DummyCommandCreateOnSurfaceTop::Name = "DummyCommandCreateOnSurfaceTop";

// Create dummy objects with group object

DummyCommandCreateGroupOnSurface::DummyCommandCreateGroupOnSurface(Application& app)
	:DummyCommandCreateOnSurface(app)
{ }

DummyCommandCreateGroupOnSurface::~DummyCommandCreateGroupOnSurface(void)
{ }

std::string SIM::DummyCommandCreateGroupOnSurface::GetUniqueName()
{
	return Name;
}

bool SIM::DummyCommandCreateGroupOnSurface::Execute()
{
	OnStart();
	GetApplication().GetActiveDocument()->GetModel().SetSelectionFilter(std::make_shared<ViewActorTypeFilter>(typeid(Geometry).name()));
	GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Surface);
	GetApplication().GetActiveDocument()->GetModel().SetHighlights(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().RegisterObserver(*this);

	QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
	SetDialog(dlg);
	dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
	dlg->setWindowTitle("Create grouped objects on surface");

	QVBoxLayout* vLayout = new QVBoxLayout;
	QGridLayout *gridLayout = new QGridLayout;

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

	double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
	DummyGroupableObject obj(0, 0, 0, h, h);
	DummyViewFactory* factory = dynamic_cast<DummyViewFactory*>(GetApplication().GetActiveDocument()->GetView().GetView3D().GetFactory(typeid(DummyGroupableObject).name()));
	_actor = vtkActor::SafeDownCast(factory->GenerateActorForObject(obj));
	_actor->SetPickable(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(_actor);

	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	connect(dlg, &QDialog::rejected, [this]() {
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().UnregisterObserver(*this);
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().RemoveActor(_actor);
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionFilter();
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
		GetApplication().GetActiveDocument()->GetModel().SetHighlights(true);
		this->OnEnd();
	});

	vLayout->addWidget(buttonBox);

	dlg->setLayout(vLayout);
	dlg->show();
	dlg->raise();

	return true;
}

void SIM::DummyCommandCreateGroupOnSurface::Notify(Event & ev)
{
	MouseClickEvent* mce = dynamic_cast<MouseClickEvent*>(&ev);
	if (mce) {
		if (mce->GetData().object > -1) {
			double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
			GetApplication().GetActiveDocument()->GetController().ExecuteRequest(
				DummyGroupableRequest::Create(*GetApplication().GetActiveDocument().get(), mce->GetData().point.x, mce->GetData().point.y, mce->GetData().point.z, h, h, mce->GetData().object));
		}
	}

	MouseMoveEvent* mme = dynamic_cast<MouseMoveEvent*>(&ev);

	if (mme) {

		if (mme->GetData().object > -1) {
			_actor->VisibilityOn();
			_actor->SetPosition(mme->GetData().point.x, mme->GetData().point.y, mme->GetData().point.z);
			_actor->Modified();
		}
		else
		{
			_actor->VisibilityOff();
		}
	}

}

const std::string SIM::DummyCommandCreateGroupOnSurface::Name = "DummyCommandCreateGroupOnSurface";




// Dialog to testing controls with units

DummyCommandControlsUnits::DummyCommandControlsUnits(Application& app) :ContinuousCommand(app)
{
}


DummyCommandControlsUnits::~DummyCommandControlsUnits(void)
{
}

bool SIM::DummyCommandControlsUnits::Execute()
{
  if (GetApplication().GetActiveDocument() == NULL)
    return true;

  OnStart();

  ControlsUnitsDialog* dlg = new ControlsUnitsDialog(GetApplication(), &GetApplication().GetMainWindow());
  SetDialog(dlg);
  dlg->setWindowTitle("Controls with units");

  connect(dlg->buttonBox, SIGNAL(rejected()), dlg, SLOT(close()));

  connect(dlg->buttonBox, &QDialogButtonBox::accepted, [this, dlg]() {
    dlg->close();
    this->OnEnd();
  });

  dlg->show();
  dlg->raise();

  return true;
}

std::string SIM::DummyCommandControlsUnits::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandControlsUnits::Name = "DummyCommandControlsUnits";


UnitControls::UnitControls(Application& app, QDialog* parent, QGridLayout* layout, QString name, double value, EUnitCategory category, int idx)
{
  auto title = new QLabel;
  auto oldValue = new QLineEdit;

  auto edit = UnitValueCommandInput::make(app, category, parent);
  edit->EnableNotify();
  auto newValue = new QLabel;

  title->setText(name);
  oldValue->setText(QString::number(value));
  edit->value(value);
  newValue->setText(QString::number(value));

  connect(oldValue, &QLineEdit::editingFinished, [edit, oldValue, newValue]() {
    QLocale ql = QLocale::system();
    double value = ql.toDouble(oldValue->text());
    edit->value(value);
    newValue->setText(ql.toString(value, 'g', 12));
  });

  connect(edit, &UnitValueCommandInput::validateData, [edit, newValue]() {
      QLocale ql = QLocale::system();
      double value = edit->value();
      newValue->setText(ql.toString(value, 'g', 12));
  });


  layout->addWidget(title, idx, 0);
  layout->addWidget(oldValue, idx, 1);
  layout->addWidget(edit, idx, 2);
  layout->addWidget(newValue, idx, 3);

  oldValue->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

ControlsUnitsDialog::ControlsUnitsDialog(Application& app, QWidget *parent) : QDialog(parent), buttonBox(nullptr)
{
  if (!app.GetActiveDocument())
    return;

  DocUnits& units = app.GetActiveDocument()->GetModel().GetUnits();
  //EUnitCategory category;

  setObjectName(QStringLiteral("Dialog testing units"));
  setEnabled(true);
  resize(400, 100);
  QVBoxLayout* vLayout = new QVBoxLayout;
  QGridLayout *gridLayout = new QGridLayout;

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);


  int idx = 1;

  auto header1 = new QLabel("SI Input Value");
  auto header2 = new QLabel("Formated Value");
  auto header3 = new QLabel("SI Output Value");
  gridLayout->addWidget(header1, idx, 1);
  gridLayout->addWidget(header2, idx, 2);
  gridLayout->addWidget(header3, idx, 3);

  UnitControls* fractional1 = new UnitControls(app, this, gridLayout, "Length", 2.0, EUnitCategory::UNIT_LENGTH, ++idx);

  vLayout->addLayout(gridLayout);
  vLayout->addWidget(buttonBox);
  setLayout(vLayout);
}

ControlsUnitsDialog::~ControlsUnitsDialog()
{

}


// Dialog to testing controls with units

DummyCommandMDSMaterial::DummyCommandMDSMaterial(Application& app) :DocCommand(app)
{
}


DummyCommandMDSMaterial::~DummyCommandMDSMaterial(void)
{
}



bool SIM::DummyCommandMDSMaterial::Execute()
{
    {
       std::shared_ptr<const MaterialsResourceLocator> locator(MaterialsResourceLocator::CreateLocalCacheLocator());
       auto jsonLocation = locator->GetJsonDataFileLocation();
       MaterialsProviderMDSLoader mdsLoader(*locator);
       MaterialsDataProvider provider;
       provider.LoadRecords(&mdsLoader);
       std::shared_ptr<MaterialsDataUserStatus> dlgMaterialsDataUserStatus = std::make_shared<MaterialsDataUserStatus>();
       MainMaterialsDialog* dlg = new MainMaterialsDialog(GetApplication(), provider, *dlgMaterialsDataUserStatus, locator, &GetApplication().GetMainWindow());
       dlg->exec();
   }

  return false;
}

std::string SIM::DummyCommandMDSMaterial::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandMDSMaterial::Name = "DummyCommandMDSMaterial";



// -----------------------------------------------------------------------------

DummyCommandVisibilityFilter::DummyCommandVisibilityFilter(Application& app) :ContinuousCommand(app)
{
}


DummyCommandVisibilityFilter::~DummyCommandVisibilityFilter(void)
{
}

bool SIM::DummyCommandVisibilityFilter::Execute()
{
  OnStart();
  GetApplication().GetActiveDocument()->GetModel().SetVisibilityFilter(std::make_shared<ViewActorTypeFilter>(typeid(DummyObject).name()));

  QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
  SetDialog(dlg);
  dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
  dlg->setWindowTitle("Visibility filter");

  QVBoxLayout* vLayout = new QVBoxLayout;
  QGridLayout *gridLayout = new QGridLayout;

  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

  connect(dlg, &QDialog::rejected, [this]() {
    this->GetApplication().GetActiveDocument()->GetModel().ResetVisibilityFilter();
    this->OnEnd();
  });

  vLayout->addWidget(buttonBox);

  dlg->setLayout(vLayout);
  dlg->show();
  dlg->raise();

  return true;
}

std::string SIM::DummyCommandVisibilityFilter::GetUniqueName()
{
  return Name;
}

const std::string SIM::DummyCommandVisibilityFilter::Name = "DummyCommandVisibilityFilter";


DummyCommandBaseDialgStyle::DummyCommandBaseDialgStyle(Application& app) :ContinuousCommand(app)
{
}


DummyCommandBaseDialgStyle::~DummyCommandBaseDialgStyle(void)
{
}

bool SIM::DummyCommandBaseDialgStyle::Execute()
{
	DemoDialog DemoDlg(&GetApplication().GetMainWindow());
	DemoDlg.exec();
	return true;
}



std::string SIM::DummyCommandBaseDialgStyle::GetUniqueName()
{
	return Name;
}

const std::string SIM::DummyCommandBaseDialgStyle::Name = "Dialog Style";

SIM::DummyCommandGenerateRandomEdges::DummyCommandGenerateRandomEdges(Application & app):DocCommand(app)
{
}

SIM::DummyCommandGenerateRandomEdges::~DummyCommandGenerateRandomEdges(void)
{

}

bool SIM::DummyCommandGenerateRandomEdges::Execute()
{
	auto& model = GetApplication().GetActiveDocument()->GetModel();

	auto geoms = model.GetStorage().GetObjectsByTypeName(typeid(Geometry).name());

	for (auto item : geoms)
	{
		auto geom = dynamic_cast<Geometry*>(item.second.get());
        for (auto body : geom->GetBodies())
        {
            for (int i = 0; i < body.second->GetRawGeometry()->GetNumberOfPoints() - 2; i += 2)
            {
                std::vector<ObjectId> nodes;
                nodes.push_back(i);
                nodes.push_back(i + 1);

                auto edge = std::make_shared<Edge>(*body.second, nodes);
                body.second->AddEdge(edge, model.GetStorage());
            }
        }

		model.NotifyObjectUpdated(item.second, "edges");
	}
	return false;
}

std::string SIM::DummyCommandGenerateRandomEdges::GetUniqueName()
{
	return DummyCommandGenerateRandomEdges::Name;
}

const std::string SIM::DummyCommandGenerateRandomEdges::Name("DummyCommandGenerateRandomEdges");




DummyCommandEdgeMode::DummyCommandEdgeMode(Application& app) :ContinuousCommand(app)
{
}


DummyCommandEdgeMode::~DummyCommandEdgeMode(void)
{
}

bool SIM::DummyCommandEdgeMode::Execute()
{
    OnStart();
    GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Edge);

    QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
	SetDialog(dlg);
    dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    dlg->setWindowTitle("Edge mode");

    QVBoxLayout* vLayout = new QVBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    connect(dlg, &QDialog::rejected, [this]() {
        this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
        this->OnEnd();
    });

    vLayout->addWidget(buttonBox);

    dlg->setLayout(vLayout);
    dlg->show();
    dlg->raise();

    return true;
}

std::string SIM::DummyCommandEdgeMode::GetUniqueName()
{
    return Name;
}

const std::string SIM::DummyCommandEdgeMode::Name = "DummyCommandEdgeMode";


DummyCommandNodeMode::DummyCommandNodeMode(Application& app) :ContinuousCommand(app)
{
}


DummyCommandNodeMode::~DummyCommandNodeMode(void)
{
}

bool SIM::DummyCommandNodeMode::Execute()
{
	OnStart();
	GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Node);

	QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
	SetDialog(dlg);
	dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
	dlg->setWindowTitle("Node mode");

	QVBoxLayout* vLayout = new QVBoxLayout;
	QGridLayout *gridLayout = new QGridLayout;

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	connect(dlg, &QDialog::rejected, [this]() {
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
		this->OnEnd();
	});

	vLayout->addWidget(buttonBox);

	dlg->setLayout(vLayout);
	dlg->show();
	dlg->raise();

	return true;
}

std::string SIM::DummyCommandNodeMode::GetUniqueName()
{
	return Name;
}

const std::string SIM::DummyCommandNodeMode::Name = "DummyCommandNodeMode";


SIM::DummyCommandGenerateRandomNodes::DummyCommandGenerateRandomNodes(Application & app) :DocCommand(app)
{
}

SIM::DummyCommandGenerateRandomNodes::~DummyCommandGenerateRandomNodes(void)
{

}

bool SIM::DummyCommandGenerateRandomNodes::Execute()
{
    auto& model = GetApplication().GetActiveDocument()->GetModel();

    auto geoms = model.GetStorage().GetObjectsByTypeName(typeid(Body).name());

    for (auto item : geoms)
    {
        auto geom = dynamic_cast<Body*>(item.second.get());
        for (int i = 0; i < geom->GetRawGeometry()->GetNumberOfPoints(); i++)
        {

            auto node = std::make_shared<Node>(*geom, i);
            geom->AddNode(node, model.GetStorage());
        }

        model.NotifyObjectUpdated(item.second, "nodes");
    }
    return false;
}

std::string SIM::DummyCommandGenerateRandomNodes::GetUniqueName()
{
    return DummyCommandGenerateRandomNodes::Name;
}

const std::string SIM::DummyCommandGenerateRandomNodes::Name("DummyCommandGenerateRandomNodes");




DummyCommandInfoCenter::DummyCommandInfoCenter(Application& app) :AppCommand(app)
{
}


DummyCommandInfoCenter::~DummyCommandInfoCenter(void)
{
}

bool SIM::DummyCommandInfoCenter::Execute()
{
	QMessageBox::information(&GetApplication().GetMainWindow(), "Info center", "This is Info Center Dummy Command", QMessageBox::Ok);

	return true;
}

std::string SIM::DummyCommandInfoCenter::GetUniqueName()
{
	return Name;
}

const std::string SIM::DummyCommandInfoCenter::Name = "DummyCommandInfoCenter";




SIM::DummyKeepInCommand::DummyKeepInCommand(Application & app) :DocCommand(app)
{
}

SIM::DummyKeepInCommand::~DummyKeepInCommand(void)
{

}

bool SIM::DummyKeepInCommand::Execute()
{

	auto objs = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();

	std::unordered_set<std::shared_ptr<Object>> modified;
	for (auto obj : objs)
	{
		auto body = dynamic_cast<Body*>(obj.get());

		if (body)
		{
			body->SetItemType(GeometryItemType::KeepIn, GetApplication().GetActiveDocument()->GetModel().GetActiveProblemDefinitionId());
			modified.insert(obj);
		}
	}

	GetApplication().GetActiveDocument()->GetModel().NotifyObjectsUpdated(modified, "ItemTypeChanged");
	
	return false;
}

std::string SIM::DummyKeepInCommand::GetUniqueName()
{
	return DummyKeepInCommand::Name;
}

const std::string SIM::DummyKeepInCommand::Name("DummyKeepInCommand");




DummyCommandGlyph::DummyCommandGlyph(Application& app) :ContinuousCommand(app)
{
}


DummyCommandGlyph::~DummyCommandGlyph(void)
{
}

bool SIM::DummyCommandGlyph::Execute()
{
	OnStart();
	GetApplication().GetActiveDocument()->GetModel().SetSelectionFilter(std::make_shared<ViewActorTypeFilter>(typeid(Geometry).name()));
	GetApplication().GetActiveDocument()->GetModel().SetSelectionContext(SelectionContext::Surface);
	GetApplication().GetActiveDocument()->GetModel().SetHighlights(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().RegisterObserver(*this);

	QDialog* dlg = new QDialog(&GetApplication().GetMainWindow());
	SetDialog(dlg);
	dlg->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
	dlg->setWindowTitle("Create obj on surface");

	QVBoxLayout* vLayout = new QVBoxLayout;
	QGridLayout *gridLayout = new QGridLayout;

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

	double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;
	DummyObjectGlyph obj(0, 0, 0, h, h);
	DummyGlyphViewFactory* factory = dynamic_cast<DummyGlyphViewFactory*>(GetApplication().GetActiveDocument()->GetView().GetView3D().GetFactory(typeid(DummyObjectGlyph).name()));
	_actor = vtkActor::SafeDownCast(factory->GenerateActorForObject(obj));
	_actor->SetPickable(false);
	GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(_actor);

	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	connect(dlg, &QDialog::rejected, [this]() {
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().UnregisterObserver(*this);
		this->GetApplication().GetActiveDocument()->GetView().GetView3D().RemoveActor(_actor);
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionFilter();
		this->GetApplication().GetActiveDocument()->GetModel().ResetSelectionContext();
		GetApplication().GetActiveDocument()->GetModel().SetHighlights(true);
		this->OnEnd();
	});

	vLayout->addWidget(buttonBox);

	dlg->setLayout(vLayout);
	dlg->show();
	dlg->raise();

	return true;
}

std::string SIM::DummyCommandGlyph::GetUniqueName()
{
	return Name;
}

void SIM::DummyCommandGlyph::Notify(Event & ev)
{
	MouseClickEvent* mce = dynamic_cast<MouseClickEvent*>(&ev);
	if (mce) {
		if (mce->GetData().object > -1) {
			double h = this->GetDocument()->GetModel().GetGlobalBounds().GetLength() / 10.0;

			std::unordered_set<std::shared_ptr<Object>> objs = { std::make_shared<DummyObjectGlyph>( mce->GetData().point.x, mce->GetData().point.y, mce->GetData().point.z, h, mce->GetData().object) };			
			GetApplication().GetActiveDocument()->GetController().ExecuteRequest(std::make_shared<AddRequest>(*GetApplication().GetActiveDocument().get(), objs));

		}
	}

	MouseMoveEvent* mme = dynamic_cast<MouseMoveEvent*>(&ev);

	if (mme) {

		if (mme->GetData().object > -1) {
			_actor->VisibilityOn();
			_actor->SetPosition(mme->GetData().point.x, mme->GetData().point.y, mme->GetData().point.z);
			_actor->Modified();
		}
		else
		{
			_actor->VisibilityOff();
		}
	}

}

const std::string SIM::DummyCommandGlyph::Name = "DummyCommandGlyph";




// -------------------------------------------------------------------------------------------

SIM::GenerateFieldNormalsCommand::GenerateFieldNormalsCommand(Application & app) :DocCommand(app)
{
}

SIM::GenerateFieldNormalsCommand::~GenerateFieldNormalsCommand(void)
{

}

bool SIM::GenerateFieldNormalsCommand::Execute()
{
	auto objs = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();
	double len = GetApplication().GetActiveDocument()->GetModel().GetGlobalBounds().GetLength() / 15;

	std::unordered_set<std::shared_ptr<Object>> modified;
	for (auto obj : objs)
	{
		auto body = dynamic_cast<Body*>(obj.get());
		if (body)
		{
			auto raw = body->GetRawGeometry();
			auto normals = raw->GetCellData()->GetNormals();

			for (auto i = 0; i < raw->GetNumberOfCells(); i++)
			{
				double normal[3];

				auto cell = raw->GetCell(i);
				normals->GetTuple(i, normal);

				auto points = cell->GetPointIds();
				double startPoint[3] = { 0, 0, 0 };
				for (auto ptId = 0; ptId < points->GetNumberOfIds(); ptId++)
				{
					auto pt = raw->GetPoint(points->GetId(ptId));
					for (int coord = 0; coord < 3; coord++)
						startPoint[coord] += pt[coord];
				}

				for (int coord = 0; coord < 3; coord++)
					startPoint[coord] /= points->GetNumberOfIds();

				GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(CreateArrow(startPoint, normal, len, 2));
			}
		}
	}

	GetApplication().GetActiveDocument()->GetModel().NotifyObjectsUpdated(objs);

	return false;
}

std::string SIM::GenerateFieldNormalsCommand::GetUniqueName()
{
	return GenerateFieldNormalsCommand::Name;
}

const std::string SIM::GenerateFieldNormalsCommand::Name("GenerateFieldNormalsCommand");


// -------------------------------------------------------------------------------------------

SIM::GeneratePointNormalsCommand::GeneratePointNormalsCommand(Application & app) :DocCommand(app)
{
}

SIM::GeneratePointNormalsCommand::~GeneratePointNormalsCommand(void)
{

}

bool SIM::GeneratePointNormalsCommand::Execute()
{
	auto objs = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();
	double len = GetApplication().GetActiveDocument()->GetModel().GetGlobalBounds().GetLength() / 15;

	std::unordered_set<std::shared_ptr<Object>> modified;
	for (auto obj : objs)
	{
		auto body = dynamic_cast<Body*>(obj.get());
		if (body)
		{
			auto raw = body->GetRawGeometry();
			auto normals = raw->GetPointData()->GetNormals();
			auto points = raw->GetPoints();

			for (auto i = 0; i < raw->GetNumberOfCells(); i++)
			{
				auto cell = raw->GetCell(i);

				for (int p = 0; p < cell->GetNumberOfPoints(); p++)
				{
					double normal[3] = { 0, 0, 0 };
					double startPoint[3] = { 0, 0, 0 };

					normals->GetTuple(cell->GetPointId(p), normal);
					points->GetPoint(cell->GetPointId(p), startPoint);

					GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(CreateArrow(startPoint, normal, len, 2));
				}
			}
		}
	}

	GetApplication().GetActiveDocument()->GetModel().NotifyObjectsUpdated(objs);

	return false;
}

std::string SIM::GeneratePointNormalsCommand::GetUniqueName()
{
	return GeneratePointNormalsCommand::Name;
}

const std::string SIM::GeneratePointNormalsCommand::Name("GeneratePointNormalsCommand");

// -------------------------------------------------------------------------------------------

SIM::GenerateEdgeNormalsCommand::GenerateEdgeNormalsCommand(Application & app) :DocCommand(app)
{
}

SIM::GenerateEdgeNormalsCommand::~GenerateEdgeNormalsCommand(void)
{

}

bool SIM::GenerateEdgeNormalsCommand::Execute()
{
	auto objs = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();
	double len = GetApplication().GetActiveDocument()->GetModel().GetGlobalBounds().GetLength() / 15;

	std::unordered_set<std::shared_ptr<Object>> modified;
	for (auto obj : objs)
	{
		auto edge = dynamic_cast<Edge*>(obj.get());
		if (edge)
		{
			auto raw = edge->GetBody().GetRawGeometry();
			auto normals = raw->GetPointData()->GetNormals();
			auto points = raw->GetPoints();

			double normal[3];
			double startPoint[3];

			if(edge->GetBody().GetNormal(*edge, normal) && edge->GetBody().GetCenterPoint(*edge, startPoint))
				GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(CreateArrow(startPoint, normal, len, 2));
		}
	}

	GetApplication().GetActiveDocument()->GetModel().NotifyObjectsUpdated(objs);

	return false;
}

std::string SIM::GenerateEdgeNormalsCommand::GetUniqueName()
{
	return GenerateEdgeNormalsCommand::Name;
}

const std::string SIM::GenerateEdgeNormalsCommand::Name("GenerateEdgeNormalsCommand");

// -------------------------------------------------------------------------------------------

SIM::GenerateNodeNormalsCommand::GenerateNodeNormalsCommand(Application & app) :DocCommand(app)
{
}

SIM::GenerateNodeNormalsCommand::~GenerateNodeNormalsCommand(void)
{

}

bool SIM::GenerateNodeNormalsCommand::Execute()
{
	auto objs = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();
	double len = GetApplication().GetActiveDocument()->GetModel().GetGlobalBounds().GetLength() / 15;

	std::unordered_set<std::shared_ptr<Object>> modified;
	for (auto obj : objs)
	{
		auto node = dynamic_cast<Node*>(obj.get());
		if (node)
		{
			auto raw = node->GetBody().GetRawGeometry();
			auto normals = raw->GetPointData()->GetNormals();
			auto points = raw->GetPoints();

			double normal[3] = { 0, 0, 0 };
			double startPoint[3] = { 0, 0, 0 };

			if (node->GetBody().GetNormal(*node, normal))
			{
				points->GetPoint(node->GetIndex(), startPoint);

				GetApplication().GetActiveDocument()->GetView().GetView3D().AddActor(CreateArrow(startPoint, normal, len, 2));
			}
		}
	}

	GetApplication().GetActiveDocument()->GetModel().NotifyObjectsUpdated(objs);

	return false;
}

std::string SIM::GenerateNodeNormalsCommand::GetUniqueName()
{
	return GenerateNodeNormalsCommand::Name;
}

const std::string SIM::GenerateNodeNormalsCommand::Name("GenerateNodeNormalsCommand");

// -------------------------------------------------------------------------------------------

SIM::NormalsCommand::NormalsCommand(Application & app) :DocCommand(app)
{
}

SIM::NormalsCommand::~NormalsCommand(void)
{

}

std::string SIM::NormalsCommand::GetUniqueName()
{
	return NormalsCommand::Name;
}

const std::string SIM::NormalsCommand::Name("NormalsCommand");




bool SIM::DebugMode6SocketsCommand::Execute()
{
    DebugModes::DebugModeManager & mgr = DebugModes::DebugModeManager::getInstance();
    mgr.set6SocMode(!DEBUG_MODE(Use6SocketsForProjectList));

    QMessageBox msgBox;
    msgBox.setText((DEBUG_MODE(Use6SocketsForProjectList)) ? "6 (default) sockets in use from now." : "16 (default) sockets in use from now.");
    msgBox.exec();

    return true;
}

std::string SIM::DebugMode6SocketsCommand::GetUniqueName()
{
    return DebugMode6SocketsCommand::Name;
}

std::string SIM::DebugMode6SocketsCommand::GetTooltipKey()
{
    return QObject::tr("Turning this option makes that project list to Open dialog is using Sync.exe with 6 Sockets (default for sync-js).").toStdString();
}

QString SIM::DebugMode6SocketsCommand::GetLabel()
{
    return QObject::tr("6 Sockets");
}

QString SIM::DebugMode6SocketsCommand::GetHint()
{
    return QObject::tr("Turning this option makes that project list to Open dialog is using Sync.exe with 6 Sockets (default for sync-js).");
}

const std::string SIM::DebugMode6SocketsCommand::Name("DebugMode6SocketsCommand");







// -------------------------------------------------------------------------------------------

vtkSmartPointer<vtkActor> CreateArrow(double point[3], double vector[3], double arrawLength, double width)
{
	double startPoint[3] = { point[0], point[1], point[2] };
	double endPoint[3] = { startPoint[0] + vector[0] * arrawLength, startPoint[1] + vector[1] * arrawLength, startPoint[2] + vector[2] * arrawLength };

	//Create an arrow.
	vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
	arrowSource->SetTipLength(0.1);
	arrowSource->SetTipRadius(0.05);
	arrowSource->SetShaftRadius(0.01);

	// Compute a basis
	double normalizedX[3];
	double normalizedY[3];
	double normalizedZ[3];

	// The X axis is a vector from start to end
	vtkMath::Subtract(endPoint, startPoint, normalizedX);
	double length = vtkMath::Norm(normalizedX);
	vtkMath::Normalize(normalizedX);

	// The Z axis is an arbitrary vector cross X
	double arbitrary[3];
	arbitrary[0] = vtkMath::Random(-10, 10);
	arbitrary[1] = vtkMath::Random(-10, 10);
	arbitrary[2] = vtkMath::Random(-10, 10);
	vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
	vtkMath::Normalize(normalizedZ);

	// The Y axis is Z cross X
	vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	// Create the direction cosine matrix
	matrix->Identity();
	for (unsigned int coord = 0; coord < 3; coord++)
	{
		matrix->SetElement(coord, 0, normalizedX[coord]);
		matrix->SetElement(coord, 1, normalizedY[coord]);
		matrix->SetElement(coord, 2, normalizedZ[coord]);
	}

	// Apply the transforms
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(startPoint);
	transform->Concatenate(matrix);
	transform->Scale(length, length, length);

	// Transform the polydata
	vtkSmartPointer<vtkTransformPolyDataFilter> transformPD = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformPD->SetTransform(transform);
	transformPD->SetInputConnection(arrowSource->GetOutputPort());

	//Create a mapper and actor for the arrow
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

	mapper->SetInputConnection(transformPD->GetOutputPort());

	actor->SetMapper(mapper);

	return actor;
}
//*********************************************************

bool SIM::DummyProgressCommand::Execute() 
{ 
    bool show = this->GetCurrentContext() == "Show" || (!_dialog && this->GetCurrentContext() != "Hide");

    if (!show)
    {
        End();
        return true;
    }

    OnStart();
    if (!_dialog)
    {
        auto ic = GetApplication().GetView().GetInfoCenter().GetItem(DummyProgressCommand::Name);

        QWidget* btn = nullptr;

        if (ic)
            btn = ic->GetWidget();

        SetDialog(new DummyProgressDialog(GetApplication(), btn, &GetApplication().GetMainWindow()), true);
        _dialog->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    }

    _dialog->show();


    return true; 
}

SIM::DummyProgressDialog::DummyProgressDialog(Application& app, QWidget *btn, QWidget *parent) : QWidget(parent), _btn(btn), _App(app)
{
    _App.GetMainWindow().installEventFilter(this);
    this->setWindowFlags(Qt::Dialog);
    this->setFixedSize(150, 50);

    QVBoxLayout* layout = new QVBoxLayout(this);

    closeButton = new QPushButton("Close", this);
    closeButton->setDefault(true);
    connect(closeButton, &QAbstractButton::clicked, this, &DummyProgressDialog::CloseDialog);
    layout->addWidget(closeButton);


}

bool SIM::DummyProgressDialog::eventFilter(QObject * obj, QEvent * event)
{

    if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
        RefreshDialogPosition();
        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}

void SIM::DummyProgressDialog::showEvent(QShowEvent * event)
{
    RefreshDialogPosition();
}

void SIM::DummyProgressDialog::RefreshDialogPosition()
{
    auto pos = QPoint(0, 0);

    if (_btn != nullptr)
    {
        pos = _btn->mapToGlobal(pos);

        auto size = _btn->size();
        pos += QPoint(0, size.height());
    }

    auto posLocal = _App.GetMainWindow().mapFromGlobal(pos);
    int dx = _App.GetMainWindow().width() - posLocal.x() - this->frameGeometry().width();

    if (dx < 0)
    {
        pos += QPoint(dx, 0);
    }

    this->move(pos);

}

void SIM::DummyProgressDialog::CloseDialog()
{
    if (_btn != nullptr)
    {
        _btn->setStyleSheet("");
        
    }
    close();
}



