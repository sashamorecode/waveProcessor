#include "waveProcessor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#define GETPARAMS(i) GetParam(kPreGain##i)->InitDouble("Pre Gain", 100., 0., 1000.0, 0.01, "%"); \
GetParam(kPostGain##i)->InitDouble("Post Gain", 100., 0., 400.0, 0.01, "%");                                                                                                                       \
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
GetParam(kMix##i)->InitDouble("Mix", 50., 0., 100.0, 0.01, "%");

#define CONNECTGRAPHICS(x, y) \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-210).GetVShifted(y), kPreGain##x, "", myStyle));                                                                              \
  pGraphics->AttachControl(new IVMenuButtonControl(b.GetCentredInside(100).GetHShifted(-110).GetVShifted(y), kWaveType##x, "Wave Type", myStyle)); \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-10).GetVShifted(y), kPostGain##x, "", myStyle));                                                                              \
  pGraphics->AttachControl(new dynamicPlot(b.GetCentredInside(100)        .GetHShifted(90)  .GetVShifted(y), [](double i) -> double { return tanh(2.*i-1.); }), kCtrlTagPlot##x); \
pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(190).GetVShifted(y), kMix##x, "", myStyle));                                                                                 \
  pGraphics->AttachControl(new VuMeterControl(b.GetCentredInside(100)     .GetHShifted(245) .GetVShifted(y).GetVPadded(-5).GetHPadded(-45), COLOR_BLACK, kCtrlTagVUMeter##x), kCtrlTagVUMeter##x); \

waveProcessor::waveProcessor(const InstanceInfo& info)
  : iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GETPARAMS(0);
  GETPARAMS(1);
  GETPARAMS(2);
  GetParam(kParallel)->InitBool("Parallel", false);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT)); };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IColor bgColor = IColor(255, 0, 90, 179);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(bgColor);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    const IVColorSpec mySpec = IVColorSpec(std::initializer_list<IColor>{bgColor, bgColor, bgColor, COLOR_BLACK, COLOR_BLUE, COLOR_BLACK});
    const IVStyle myStyle =
      IVStyle(true, true, mySpec, DEFAULT_LABEL_TEXT, DEFAULT_VALUE_TEXT, false, true, false,false, 0.2f, 1.f, 0.f, 1.f, 0.f);
    pGraphics->AttachControl(new IVToggleControl(b.GetCentredInside(120).GetVPadded(-35).GetVShifted(-170), kParallel, "Processing Mode", myStyle, "Serial", "Parallel"));
    CONNECTGRAPHICS(0, -80); 
    CONNECTGRAPHICS(1, 20);
    CONNECTGRAPHICS(2, 120);
  };
#endif

}
  

#define SENDMESSAGES(i) \
paramDVal = GetParam(kPreGain##i)->Value();\
  SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotIn, sizeof(double), &paramDVal);\
  paramDVal = GetParam(kPostGain##i)->Value();\
  SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotOut, sizeof(double), &paramDVal);\
  paramWVal = (waveform)GetParam(kWaveType##i)->Value(); \
  SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotWaveType, sizeof(waveform), &paramWVal);  \
  paramDVal = GetParam(kMix##i)->Value();\
  SendControlMsgFromDelegate(kCtrlTagPlot##i, kPlotMix, sizeof(double), &paramDVal);


void waveProcessor::OnIdle()
{
  SendControlValueFromDelegate(kCtrlTagVUMeter0, mVuMeters[0]->Voltage.load());
  SendControlValueFromDelegate(kCtrlTagVUMeter1, mVuMeters[1]->Voltage.load());
  SendControlValueFromDelegate(kCtrlTagVUMeter2, mVuMeters[2]->Voltage.load());
  double paramDVal = 0;
  waveform paramWVal = TANH;
  SENDMESSAGES(0);
  SENDMESSAGES(1);
  SENDMESSAGES(2);
}


#if IPLUG_DSP
void waveProcessor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  mShaper[0].updateParams();
  mShaper[1].updateParams();
  mShaper[2].updateParams();

  const int nChans = NOutChansConnected();
  iplug::sample temp = 0.0;
  iplug::sample temp2 = 0.0;
  iplug::sample temp3 = 0.0;
  if (!GetParam(kParallel)->Value())
  {
    for (int s = 0; s < nFrames; s++)
    {
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
