
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string_view>

#include <../portaudio/include/portaudio.h>

static const constexpr double fs = 44100.;

struct audio_state {
   int phase_l{};
   int phase_r{};
};

float sine_wave(int phase, float frequency)
{
  return sin(2 * M_PI * frequency * phase / fs);
}
float square_wave(int phase, float frequency)
{
  return sine_wave(phase, frequency) > 0.f ? 1.f : -1.f;
}
float triangle_wave(int phase, float frequency)
{
  return asin(sine_wave(phase, frequency));
}
float sawtooth_wave(int phase, float frequency)
{
  return atan(sine_wave(phase, frequency));
}

static int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
  using namespace std;
  float** out = (float**)outputBuffer;
  float* out_l = out[0];
  float* out_r = out[1];

  audio_state& state = *reinterpret_cast<audio_state*>(userData);

  for(unsigned long i = 0; i < framesPerBuffer; i++)
  {
      out_l[i] = 0.5 * square_wave(state.phase_l++, 200);
      out_r[i] = 0.7 * sawtooth_wave(state.phase_r++, 480);
  }

  return paContinue;
}

int get_default_device()
{
  using namespace std::literals;
  for (int i = 0; i < Pa_GetDeviceCount(); i++)
  {
    auto info = Pa_GetDeviceInfo(i);
    std::cerr << info->name << std::endl;
    if(info->name == "pulse"sv)
      return i;
  }
  return Pa_GetDefaultOutputDevice();
}

int main(void)
{
  audio_state state;

  Pa_Initialize();

  PaStreamParameters outputParameters{};
  outputParameters.device = get_default_device();
  outputParameters.channelCount = 2;
  outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
  outputParameters.hostApiSpecificStreamInfo = nullptr;

  PaStream *stream{};
  Pa_OpenStream(&stream, nullptr,  &outputParameters,
                 48000, 128, paClipOff, audio_callback, &state);
  Pa_StartStream(stream);

  Pa_Sleep(15000);

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  return 0;
}