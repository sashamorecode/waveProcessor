#pragma once
#include "IControls.h"
#include "AudioModules.h"

using namespace iplug;
using namespace igraphics;

enum MPlotValType
{
  kPlotIn = 0,
  kPlotOut,
  kPlotWaveType,
  kPlotMix,
  kPlotClip,
};


class VuMeterControl : public IControl
{
public:
  VuMeterControl(const IRECT& bounds, IColor color, int ctrlTag);
  void Draw(IGraphics& g) override;
  void SetValueFromDelegate(double val, int valIdx) override;
    
private:
  IColor mColor = IColor(200, 30, 30, 122);
  double mValue = 0.0;
  int mCtrlTag;
};

class dynamicPlot : public IVPlotControl
{
public:
  dynamicPlot(const IRECT& bounds, std::function<double(double)> func, const char* label = "");
  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override;
  std::function<double(double)> getPlotFunc(Waveform waveType, double gainIn, double gainOut, double mix, double clip);

private:
  double gainIn = 1.;
  double gainOut = 1.;
  double mix = 0.5;
  double clip = 3.0;
  Waveform mWaveType = TANH;
  Algos mAlgo;
};

