#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include "export.h"
/*
 This class is used to unify the dialog's appearance
*/

class QFrame;
class NFDCAPPCORE_EXPORT BaseDialog : public QDialog
{
	Q_OBJECT
protected:
	BaseDialog(QWidget *parent = 0, bool bHeader = true);
	~BaseDialog();

	static void SetLayoutStyle(QLayout* layout);

	void SetHeaderTitle(const QString&);
    void SetHeaderVisible(bool visible);

	QHBoxLayout* GetTitleLayout() { return m_pTitleLayout; };
	QVBoxLayout* GetMainLayout() { return m_pMainLayout; };
	QHBoxLayout* GetDialogLayout() { return m_pDialogLayout; };
	QHBoxLayout* GetBottomLayout() { return m_pBottomLayout; };
	void CreateBottomFrame( bool bOKCancel = true, bool bApply = false);
	QFrame*			BotFrameDivider() { return m_pBotFrameDivider; }

	QPushButton* GetOKButton() { return m_pOKButton; };
	QPushButton* GetCancelButton() { return m_pCancelButton; };
	QPushButton* GetApplyButton() { return m_pApplyButton; };


	virtual void OnOk() {};
	virtual void OnCancel() {};
	virtual void OnApply() {};

protected slots:
	virtual void OkButtonClicked();
	virtual void CancelButtonClicked();
	virtual void ApplyButtonClicked();

private:
	QString getDialogStyle();
	void CreateHeaderFrame();
	
	void SetupMainLayout(bool bHeader);

	///these are needed because the dialog is skinned and cannot be moved as a common qt dialog, it is moved using these events
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:
	QFrame*			m_DialogFrame;
	QFrame*			m_pMainFrame;
	QHBoxLayout* m_pDialogLayout;
	QFrame*			m_pHeaderFrame;
	QHBoxLayout* m_pTitleLayout;
	QVBoxLayout* m_pMainLayout;
	QLabel*			m_pTitleLabel;
	QHBoxLayout* m_pBottomLayout;
	QFrame*			m_pBotFrameDivider;
	QPushButton* m_pOKButton;
	QPushButton* m_pCancelButton;
	QPushButton* m_pApplyButton;
	QPoint m_pointMouseOffset;
	QVBoxLayout * m_pHeaderVBoxLayout;
	QVBoxLayout * m_pBottomVBoxLayout;
	bool m_bMouseMoving;
};


