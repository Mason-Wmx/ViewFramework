#include "stdafx.h"
#include "TitleWindow.h"

namespace SIM
{

TitleWindow::TitleWindow(): _frame(nullptr)
{
}


TitleWindow::~TitleWindow(void)
{
}

void TitleWindow::Initialize(void)
{
  _frame = new QFrame();
  _frame->setObjectName("TitleFrame");

  auto layout = new QHBoxLayout(_frame);
  titleLabel = new QLabel();
  titleLabel->setObjectName("TitleLabel");
  layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
}

void TitleWindow::SetTilte(const QString& value)
{
  titleLabel->setText(value);
}

}