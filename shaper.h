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

class shaper
{
public:
  shaper(iplug::IParam* pre, iplug::IParam* post, iplug::IParam* mix, iplug::IParam* wavetype) :
    pregainParam(pre), postgainParam(post), mixParam(mix), waveParam(wavetype) {}
  void init() 
  {
    switch (wave)
    {
    case ::ARCTAN:
      algo = arctan;
      break;
    case ::TANH:
      algo = tanh;
      break;
    case ::TANH3:
      algo = tanh3;
      break;
    case ::SIN:
      algo = sin;
      break;
    case ::SIN3:
      algo = sin3;
      break;
    case ::SIGMOID:
      algo = sigmoid;
      break;
    case ::TANHTAN:
      algo = tanhtan;
      break;
    case ::TANHTAN3:
      algo = tanhtan3;
      break;
    case ::TANHEXP:
      algo = tanhexp;
      break;
    case ::SINTAN:
      algo = sintan;
      break;
    case ::SINTAN3:
      algo = sintan3;
      break;
    case ::SINEXP:
      algo = sinexp;
      break;
    default:
      algo = arctan;
      assert(false);
      break;
    }
  }
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

   double process(double x){
     return algo(x * pregain) * postgain*mix + x * (1. - mix);
  }
   void updateParams() {
     pregain = pregainParam->Value() / 100.;
     postgain = postgainParam->Value() / 100.;
     mix = mixParam->Value() / 100.;
     wave = static_cast<waveform>(waveParam->Int());
     init();
    } 
private:
  std::function<double(double)> algo = tanh;
  waveform waveType = ARCTAN;
  iplug::IParam* pregainParam = nullptr;
  iplug::IParam* postgainParam = nullptr;
  iplug::IParam* mixParam = nullptr;
  iplug::IParam* waveParam = nullptr; 
  double pregain = 1;
  double postgain = 1;
  double mix = 0;
  waveform wave = ARCTAN;
};
