#pragma once
#include "stdafx.h"

namespace SIM
{
  class TitleWindow : public QObject
  {
    Q_OBJECT
  public:
    TitleWindow();
    ~TitleWindow(void);

    void Initialize(void);
    QFrame* GetFrame() { return _frame; }
    void SetTilte(const QString& value);
  private:
    QFrame* _frame;
    QLabel* titleLabel;
  };
}