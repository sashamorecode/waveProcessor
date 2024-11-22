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
  kPlotWaveType
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
    IVPlotControl(bounds, {Plot{IColor(255, 0, 0, 0), func}}, 1024, label) {
    myFunc = func;
  }

  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {

    if (msgTag== kPlotOut) {
      gainOut = *static_cast<const double*>(pData) / 100.;
    }
    else if (msgTag== kPlotIn)
    {
      gainIn = *static_cast<const double*>(pData) / 100.;
    }
    else
    {
      mWaveType = static_cast<waveform>(*static_cast<const int*>(pData));
    }
    auto algo = getAlgo();
    std::function<double(double)> newFunc = [this, algo](double x)->double { return algo((x*2. -1.) * gainIn) * gainOut; };  
    this->mPlots[0].func = newFunc;
    SetDirty();
  } 
   std::function<double(double)> getAlgo()
  {
    switch (mWaveType)
    {
    case ARCTAN:
      return arctan;
    case TANH:
      return tanh;
    case TANH3:
      return tanh3;
    case SIN:
      return sin;
    case SIN3:
      return sin3;
    case SIGMOID:
      return sigmoid;
    case TANHTAN:
      return tanhtan;
    case TANHTAN3:
      return tanhtan3;
    case TANHEXP:
      return tanhexp;
    case SINTAN:
      return sintan;
    case SINTAN3:
      return sintan3;
    case SINEXP:
      return sinexp;
    default:
      return tanh;
    }
  }

private:
  std::function<double(double)> myFunc = [](double x) { return x; };
  double gainIn = 1.;
  double gainOut = 1.;
  waveform mWaveType = TANH;
  inline static double arctan(double x) { return std::atan(x); }
  inline static double tanh(double x) { return std::tanh(x); }
  inline static double tanh3(double x) { return std::tanh(std::pow(x, 3.)); }
  inline static double sin(double x) { return std::sin(x); }
  inline static double sin3(double x) { return std::sin(std::pow(x, 3.)); }
  inline static double sigmoid(double x) { return (1. / (1 + std::exp(-x*2.))) - 0.5; }
  inline static double tanhtan(double x) { return tanh(tan(x)); }
  inline static double tanhtan3(double x) { return tanh(tan(std::pow(x, 3.))); }
  inline static double tanhexp(double x) { return tanh(std::exp(x)*x); }
  inline static double sintan(double x) { return sin(tan(x)); }
  inline static double sintan3(double x) { return sin(tan(std::pow(x, 3.))); }
  inline static double sinexp(double x) { return sin(std::exp(x)*x); }


};
