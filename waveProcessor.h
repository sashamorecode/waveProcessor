#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "shaper.h"
#include "vuMeter.h"
#define SHAPE(x) kPreGain##x, kPostGain##x, kWaveType##x, kMix##x
enum EParams
{
  SHAPE(0),
  SHAPE(1),
  SHAPE(2),
  kParallel,
  kNumParams
};

enum KCtrlTags
{
  kCtrlTagVUMeter0 = 0,
  kCtrlTagVUMeter1,
  kCtrlTagVUMeter2,
  kCtrlTagPlot0,
  kCtrlTagPlot1,
  kCtrlTagPlot2,
};

enum MPlotValType
{
  kPlotIn = 0,
  kPlotOut,
  kPlotWaveType,
  kPlotMix
};

const int kNumPresets = 1;

using namespace iplug;
using namespace igraphics;
using Plot = IVPlotControl::Plot;

class waveProcessor final : public Plugin
{
public:
  waveProcessor(const InstanceInfo& info);
  void OnIdle() override;
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif


private:
#define SHAPER(x) shaper(GetParam(kPreGain##x), GetParam(kPostGain##x), GetParam(kMix##x), GetParam(kWaveType##x))
  shaper mShaper[3] = {SHAPER(0), SHAPER(1), SHAPER(2)};
  VuMeter* mVuMeters[3] = {new VuMeter(), new VuMeter(), new VuMeter()};
};

class dynamicPlot : public IVPlotControl
{
public:
  dynamicPlot(const IRECT& bounds, std::function<double(double)> func, const char* label = "")
    : 
    IVPlotControl(bounds, {Plot{COLOR_WHITE, func}}, 2048, label) {
  }

  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {

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
      
      const waveform newWaveType = static_cast<waveform>(*static_cast<const int*>(pData));
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
  std::function<double(double)> getPlotFunc(waveform waveType, double gainIn, double gainOut, double mix)
  {
    const auto algo = mAlgo.getAlgo(waveType);
    return [algo,  gainIn, gainOut, mix](double x) -> double { return algo((x * 2. - 1.) * gainIn) * gainOut * mix + (x * 2. - 1) * (1. - mix); };
  }

 private:
  double gainIn = 1.;
  double gainOut = 1.;
  double mix = 0.5;
  waveform mWaveType = TANH;
  algos mAlgo;
};
