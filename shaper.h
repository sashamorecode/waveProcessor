#pragma once
#include "IPlug_include_in_plug_hdr.h"
enum waveform
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

struct algos
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
  std::function<double(double)> getAlgo(waveform wave) {
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

class shaper
{
public:
  shaper(iplug::IParam* pre, iplug::IParam* post, iplug::IParam* mix, iplug::IParam* wavetype) :
    pregainParam(pre), postgainParam(post), mixParam(mix), waveParam(wavetype) {}
  double process(double x){
     return algo(x * pregain) * postgain*mix + x * (1. - mix);
  }
   void updateParams() {
     pregain = pregainParam->Value() / 100.;
     postgain = postgainParam->Value() / 100.;
     mix = mixParam->Value() / 100.;
     wave = static_cast<waveform>(waveParam->Int());
     algo = myAlgos.getAlgo(wave);
    } 
private:
  std::function<double(double)> algo = myAlgos.arctan;
  waveform waveType = ARCTAN;
  iplug::IParam* pregainParam = nullptr;
  iplug::IParam* postgainParam = nullptr;
  iplug::IParam* mixParam = nullptr;
  iplug::IParam* waveParam = nullptr;
  algos myAlgos;
  double pregain = 1;
  double postgain = 1;
  double mix = 0;
  waveform wave = ARCTAN;
};
