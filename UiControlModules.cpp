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
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix, clip);
      SetDirty();
    }
  }
  else if (msgTag== kPlotIn)
  {
    const double newGain = *static_cast<const double*>(pData) / 100.;
    if (newGain != gainIn)
    {
      gainIn = newGain;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix, clip);
      SetDirty();
    }
  }
  else if (msgTag== kPlotWaveType)
  {
    
    const Waveform newWaveType = static_cast<Waveform>(*static_cast<const int*>(pData));
    if (newWaveType != mWaveType)
    {
      mWaveType = newWaveType;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix, clip);
      SetDirty();
    }
  }
  else if (msgTag == kPlotMix)
  {
    const double newMix = *static_cast<const double*>(pData) / 100.;
    if (newMix != mix)
    {
      mix = newMix;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix, clip);
      SetDirty();
    }
  }
  else if (msgTag == kPlotClip)
  {
    const double newClip = *static_cast<const double*>(pData) / 100.;
    if (newClip != clip)
    {
      clip = newClip;
      this->mPlots[0].func = getPlotFunc(mWaveType, gainIn, gainOut, mix, clip);
      SetDirty();
    }
  }
  else
  {
    assert(false);
  }
}
 std::function<double(double)> dynamicPlot::getPlotFunc(Waveform waveType, double gainIn, double gainOut, double mix, double clip)
 {
  const auto algo = mAlgo.getAlgo(waveType);
   return [algo, gainIn, gainOut, mix, clip](double x) -> double {
    double procesed = algo((x * 2. - 1.) * gainIn) * gainOut;
    if (clip < 5 && std::abs(procesed) > clip)
    {
      procesed = procesed / std::abs(procesed) * clip;
    }
    return procesed * mix + (x * 2. - 1) * (1. - mix);
  };
}

void LfoModuleControl::Draw(IGraphics& g)
{
  if (isHostClocked)
  {
    mRatioControl->Draw(g);
  }
  else
  {
    mFreqControl->Draw(g);
  }
}

void LfoModuleControl::SetValueFromDelegate(double val, int valIdx) {
  isHostClocked = static_cast<bool>(val);
  if (isHostClocked)
  {
    mFreqControl->Hide(true);
    mRatioControl->Hide(false);
  }
  else
  {
    mFreqControl->Hide(false);
    mRatioControl->Hide(true);
  }

  SetDirty(false);
}

void ModKnobControl::DrawWidget(IGraphics& g) {
  float widgetRadius; // The radius out to the indicator track arc

  if (mWidgetBounds.W() > mWidgetBounds.H())
    widgetRadius = (mWidgetBounds.H() / 2.f);
  else
    widgetRadius = (mWidgetBounds.W() / 2.f);

  const float cx = mWidgetBounds.MW(), cy = mWidgetBounds.MH();

  widgetRadius -= (mTrackSize / 2.f);

  IRECT knobHandleBounds = mWidgetBounds.GetCentredInside((widgetRadius - mTrackToHandleDistance) * 2.f);
  const float angle = mAngle1 + (static_cast<float>(GetValue()) * (mAngle2 - mAngle1));
  mAnchorAngle = angle;
  DrawIndicatorTrack(g, mVal + angle, cx, cy, widgetRadius);
  DrawPressableShape(g, /*mShape*/ EVShape::Ellipse, knobHandleBounds, mMouseDown, mMouseIsOver, IsDisabled());
  DrawPointer(g, angle, cx, cy, knobHandleBounds.W() / 2.f);
}

void ModKnobControl::OnMsgFromDelegate(int msgTag, int msgSize, const void* msg)
{
  if (!IsDisabled())
  {
    const double newMVal = *static_cast<const double*>(msg);
    if (newMVal != mVal)
    {
      mVal = newMVal;
      SetDirty(false);
    }
  }
}
