#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "AudioModules.h"
#include "UiControlModules.h"
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
#define SHAPER(x) Shaper(GetParam(kPreGain##x), GetParam(kPostGain##x), GetParam(kMix##x), GetParam(kWaveType##x))
  Shaper mShaper[3] = {SHAPER(0), SHAPER(1), SHAPER(2)};
  VuMeter* mVuMeters[3] = {new VuMeter(), new VuMeter(), new VuMeter()};
};

