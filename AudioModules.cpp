#include "AudioModules.h"


Shaper::Shaper(iplug::IParam* pre, iplug::IParam* post, iplug::IParam* mix, iplug::IParam* wavetype, iplug::IParam* clip,
  iplug::IParam* pregainLfo, iplug::IParam* postgainLfo, iplug::IParam* mixLfo, iplug::IParam* clipLfo, Lfo* lfoPtr)
  : pregainParam(pre)
  , postgainParam(post)
  , mixParam(mix)
  , waveParam(wavetype)
  , clipParam(clip)
  , preGainLfoParam(pregainLfo)
  , postGainLfoParam(postgainLfo)
  , mixLfoParam(mixLfo)
  , clipLfoParam(clipLfo)
  , lfo(lfoPtr)
{}

double Shaper::process(double x){
  const double lfoVal = lfo->getVal();
  const double pregainL = iplug::Clip(pregain + lfoVal * pregainLfo, 0., 1000.);
  const double postgainL = iplug::Clip(postgain + lfoVal * postgainLfo, 0., 400.);
  const double mixL = iplug::Clip(this->mix + lfoVal * mixLfo, 0., 1.);
  const double clipL = std::max(this->clip + lfoVal * clipLfo, 0.);
  double procesed = algo(x * pregainL) * postgainL;
  if (clipL < 5 && std::abs(procesed) > clipL)
    {
    procesed = procesed / abs(procesed) * clipL;
    }
  return procesed * mixL + x * (1. - mixL);
}
void Shaper::updateParams() {
 pregain = pregainParam->Value() / 100.;
 pregainLfo = preGainLfoParam->Value() / 100.;
 postgain = postgainParam->Value() / 100.;
 postgainLfo = postGainLfoParam->Value() / 100.;
 mix = mixParam->Value() / 100.;
 mixLfo = mixLfoParam->Value() / 100.;
 clip = clipParam->Value() / 100.;
 clipLfo = clipLfoParam->Value() / 100.;
 wave = static_cast<Waveform>(waveParam->Int());
 algo = myAlgos.getAlgo(wave);
} 


double Lfo::getRatio(KLfoTimes timeSig)
{
  switch (timeSig)
  {
  case t2beat:
    return 2;
  case t3_2beat:
    return 3. / 2.;
  case t1beat:
    return 1;
  case t3_4beat:
    return 3. / 4.;
  case t2_3beat:
    return 2. / 3.;
  case t1_2beat:
    return 1. / 2.;
  case t1_3beat:
    return 1. / 3.;
  case t1_4beat:
    return 1. / 4.;
  case t1_6beat:
    return 1. / 6.;
  case t1_8beat:
    return 1. / 8.;
  case t1_16beat:
    return 1. / 16.;
  default:
    return 1;
  }
}

void Lfo::updateParams(double sampleRate, double samplesPerBeat, double samplePos)
{
  if (isHostClockedParam->Bool())
  {
    wavelength = getRatio(static_cast<KLfoTimes>(ratioParam->Value())) * samplesPerBeat;
    if (samplePos != -1)
    {
      phase = std::fmod(samplePos, wavelength);
    }
  }
  else
  {
    wavelength = sampleRate / freqParam->Value();
  }
  amp = ampParam->Value();
  offset = offsetParam->Value() * wavelength;
}

double Lfo::getVal() {
  double constexpr pi = 3.141592653589793238462643383279502;
  return std::sin(2 * pi * (phase + offset) / wavelength) * amp;
}
void Lfo::process() {
  phase += 1;
  if (phase >= wavelength)
  {
    phase = 0;
  }

}


VuMeter::VuMeter() {
  Voltage.store(0.0);
}
void VuMeter::process(double x) {
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
void VuMeter::store() {
  Voltage.store(mValue);
}