#pragma once

#include <QCheckBox>
#include "export.h"

namespace SIM
{
  class NFDCAPPCORE_EXPORT SwitchCheckBox : public QCheckBox
  {
    Q_OBJECT

  public:
    SwitchCheckBox(QWidget *parent = 0);
    ~SwitchCheckBox();

  protected:

  private:

  };

  class NFDCAPPCORE_EXPORT SwitchExpander : public QCheckBox
  {
    Q_OBJECT

  public:
    SwitchExpander(QWidget *parent = 0);
    ~SwitchExpander();

  protected:

  private:

  };
}

