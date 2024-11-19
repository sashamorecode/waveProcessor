#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "shaper.h"
#define SHAPE(x) kPreGain##x, kPostGain##x, kWaveType##x, kMix##x
enum EParams
{
  SHAPE(0),
  SHAPE(1),
  SHAPE(2),
  kNumParams
};
const int kNumPresets = 1;

using namespace iplug;
using namespace igraphics;


class waveProcessor final : public Plugin
{
public:
  waveProcessor(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
private:
  shaper mShaper[3];
};



