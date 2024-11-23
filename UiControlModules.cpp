#include "uiControlModules.h"
#include "IControls.h"


VuMeterControl::VuMeterControl(const IRECT& bounds, IColor color, int ctrlTag)

    : IControl(bounds), mCtrlTag(ctrlTag) {
  }  
void VuMeterControl::Draw(IGraphics& g)
{
  g.FillRect(mColor, mRECT);
  g.FillRect(COLOR_BLACK, mRECT.GetPadded(-2).FracRectVertical(mValue));
}
void VuMeterControl::SetValueFromDelegate(double val, int valIdx) {
  mValue = val;
  mColor = IColor(200, 225 * mValue, 30, 122);
  SetDirty(false);
}

dynamicPlot::dynamicPlot(const IRECT& bounds, std::function<double(double)> func, const char* label)
    : 
    IVPlotControl(bounds, {Plot{COLOR_WHITE, func}}, 2048, label) {
 }



void dynamicPlot::OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) 
 {
  if (msgTag== kPlotOut) {
    const double newGain = *static_cast<const double*>(pData) / 100.;
    if (newGain != gainOut)
    {
      gainOut = newGain;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix);
      SetDirty();
    }
  }
  else if (msgTag== kPlotIn)
  {
    const double newGain = *static_cast<const double*>(pData) / 100.;
    if (newGain != gainIn)
    {
      gainIn = newGain;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix);
      SetDirty();
    }
  }
  else if (msgTag== kPlotWaveType)
  {
    
    const Waveform newWaveType = static_cast<Waveform>(*static_cast<const int*>(pData));
    if (newWaveType != mWaveType)
    {
      mWaveType = newWaveType;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix);
      SetDirty();
    }
  }
  else if (msgTag == kPlotMix)
  {
    const double newMix = *static_cast<const double*>(pData) / 100.;
    if (newMix != mix)
    {
      mix = newMix;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix);
      SetDirty();
    }
  }
  else
  {
    assert(false);
  }
}
std::function<double(double)> dynamicPlot::getPlotFunc(Waveform waveType, double gainIn, double gainOut, double mix)
{
  const auto algo = mAlgo.getAlgo(waveType);
  return [algo,  gainIn, gainOut, mix](double x) -> double { return algo((x * 2. - 1.) * gainIn) * gainOut * mix + (x * 2. - 1) * (1. - mix); };
}

