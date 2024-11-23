#include "AudioModules.h"


Shaper::Shaper(iplug::IParam* pre, iplug::IParam* post, iplug::IParam* mix, iplug::IParam* wavetype) :
    pregainParam(pre), postgainParam(post), mixParam(mix), waveParam(wavetype) {}
double Shaper::process(double x){
   return algo(x * pregain) * postgain*mix + x * (1. - mix);
}
void Shaper::updateParams() {
 pregain = pregainParam->Value() / 100.;
 postgain = postgainParam->Value() / 100.;
 mix = mixParam->Value() / 100.;
 wave = static_cast<Waveform>(waveParam->Int());
 algo = myAlgos.getAlgo(wave);
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