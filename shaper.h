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
  void init(iplug::IParam* pre, iplug::IParam* post, waveform wave, iplug::IParam* mix) 
  {
    pregain = pre;
    postgain = post;
    this->mix = mix;
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
  inline static double arctan(double x) { return atan(x); }
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

   double process(double x)
  {
    mixVal = (mix->Value() / 100.);
    return (algo(x * (pregain->Value()/100.)) * (postgain->Value()/100.)) * mixVal + x * (1. - mixVal);
  }

private:
  waveform waveType = ARCTAN;
  iplug::IParam* pregain = nullptr;
  iplug::IParam* postgain = nullptr;
  iplug::IParam* mix = nullptr;
  double (*algo)(double x) = nullptr;
  double mixVal = 1.;
};
