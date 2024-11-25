#include "waveProcessor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#define GETPARAMS(i) \
GetParam(kPreGain##i)->InitDouble("Pre Gain", 100., 0., 1000.0, 0.01, "%"); \
GetParam(kPostGain##i)->InitDouble("Post Gain", 100., 0., 400.0, 0.01, "%");\
GetParam(kClip##i)->InitDouble("Clip", 300., 0., 500.0, 0.01, "%", 0, "", IParam::ShapePowCurve(3.));\
GetParam(kWaveType##i)->InitEnum("Wave Type", TANH, 12); \
GetParam(kWaveType##i)->SetDisplayText(0, "Arctan"); \
GetParam(kWaveType##i)->SetDisplayText(1, "TANH"); \
GetParam(kWaveType##i)->SetDisplayText(2, "TANH^3"); \
GetParam(kWaveType##i)->SetDisplayText(3, "SIN"); \
GetParam(kWaveType##i)->SetDisplayText(4, "SIN^3"); \
GetParam(kWaveType##i)->SetDisplayText(5, "SIGMOID"); \
GetParam(kWaveType##i)->SetDisplayText(6, "TANH(TAN)"); \
GetParam(kWaveType##i)->SetDisplayText(7, "TANH(TAN^3)"); \
GetParam(kWaveType##i)->SetDisplayText(8, "TANH(EXP)"); \
GetParam(kWaveType##i)->SetDisplayText(9, "SIN(TAN)"); \
GetParam(kWaveType##i)->SetDisplayText(10, "SIN(TAN^3)"); \
GetParam(kWaveType##i)->SetDisplayText(11, "SIN(EXP)"); \
GetParam(kMix##i)->InitDouble("Mix", 50., 0., 100.0, 0.01, "");\
GetParam(kPreGainLfo##i)->InitDouble("", 0., -100., 100.0, 0.01, "");\
GetParam(kPostGainLfo##i)->InitDouble("", 0., -100., 100.0, 0.01, "");\
GetParam(kMixLfo##i)->InitDouble("", 0., -100., 100.0, 0.01, "");\
GetParam(kClipLfo##i)->InitDouble("", 0., -100., 100.0, 0.01, "");

#define CONNECTGRAPHICS(x, y) \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)        .GetHShifted(-230).GetVShifted(y), kPreGain##x,"", myStyle));\
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(20)        .GetHShifted(-230).GetVShifted(y + 70), kPreGainLfo##x, "", minimalKnobStyle));\
pGraphics->AttachControl(new IVMenuButtonControl(b.GetCentredInside(100).GetHShifted(-130).GetVShifted(y), kWaveType##x, "Wave Type", myStyle)); \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)        .GetHShifted(-30).GetVShifted(y), kPostGain##x, "", myStyle));  \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(20)        .GetHShifted(-30).GetVShifted(y + 70), kPostGainLfo##x, "", minimalKnobStyle));\
pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(100).GetHPadded(-30).GetHShifted(30).GetVShifted(y), kClip##x, "Clip", myStyle));\
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(20)        .GetHShifted(30).GetVShifted(y + 70), kClipLfo##x, "", minimalKnobStyle));\
pGraphics->AttachControl(new dynamicPlot(b.GetCentredInside(100).GetHShifted(100).GetVShifted(y).GetPadded(-5), [](double i) -> double { return tanh(2. * i - 1.); }), kCtrlTagPlot##x);               \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)        .GetHShifted(200).GetVShifted(y), kMix##x, "", myStyle));\
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(20)        .GetHShifted(200).GetVShifted(y + 70), kMixLfo##x, "", minimalKnobStyle));\
pGraphics->AttachControl(new VuMeterControl(b.GetCentredInside(100)     .GetHShifted(250) .GetVShifted(y).GetVPadded(-5).GetHPadded(-45), COLOR_BLACK, kCtrlTagVUMeter##x), kCtrlTagVUMeter##x); \

waveProcessor::waveProcessor(const InstanceInfo& info)
  : iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GETPARAMS(0);
  GETPARAMS(1);
  GETPARAMS(2);
  GetParam(kParallel)->InitBool("Parallel", false);
  GetParam(kLfoIsHostClocked)->InitBool("", false);
  GetParam(kLfoAmp)->InitDouble("Amp", 0., -10., 10., 0.01, "");
  GetParam(kLfoFreq)->InitDouble("Freq", 1., 0.01, 20., 0.01, "Hz");  
  GetParam(kLfoOffset)->InitDouble("Offset", 0., -1., 1., 0.01, "* λ");
  GetParam(kLfoRatio)->InitEnum("Ratio", 0, lfo_times.size());
  for (int i = 0; i < lfo_times.size(); i++)
  {
    GetParam(kLfoRatio)->SetDisplayText(i, lfo_times[i].c_str());
  }

  //Presets
  MakePreset( "Default",
    100., 100., TANH, 100., 500., 0., 0., 0., 0.,
    100., 100., TANH, 0., 500., 0., 0., 0., 0.,
    100., 100., TANH, 0., 500., 0., 0., 0., 0.,
    false, false, 1., t1beat, 0., 0.);
  MakePreset("Saturator",
    200., 100., TANH, 100., 500., 0., 0., 0., 0.,
    100., 100., TANH, 0., 500., 0., 0., 0., 0.,
    100., 100., TANH, 0., 500., 0., 0., 0., 0.,
    false, false, 1., t1beat, 0., 0.);
  MakePreset("Unstable Wave",
    180., 100., TANH, 100., 500., 25., 0., 0., 0.,
    287., 100., SIGMOID, 75., 500., -40., 0., 0., 0.,
    152., 70., TANH, 60., 300., 0., 0., 0., 0.,
    false, true, 1., t1beat, 1.85, 0.);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
    mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT)); };

    mLayoutFunc = [&](IGraphics* pGraphics) {
      const IColor bgColor = IColor(255, 0, 90, 179);
      pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
      pGraphics->AttachPanelBackground(bgColor);
      pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
      const IRECT b = pGraphics->GetBounds();
      const IVColorSpec mySpec = IVColorSpec(std::initializer_list<IColor>{bgColor, bgColor, bgColor, COLOR_BLACK, COLOR_BLUE, COLOR_BLACK});
      const IVStyle myStyle = IVStyle(true, true, mySpec, DEFAULT_LABEL_TEXT, DEFAULT_VALUE_TEXT, false, true, false, false, 0.2f, 1.f, 0.f, 1.f, 0.f);
      IVStyle minimalKnobStyle = IVStyle(myStyle);
      minimalKnobStyle.showValue = false;
      pGraphics->AttachControl(new IVToggleControl(b.GetCentredInside(120).GetVPadded(-30).GetVShifted(-210).GetHShifted(-200), kParallel, "Processing Mode", myStyle, "Serial", "Parallel"));
      pGraphics->AttachControl(new IVToggleControl(b.GetCentredInside(50).GetVShifted(-205).GetHShifted(50), kLfoIsHostClocked, "", myStyle, "Free", "Sync"), -1, "LFO");
      pGraphics->AttachControl(new LfoModuleControl(b.GetCentredInside(60).GetVShifted(-210).GetHShifted(110), myStyle, kLfoFreq, kLfoRatio), kCtrlTagLfo, "LFO");
      pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(60).GetVShifted(-210).GetHShifted(160), kLfoAmp, "", myStyle), -1, "LFO");
      pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(60).GetVShifted(-210).GetHShifted(210), kLfoOffset, "", myStyle), -1, "LFO");
      pGraphics->AttachControl(new IVGroupControl("LFO", "LFO", 10., 10., 10., 10., myStyle));
      CONNECTGRAPHICS(0, -110);
      CONNECTGRAPHICS(1, 30);
      CONNECTGRAPHICS(2, 170);
    };
#endif

}
  

#define SENDMESSAGES(i) \
paramDVal = GetParam(kPreGain##i)->Value() + lfoVal * GetParam(kPreGainLfo##i)->Value();\
SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotIn, sizeof(double), &paramDVal);\
paramDVal = GetParam(kPostGain##i)->Value() + lfoVal * GetParam(kPostGainLfo##i)->Value();\
SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotOut, sizeof(double), &paramDVal);\
paramWVal = (Waveform)GetParam(kWaveType##i)->Value(); \
SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotWaveType, sizeof(Waveform), &paramWVal);  \
paramDVal = GetParam(kMix##i)->Value() + std::max(std::min(lfoVal * GetParam(kMixLfo##i)->Value(), 100.), 0.);\
SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotMix, sizeof(double), &paramDVal);\
paramDVal = GetParam(kClip##i)->Value() + std::max(0.,lfoVal * GetParam(kClipLfo##i)->Value());\
SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotClip, sizeof(double), &paramDVal);


void waveProcessor::OnIdle()
{
  SendControlValueFromDelegate(kCtrlTagVUMeter0, mVuMeters[0]->Voltage.load());
  SendControlValueFromDelegate(kCtrlTagVUMeter1, mVuMeters[1]->Voltage.load());
  SendControlValueFromDelegate(kCtrlTagVUMeter2, mVuMeters[2]->Voltage.load());
  double paramDVal = 0;
  double lfoVal = mLfo.getVal();
  Waveform paramWVal = TANH;
  SENDMESSAGES(0);
  SENDMESSAGES(1);
  SENDMESSAGES(2);
  paramDVal = GetParam(kLfoIsHostClocked)->Bool();
  SendControlValueFromDelegate(kCtrlTagLfo, paramDVal);
}


#if IPLUG_DSP
void waveProcessor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  mShaper[0].updateParams();
  mShaper[1].updateParams();
  mShaper[2].updateParams();
  mLfo.updateParams(GetSampleRate(), GetSamplesPerBeat(), GetSamplePos());
  const int nChans = NOutChansConnected();
  iplug::sample temp = 0.0;
  iplug::sample temp2 = 0.0;
  iplug::sample temp3 = 0.0;
  if (!GetParam(kParallel)->Value())
  {
    for (int s = 0; s < nFrames; s++)
    {
      mLfo.process();
      for (int c = 0; c < nChans; c++)
      {
        temp = inputs[c][s];
        temp = mShaper[0].process(temp);
        mVuMeters[0]->process(temp);
        temp = mShaper[1].process(temp);
        mVuMeters[1]->process(temp);
        temp = mShaper[2].process(temp);
        mVuMeters[2]->process(temp);
        outputs[c][s] = temp;
      }
    }
  }
  else {
    for (int s = 0; s < nFrames; s++)
    {
      mLfo.process();
      for (int c = 0; c < nChans; c++)
      {
        temp = inputs[c][s];
        temp2 = mShaper[1].process(temp);
        mVuMeters[1]->process(temp2);
        temp3 = mShaper[2].process(temp);
        mVuMeters[2]->process(temp3);
        temp = mShaper[0].process(temp);
        mVuMeters[0]->process(temp);
        temp = (temp + temp2 + temp3) / 3.; 
        outputs[c][s] = temp;
      }

    }
  }
  mVuMeters[0]->store();
  mVuMeters[1]->store();
  mVuMeters[2]->store();
}

#endif
