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

constexpr int kCtrlTagVUMeter0 = 0;
constexpr int kCtrlTagVUMeter1 = 1;
constexpr int kCtrlTagVUMeter2 = 2;
const int kNumPresets = 1;

using namespace iplug;
using namespace igraphics;


class waveProcessor final : public Plugin
{
public:
  waveProcessor(const InstanceInfo& info);
  void OnIdle() override;
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
private:
  shaper mShaper[3];
  VuMeter* mVuMeters[3] = {new VuMeter(), new VuMeter(), new VuMeter()};
};


