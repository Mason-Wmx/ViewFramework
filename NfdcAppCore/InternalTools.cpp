
#include "stdafx.h"
#include "InternalTools.h"
#include "mainwindow.h"
#include "View3D.h"
#include "View3dCommand.h"
#include "vtkLightKit.h"

using namespace SIM;

SetViewColorDialog::SetViewColorDialog(DocModel& model, Application& app, QWidget *parent)
	: BaseDialog(parent),
	m_Model(model),
	m_app(app)
{
	BaseDialog::SetHeaderTitle("Set View Color");

	QVBoxLayout* pMainLayout = GetMainLayout();

	// colors
	QGridLayout* layoutColor = new QGridLayout(this);
	pMainLayout->addLayout(layoutColor);
	int row = 0;

	QLabel* colorRange = new QLabel("RGB Color value range: 0~255", this);
	layoutColor->addWidget(colorRange, row, 0);
	++row;

	QLabel* r = new QLabel("R", this);
	QLabel* g = new QLabel("G", this);
	QLabel* b = new QLabel("B", this);
	layoutColor->addWidget(r, row, 1);
	layoutColor->addWidget(g, row, 2);
	layoutColor->addWidget(b, row, 3);
	++row;

	QLabel* selectionColor = new QLabel("Selected geometry Color", this);
	m_SelectionColorR = new QLineEdit(this);
	m_SelectionColorG = new QLineEdit(this);
	m_SelectionColorB = new QLineEdit(this);
	layoutColor->addWidget(selectionColor, row, 0);
	layoutColor->addWidget(m_SelectionColorR, row, 1);
	layoutColor->addWidget(m_SelectionColorG, row, 2);
	layoutColor->addWidget(m_SelectionColorB, row, 3);
	++row;

	QLabel* preSelectionColor = new QLabel("Pre-selected geometry Color", this);
	m_PreSelectionColorR = new QLineEdit(this);
	m_PreSelectionColorG = new QLineEdit(this);
	m_PreSelectionColorB = new QLineEdit(this);
	layoutColor->addWidget(preSelectionColor, row, 0);
	layoutColor->addWidget(m_PreSelectionColorR, row, 1);
	layoutColor->addWidget(m_PreSelectionColorG, row, 2);
	layoutColor->addWidget(m_PreSelectionColorB, row, 3);
	++row;

	QLabel* geometryColor = new QLabel("Body Color", this);
	m_GeometryColorR = new QLineEdit(this);
	m_GeometryColorG = new QLineEdit(this);
	m_GeometryColorB = new QLineEdit(this);
	layoutColor->addWidget(geometryColor, row, 0);
	layoutColor->addWidget(m_GeometryColorR, row, 1);
	layoutColor->addWidget(m_GeometryColorG, row, 2);
	layoutColor->addWidget(m_GeometryColorB, row, 3);
	++row;

	QLabel* edgeColor = new QLabel("Edge Color", this);
	m_EdgeColorR = new QLineEdit(this);
	m_EdgeColorG = new QLineEdit(this);
	m_EdgeColorB = new QLineEdit(this);
	layoutColor->addWidget(edgeColor, row, 0);
	layoutColor->addWidget(m_EdgeColorR, row, 1);
	layoutColor->addWidget(m_EdgeColorG, row, 2);
	layoutColor->addWidget(m_EdgeColorB, row, 3);
	++row;

	QLabel* nodeColor = new QLabel("Node Color", this);
	m_NodeColorR = new QLineEdit(this);
	m_NodeColorG = new QLineEdit(this);
	m_NodeColorB = new QLineEdit(this);
	layoutColor->addWidget(nodeColor, row, 0);
	layoutColor->addWidget(m_NodeColorR, row, 1);
	layoutColor->addWidget(m_NodeColorG, row, 2);
	layoutColor->addWidget(m_NodeColorB, row, 3);
	++row;

	QLabel* keepinColor = new QLabel("Keep in Color", this);
	m_KeepinColorR = new QLineEdit(this);
	m_KeepinColorG = new QLineEdit(this);
	m_KeepinColorB = new QLineEdit(this);
	layoutColor->addWidget(keepinColor, row, 0);
	layoutColor->addWidget(m_KeepinColorR, row, 1);
	layoutColor->addWidget(m_KeepinColorG, row, 2);
	layoutColor->addWidget(m_KeepinColorB, row, 3);
	++row;

	QLabel* keepoutColor = new QLabel("Keep out Color", this);
	m_KeepoutColorR = new QLineEdit(this);
	m_KeepoutColorG = new QLineEdit(this);
	m_KeepoutColorB = new QLineEdit(this);
	layoutColor->addWidget(keepoutColor, row, 0);
	layoutColor->addWidget(m_KeepoutColorR, row, 1);
	layoutColor->addWidget(m_KeepoutColorG, row, 2);
	layoutColor->addWidget(m_KeepoutColorB, row, 3);
	++row;

	QLabel* seedColor = new QLabel("Seed Color", this);
	m_SeedColorR = new QLineEdit(this);
	m_SeedColorG = new QLineEdit(this);
	m_SeedColorB = new QLineEdit(this);
	layoutColor->addWidget(seedColor, row, 0);
	layoutColor->addWidget(m_SeedColorR, row, 1);
	layoutColor->addWidget(m_SeedColorG, row, 2);
	layoutColor->addWidget(m_SeedColorB, row, 3);
	++row;

	// divider 1
	QFrame* divider1 = new QFrame(this);
	divider1->setFrameShape(QFrame::HLine);
	pMainLayout->addWidget(divider1);

	// lights
	QGridLayout* layoutLight = new QGridLayout(this);
	pMainLayout->addLayout(layoutLight);
	row = 0;

	QLabel* lightRange = new QLabel("Light value range: 0~1\nvtk light has limited colors defined by warmth which is:\n0: blue (the coldest)\n1: red (the hottest)\n0.5: white", this);
	layoutLight->addWidget(lightRange, row, 0);
	++row;

	QLabel* keyLightIntensity = new QLabel("Key Light Intensity", this);
	m_KeyLightIntensity = new QLineEdit(this);
	layoutLight->addWidget(keyLightIntensity, row, 0);
	layoutLight->addWidget(m_KeyLightIntensity, row, 1);
	++row;

	QLabel* keyLightWarmth = new QLabel("Key Light Warmth", this);
	m_KeyLightWarmth = new QLineEdit(this);
	layoutLight->addWidget(keyLightWarmth, row, 0);
	layoutLight->addWidget(m_KeyLightWarmth, row, 1);
	++row;

	QLabel* fillLightWarmth  = new QLabel("Fill Light Warmth", this);
	m_FillLightWarmth = new QLineEdit(this);
	layoutLight->addWidget(fillLightWarmth, row, 0);
	layoutLight->addWidget(m_FillLightWarmth, row, 1);
	++row;

	QLabel* headLightWarmth = new QLabel("Head Light Warmth", this);
	m_HeadLightWarmth = new QLineEdit(this);
	layoutLight->addWidget(headLightWarmth, row, 0);
	layoutLight->addWidget(m_HeadLightWarmth, row, 1);
	++row;

	QLabel* backLightWarmth = new QLabel("Back Light Warmth", this);
	m_BackLightWarmth = new QLineEdit(this);
	layoutLight->addWidget(backLightWarmth, row, 0);
	layoutLight->addWidget(m_BackLightWarmth, row, 1);
	++row;

	QPushButton* suggest = new QPushButton("Use vtk Default Light", this);
	layoutLight->addWidget(suggest, row, 0);
	connect(suggest, SIGNAL(clicked()), this, SLOT(onSuggestLight()));
	++row;

	// divider 2
	QFrame* divider2 = new QFrame(this);
	divider2->setFrameShape(QFrame::HLine);
	pMainLayout->addWidget(divider2);

	// buttons
	QGridLayout* layoutButton = new QGridLayout(this);
	pMainLayout->addLayout(layoutButton);
	row = 0;

	QPushButton* default = new QPushButton("Set to Default", this);
	layoutButton->addWidget(default, row, 1);
	connect(default, SIGNAL(clicked()), this, SLOT(onDefault()));
	++row;

	BaseDialog::CreateBottomFrame(true, true);

	SIM::ViewSettings& viewSettings = m_Model.GetViewSettings();

	// load color values
	SIM::RenderProps& selectionProps = viewSettings.GetSelectionProps();
	m_SelectionColorR->setText(QString::number(int(selectionProps.color.R * 255)));
	m_SelectionColorG->setText(QString::number(int(selectionProps.color.G * 255)));
	m_SelectionColorB->setText(QString::number(int(selectionProps.color.B * 255)));

	SIM::RenderProps& preselectionProps = viewSettings.GetPreselectionProps();
	m_PreSelectionColorR->setText(QString::number(int(preselectionProps.color.R * 255)));
	m_PreSelectionColorG->setText(QString::number(int(preselectionProps.color.G * 255)));
	m_PreSelectionColorB->setText(QString::number(int(preselectionProps.color.B * 255)));

	SIM::RenderProps& geometryProps = viewSettings.GetGeometryProps();
	m_GeometryColorR->setText(QString::number(int(geometryProps.color.R * 255)));
	m_GeometryColorG->setText(QString::number(int(geometryProps.color.G * 255)));
	m_GeometryColorB->setText(QString::number(int(geometryProps.color.B * 255)));

	SIM::RenderProps& geometryEdgeProps = viewSettings.GetGeometryEdgeProps();
	m_EdgeColorR->setText(QString::number(int(geometryEdgeProps.color.R * 255)));
	m_EdgeColorG->setText(QString::number(int(geometryEdgeProps.color.G * 255)));
	m_EdgeColorB->setText(QString::number(int(geometryEdgeProps.color.B * 255)));

	SIM::RenderProps& geometryNodeProps = viewSettings.GetGeometryNodeProps();
	m_NodeColorR->setText(QString::number(int(geometryNodeProps.color.R * 255)));
	m_NodeColorG->setText(QString::number(int(geometryNodeProps.color.G * 255)));
	m_NodeColorB->setText(QString::number(int(geometryNodeProps.color.B * 255)));

	m_KeepinColorR->setText(QString::number(int(geometryProps.colorKeepIn.R * 255)));
	m_KeepinColorG->setText(QString::number(int(geometryProps.colorKeepIn.G * 255)));
	m_KeepinColorB->setText(QString::number(int(geometryProps.colorKeepIn.B * 255)));

	m_KeepoutColorR->setText(QString::number(int(geometryProps.colorKeepOut.R * 255)));
	m_KeepoutColorG->setText(QString::number(int(geometryProps.colorKeepOut.G * 255)));
	m_KeepoutColorB->setText(QString::number(int(geometryProps.colorKeepOut.B * 255)));

	m_SeedColorR->setText(QString::number(int(geometryProps.colorSeed.R * 255)));
	m_SeedColorG->setText(QString::number(int(geometryProps.colorSeed.G * 255)));
	m_SeedColorB->setText(QString::number(int(geometryProps.colorSeed.B * 255)));

	// load light values
	m_KeyLightIntensity->setText(QString::number(viewSettings.GetKeyLightIntensity()));
	m_KeyLightWarmth->setText(QString::number(viewSettings.GetKeyLightWarmth()));
	m_FillLightWarmth->setText(QString::number(viewSettings.GetFillLightWarmth()));
	m_HeadLightWarmth->setText(QString::number(viewSettings.GetHeadLightWarmth()));
	m_BackLightWarmth->setText(QString::number(viewSettings.GetBackLightWarmth()));

}

void SetViewColorDialog::OnApply()
{
	UpdateColors();
}

void SetViewColorDialog::OnOk()
{
	UpdateColors();
	setResult(1);
}

void SetViewColorDialog::onDefault()
{
	SIM::ViewSettings viewSettings;
	SIM::RenderProps& selectionProps = viewSettings.GetSelectionProps();
	m_SelectionColorR->setText(QString::number(int(selectionProps.color.R * 255)));
	m_SelectionColorG->setText(QString::number(int(selectionProps.color.G * 255)));
	m_SelectionColorB->setText(QString::number(int(selectionProps.color.B * 255)));

	SIM::RenderProps& preselectionProps = viewSettings.GetPreselectionProps();
	m_PreSelectionColorR->setText(QString::number(int(preselectionProps.color.R * 255)));
	m_PreSelectionColorG->setText(QString::number(int(preselectionProps.color.G * 255)));
	m_PreSelectionColorB->setText(QString::number(int(preselectionProps.color.B * 255)));

	SIM::RenderProps& geometryProps = viewSettings.GetGeometryProps();
	m_GeometryColorR->setText(QString::number(int(geometryProps.color.R * 255)));
	m_GeometryColorG->setText(QString::number(int(geometryProps.color.G * 255)));
	m_GeometryColorB->setText(QString::number(int(geometryProps.color.B * 255)));

	SIM::RenderProps& geometryEdgeProps = viewSettings.GetGeometryEdgeProps();
	m_EdgeColorR->setText(QString::number(int(geometryEdgeProps.color.R * 255)));
	m_EdgeColorG->setText(QString::number(int(geometryEdgeProps.color.G * 255)));
	m_EdgeColorB->setText(QString::number(int(geometryEdgeProps.color.B * 255)));

	SIM::RenderProps& geometryNodeProps = viewSettings.GetGeometryNodeProps();
	m_NodeColorR->setText(QString::number(int(geometryNodeProps.color.R * 255)));
	m_NodeColorG->setText(QString::number(int(geometryNodeProps.color.G * 255)));
	m_NodeColorB->setText(QString::number(int(geometryNodeProps.color.B * 255)));

	m_KeepinColorR->setText(QString::number(int(geometryProps.colorKeepIn.R * 255)));
	m_KeepinColorG->setText(QString::number(int(geometryProps.colorKeepIn.G * 255)));
	m_KeepinColorB->setText(QString::number(int(geometryProps.colorKeepIn.B * 255)));

	m_KeepoutColorR->setText(QString::number(int(geometryProps.colorKeepOut.R * 255)));
	m_KeepoutColorG->setText(QString::number(int(geometryProps.colorKeepOut.G * 255)));
	m_KeepoutColorB->setText(QString::number(int(geometryProps.colorKeepOut.B * 255)));

	m_SeedColorR->setText(QString::number(int(geometryProps.colorSeed.R * 255)));
	m_SeedColorG->setText(QString::number(int(geometryProps.colorSeed.G * 255)));
	m_SeedColorB->setText(QString::number(int(geometryProps.colorSeed.B * 255)));

	m_KeyLightIntensity->setText(QString::number(viewSettings.GetKeyLightIntensity()));
	m_KeyLightWarmth->setText(QString::number(viewSettings.GetKeyLightWarmth()));
	m_FillLightWarmth->setText(QString::number(viewSettings.GetFillLightWarmth()));
	m_HeadLightWarmth->setText(QString::number(viewSettings.GetHeadLightWarmth()));
	m_BackLightWarmth->setText(QString::number(viewSettings.GetBackLightWarmth()));
}

void SetViewColorDialog::onSuggestLight()
{
	auto lightKit = vtkSmartPointer<vtkLightKit>::New();

	m_KeyLightIntensity->setText(QString::number(lightKit->GetKeyLightIntensity()));
	m_KeyLightWarmth->setText(QString::number(lightKit->GetKeyLightWarmth()));
	m_FillLightWarmth->setText(QString::number(lightKit->GetFillLightWarmth()));
	m_HeadLightWarmth->setText(QString::number(lightKit->GetHeadLightWarmth()));
	m_BackLightWarmth->setText(QString::number(lightKit->GetBackLightWarmth()));
}

void SetViewColorDialog::UpdateColors()
{
	SIM::ViewSettings& viewSettings = m_Model.GetViewSettings();

	SIM::RenderProps& selectionProps = viewSettings.GetSelectionProps();
	selectionProps.color.R = m_SelectionColorR->text().toInt() / 255.0;
	selectionProps.color.G = m_SelectionColorG->text().toInt() / 255.0;
	selectionProps.color.B = m_SelectionColorB->text().toInt() / 255.0;

	SIM::RenderProps& preselectionProps = viewSettings.GetPreselectionProps();
	preselectionProps.color.R = m_PreSelectionColorR->text().toInt() / 255.0;
	preselectionProps.color.G = m_PreSelectionColorG->text().toInt() / 255.0;
	preselectionProps.color.B = m_PreSelectionColorB->text().toInt() / 255.0;

	SIM::RenderProps& geometryProps = viewSettings.GetGeometryProps();
	geometryProps.color.R = m_GeometryColorR->text().toInt() / 255.0;
	geometryProps.color.G = m_GeometryColorG->text().toInt() / 255.0;
	geometryProps.color.B = m_GeometryColorB->text().toInt() / 255.0;

	SIM::RenderProps& geometryEdgeProps = viewSettings.GetGeometryEdgeProps();
	geometryEdgeProps.color.R = m_EdgeColorR->text().toInt() / 255.0;
	geometryEdgeProps.color.G = m_EdgeColorG->text().toInt() / 255.0;
	geometryEdgeProps.color.B = m_EdgeColorB->text().toInt() / 255.0;

	SIM::RenderProps& geometryNodeProps = viewSettings.GetGeometryNodeProps();
	geometryNodeProps.color.R = m_NodeColorR->text().toInt() / 255.0;
	geometryNodeProps.color.G = m_NodeColorG->text().toInt() / 255.0;
	geometryNodeProps.color.B = m_NodeColorB->text().toInt() / 255.0;

	geometryProps.colorKeepIn.R = m_KeepinColorR->text().toInt() / 255.0;
	geometryProps.colorKeepIn.G = m_KeepinColorG->text().toInt() / 255.0;
	geometryProps.colorKeepIn.B = m_KeepinColorB->text().toInt() / 255.0;

	geometryProps.colorKeepOut.R = m_KeepoutColorR->text().toInt() / 255.0;
	geometryProps.colorKeepOut.G = m_KeepoutColorG->text().toInt() / 255.0;
	geometryProps.colorKeepOut.B = m_KeepoutColorB->text().toInt() / 255.0;

	geometryProps.colorSeed.R = m_SeedColorR->text().toInt() / 255.0;
	geometryProps.colorSeed.G = m_SeedColorG->text().toInt() / 255.0;
	geometryProps.colorSeed.B = m_SeedColorB->text().toInt() / 255.0;

	viewSettings.GetKeyLightIntensity() = m_KeyLightIntensity->text().toDouble();
	viewSettings.GetKeyLightWarmth() = m_KeyLightWarmth->text().toDouble();
	viewSettings.GetFillLightWarmth() = m_FillLightWarmth->text().toDouble();
	viewSettings.GetHeadLightWarmth() = m_HeadLightWarmth->text().toDouble();
	viewSettings.GetBackLightWarmth() = m_BackLightWarmth->text().toDouble();

    m_Model.GetParent().GetView().GetView3D().ChangeLight();

	m_Model.NotifyModelReloaded();

	SIM::Command* pCmd = m_app.GetController().GetCommand(View3dFrontCommand::Name);
	if (pCmd)
		pCmd->Execute();
	
}

SIM::SetViewColorCommand::SetViewColorCommand(Application & application):DocCommand(application)
{
}

bool SIM::SetViewColorCommand::Execute()
{
    if (GetApplication().GetActiveDocument() == nullptr)
        return false;
	DocModel& model = GetApplication().GetActiveDocument()->GetModel();
	SetViewColorDialog* dlg = new SetViewColorDialog(model, GetApplication(), &(GetApplication().GetMainWindow()));
	dlg->show();
	dlg->raise();

	return true;
}

std::string SIM::SetViewColorCommand::GetUniqueName()
{
	return SetViewColorCommand::Name;
}

const std::string SIM::SetViewColorCommand::Name("SetViewColorCommand");

