#pragma once

#include "stdafx.h"
#include "BaseDialog.h"
#include "DocCommand.h"


namespace SIM
{
	class SetViewColorDialog : public BaseDialog
	{
		Q_OBJECT

	public:
		SetViewColorDialog(DocModel& model, Application& app, QWidget *parent = 0);
		~SetViewColorDialog() {};

		private slots:
		void onDefault();
		void onSuggestLight();

	protected:
		virtual void OnOk();
		virtual void OnApply();
		void UpdateColors();

	private:

		QLineEdit* m_SelectionColorR;
		QLineEdit* m_SelectionColorG;
		QLineEdit* m_SelectionColorB;

		QLineEdit* m_PreSelectionColorR;
		QLineEdit* m_PreSelectionColorG;
		QLineEdit* m_PreSelectionColorB;

		QLineEdit* m_GeometryColorR;
		QLineEdit* m_GeometryColorG;
		QLineEdit* m_GeometryColorB;

		QLineEdit* m_EdgeColorR;
		QLineEdit* m_EdgeColorG;
		QLineEdit* m_EdgeColorB;

		QLineEdit* m_NodeColorR;
		QLineEdit* m_NodeColorG;
		QLineEdit* m_NodeColorB;

		QLineEdit* m_KeepinColorR;
		QLineEdit* m_KeepinColorG;
		QLineEdit* m_KeepinColorB;

		QLineEdit* m_KeepoutColorR;
		QLineEdit* m_KeepoutColorG;
		QLineEdit* m_KeepoutColorB;

		QLineEdit* m_SeedColorR;
		QLineEdit* m_SeedColorG;
		QLineEdit* m_SeedColorB;

		QLineEdit* m_KeyLightIntensity;
		QLineEdit* m_KeyLightWarmth;
		QLineEdit* m_FillLightWarmth;
		QLineEdit* m_HeadLightWarmth;
		QLineEdit* m_BackLightWarmth;

		DocModel& m_Model;
		Application& m_app;
	};

	class NFDCAPPCORE_EXPORT SetViewColorCommand:public DocCommand
	{
	public:
		SetViewColorCommand(Application& application);
		
		virtual bool Execute();
		virtual std::string GetUniqueName();
		virtual QString GetLabel() { return "Set View Color"; }

		static const std::string Name;
	};

}