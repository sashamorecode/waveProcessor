#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "AudioModules.h"
#include "UiControlModules.h"
#define SHAPE(x) kPreGain##x, kPostGain##x, kWaveType##x, kMix##x, kClip##x, kPreGainLfo##x, kPostGainLfo##x, kMixLfo##x, kClipLfo##x
enum EParams
{
  SHAPE(0),
  SHAPE(1),
  SHAPE(2),
  kParallel,
  kLfoIsHostClocked,
  kLfoFreq,
  kLfoRatio,
  kLfoAmp,
  kLfoOffset,
  kNumParams
};
#undef SHAPE

#define MODCTRL(x) kCtrlTagPreGainMod##x, kCtrlTagPostGainMod##x, kCtrlTagMixMod##x, kCtrlTagClipMod##x
  enum KCtrlTags
{
  kCtrlTagVUMeter0 = 0,
  kCtrlTagVUMeter1,
  kCtrlTagVUMeter2,
  kCtrlTagPlot0,
  kCtrlTagPlot1,
  kCtrlTagPlot2,
  kCtrlTagLfo,
  MODCTRL(0),
  MODCTRL(1),
  MODCTRL(2),
};
#undef MODCTRL

const int kNumPresets = 3;

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
#define SHAPER(x) Shaper(GetParam(kPreGain##x), GetParam(kPostGain##x), GetParam(kMix##x), GetParam(kWaveType##x), GetParam(kClip##x),\
GetParam(kPreGainLfo##x), GetParam(kPostGainLfo##x), GetParam(kMixLfo##x), GetParam(kClipLfo##x), &mLfo)
  Lfo mLfo = Lfo(GetParam(kLfoIsHostClocked), GetParam(kLfoFreq), GetParam(kLfoRatio), GetParam(kLfoAmp), GetParam(kLfoOffset));
  Shaper mShaper[3] = {Shaper(GetParam(kPreGain0),
                              GetParam(kPostGain0),
                              GetParam(kMix0),
                              GetParam(kWaveType0),
                              GetParam(kClip0),
                              GetParam(kPreGainLfo0),
                              GetParam(kPostGainLfo0),
                              GetParam(kMixLfo0),
                              GetParam(kClipLfo0),
                              &mLfo),
                       SHAPER(1), SHAPER(2)};
  VuMeter* mVuMeters[3] = {new VuMeter(), new VuMeter(), new VuMeter()};
};

