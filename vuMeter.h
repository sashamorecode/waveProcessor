#pragma once
#include "IControls.h"

using namespace iplug;
using namespace igraphics;

class VuMeterControl : public IControl
{
public:
  VuMeterControl(const IRECT& bounds, IColor color, int ctrlTag)
    : IControl(bounds), mCtrlTag(ctrlTag) {
  }  
  void Draw(IGraphics& g) override
  {
    g.FillRect(mColor, mRECT);
    g.FillRect(COLOR_BLACK, mRECT.GetPadded(-2).FracRectVertical(mValue));
  }
  void SetValueFromDelegate(double val, int valIdx) override {
    mValue = val;
    mColor = IColor(200, 225 * mValue, 30, 122);
    SetDirty(false);
  }
  
private:
  IColor mColor = IColor(200, 30, 30, 122);
  double mValue = 0.0;
  int mCtrlTag;
};

class VuMeter
{
public:
  VuMeter() {
    Voltage.store(0.0);
  }
  void process(sample x) {
    const double in = abs(x);
    if (in > mValue)
    {
      mValue = mValue * (1 - kCharge) + in * kCharge;
    }
    else
    {
      mValue = mValue * (1 - kDischarge);
    }
  }
  void store() {
    Voltage.store(mValue);
  }
  std::atomic<double> Voltage;

private:
  const double kCharge = 0.2;
  const double kDischarge = 0.00007;
  double mValue = 0.0;

};
