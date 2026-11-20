#include <emscripten.h>
#include <math.h>

static float phase = 0.0f;
static float curMidi = 69.0f;   // glide is done in semitones (more natural than Hz)
static float curAmp = 0.0f;
static float dcX = 0.0f, dcY = 0.0f;   // DC blocker state

// Vacuum tube emulation: asymmetric saturation (generates even harmonics and some DC)
static inline float tubeDistortion(float input, float drive) {
    float x = input * drive;
    float out = (x > 0) ? (1.0f - expf(-x)) : (-1.0f + expf(x * 1.2f));
    return out * 0.6f;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void processAudio(float* output, int length, float targetFreq, float targetAmp,
                      float drive, float sr) {
        const float twoPi = 6.28318530718f;
        const float targetMidi = 69.0f + 12.0f * log2f(targetFreq / 440.0f);

        // Coefficients derived from time constants, independent of sample rate
        const float glide   = 1.0f - expf(-1.0f / (0.035f * sr));  // ~35 ms
        const float ampUp   = 1.0f - expf(-1.0f / (0.015f * sr));  // attack ~15 ms
        const float ampDown = 1.0f - expf(-1.0f / (0.060f * sr));  // release ~60 ms

        for (int i = 0; i < length; i++) {
            curMidi += (targetMidi - curMidi) * glide;
            curAmp  += (targetAmp - curAmp) * (targetAmp > curAmp ? ampUp : ampDown);

            float f = 440.0f * exp2f((curMidi - 69.0f) / 12.0f);
            phase += twoPi * f / sr;
            if (phase >= twoPi) phase -= twoPi;

            // Less drive at high pitches = less aliasing
            float d = drive * fminf(1.0f, 800.0f / f);
            float y = tubeDistortion(sinf(phase), d);

            // DC blocker (high-pass ~4 Hz): removes thumps when moving the volume
            float hp = y - dcX + 0.9995f * dcY;
            dcX = y; dcY = hp;

            output[i] = hp * curAmp;
        }
    }
}