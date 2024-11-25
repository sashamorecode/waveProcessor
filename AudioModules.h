#pragma once
#include "IPlug_include_in_plug_hdr.h"
enum Waveform
{
  ARCTAN,
  TANH,
  TANH3,
  SIN,
  SIN3,
  SIGMOID,
  TANHTAN,
  TANHTAN3,
  TANHEXP,
  SINTAN,
  SINTAN3,
  SINEXP,
};


struct Algos
{
  static double arctan(double x) { return std::atan(x); }
  static double tanh(double x) { return std::tanh(x); }
  static double tanh3(double x) { return std::tanh(std::pow(x, 3.)); }
  static double sin(double x) { return std::sin(x); }
  static double sin3(double x) { return std::sin(std::pow(x, 3.)); }
  static double sigmoid(double x) { return (1. / (1 + std::exp(-x * 2.))) - 0.5; }
  static double tanhtan(double x) { return tanh(tan(x)); }
  static double tanhtan3(double x) { return tanh(tan(std::pow(x, 3.))); }
  static double tanhexp(double x) { return tanh(std::exp(x)*x); }
  static double sintan(double x) { return sin(tan(x)); }
  static double sintan3(double x) { return sin(tan(std::pow(x, 3.))); }
  static double sinexp(double x) { return sin(std::exp(x)*x); }
  std::function<double(double)> getAlgo(Waveform wave) {
    switch (wave)
    {
    case ::ARCTAN:
      return arctan;
    case ::TANH:
      return tanh;
    case ::TANH3:
      return tanh3;
    case ::SIN:
      return sin;
    case ::SIN3:
      return sin3;
    case ::SIGMOID:
      return sigmoid;
    case ::TANHTAN:
      return tanhtan;
    case ::TANHTAN3:
      return tanhtan3;
    case ::TANHEXP:
      return tanhexp;
    case ::SINTAN:
      return sintan;
    case ::SINTAN3:
      return sintan3;
    case ::SINEXP:
      return sinexp;
    default:
      return arctan;
    }
  }
};

enum KLfoTimes
{
  t2beat = 0,
  t3_2beat,
  t1beat,
  t3_4beat,
  t2_3beat,
  t1_2beat,
  t1_3beat,
  t1_4beat,
  t1_6beat,
  t1_8beat,
  t1_16beat,
  tnum_lfo_times,
};
static std::vector<std::string> lfo_times = {"2/1", "3/2", "1/1", "3/4", "2/3", "1/2", "1/3", "1/4", "1/6", "1/8", "1/16"};



class Lfo
{
public:
  Lfo(iplug::IParam* isHostClocked, iplug::IParam* freq, iplug::IParam* ratio, iplug::IParam* amp, iplug::IParam* offset) :
    isHostClockedParam(isHostClocked), freqParam(freq), ratioParam(ratio), ampParam(amp), offsetParam(offset) {}
  void process();
  double getVal();
  static double getRatio(KLfoTimes time);
  void updateParams(double sampleRate, double samplesPerBeat, double samplePos);

private:
  iplug::IParam* isHostClockedParam = nullptr;
  iplug::IParam* freqParam = nullptr;
  iplug::IParam* ratioParam = nullptr;
  iplug::IParam* ampParam = nullptr;
  iplug::IParam* offsetParam = nullptr;
  double wavelength = 1;
  double amp = 1;
  double offset = 0;
  double phase = 0;
};

class VuMeter
{
public:
  VuMeter();
  void process(double x);
  std::atomic<double> Voltage;
  void store();

private:
  const double kCharge = 0.2;
  const double kDischarge = 0.00007;
  double mValue = 0.0;

};




class Shaper
{
public:
  Shaper(iplug::IParam* pre, iplug::IParam* post, iplug::IParam* mix, iplug::IParam* wavetype, iplug::IParam* clip,
    iplug::IParam* preGainLfo, iplug::IParam* postGainLfo, iplug::IParam* mixLfo, iplug::IParam* clipLfo, Lfo* lfoPtr);
  double process(double x);
  void updateParams();


  Lfo* lfo = nullptr;
private:
  std::function<double(double)> algo = myAlgos.arctan;
  Waveform waveType = ARCTAN;
  iplug::IParam* pregainParam = nullptr;
  iplug::IParam* postgainParam = nullptr;
  iplug::IParam* mixParam = nullptr;
  iplug::IParam* waveParam = nullptr;
  iplug::IParam* clipParam = nullptr;

  iplug::IParam* preGainLfoParam = nullptr;
  iplug::IParam* postGainLfoParam = nullptr;
  iplug::IParam* mixLfoParam = nullptr;
  iplug::IParam* clipLfoParam = nullptr;

  Algos myAlgos;

  double pregain = 1;
  double pregainLfo = 0;
  double postgain = 1;
  double postgainLfo = 0;
  double mix = 0;
  double mixLfo = 0;
  double clip = 3.0;
  double clipLfo = 0; 

  Waveform wave = ARCTAN;
};

