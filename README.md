# Wasm Analog Theremin

A virtual theremin that runs in the browser. Your webcam tracks your hands, and a WebAssembly synthesizer turns their position into sound in real time. No hardware needed.

- **Right hand (Y axis):** pitch. Moving it sideways (X axis) adds vibrato.
- **Left hand (Y axis):** volume.

## Who it's for

- Musicians and curious people who want to play a theremin without buying one.
- Developers interested in a small example of MediaPipe hand tracking, AudioWorklet and WebAssembly working together.
- Anyone learning pitch and scales: the on-screen piano and note grid give visual feedback.

## Features

- Analog-style synth with vacuum-tube-like saturation, glide and DC blocking (C++ compiled to WASM).
- Tuning modes: Free (theremin), Chromatic, C Major and C Minor Pentatonic (default: C Major).
- Selectable range of 1, 2 or 3 octaves (default: 1) and octave shifting.
- Echo delay and hall reverb with sliders.
- Reference piano keyboard: click a key to hear the note.
- Low-latency tracking: One Euro filter, motion prediction and automatic brightness compensation for the camera.

Keyboard shortcuts: `P` toggles prediction, `G` toggles auto-gain.

## Built with

- HTML, CSS and vanilla JavaScript (ES modules)
- [MediaPipe Tasks Vision](https://developers.google.com/mediapipe) (Hand Landmarker), loaded from a CDN
- Web Audio API with an `AudioWorklet` (`processor.js`)
- C++ DSP (`theremin_dsp.cpp`) compiled to WebAssembly (`theremin.wasm`) with Emscripten

## Project structure

| File | Purpose |
| --- | --- |
| `index.html` | UI, hand tracking, effects chain and app logic |
| `processor.js` | AudioWorklet that loads and runs the WASM module |
| `theremin_dsp.cpp` | Synth DSP source |
| `theremin.wasm` | Compiled DSP module |

## How to run

Requirements: a modern browser (Chrome or Edge recommended), a webcam, speakers or headphones, and an internet connection (MediaPipe and its model are loaded from a CDN).

The app must be served over HTTP. Opening `index.html` directly won't work because of the AudioWorklet and `fetch` of the `.wasm` file. `localhost` counts as a secure context, so the camera works.

```bash
cd theremin
python3 -m http.server 8000
```

Then open <http://localhost:8000>, click **Turn On Theremin (Enable Audio)**, and allow camera access.

### Rebuilding the WASM (optional)

`theremin.wasm` is already included. If you change `theremin_dsp.cpp`, rebuild it with [Emscripten](https://emscripten.org/):

```bash
emcc theremin_dsp.cpp -O3 -s WASM=1 -s STANDALONE_WASM \
  -s EXPORTED_FUNCTIONS='["_processAudio", "_malloc"]' -o theremin.wasm
```

## TODO

### Latency
- [ ] Measure and display end-to-end latency (camera → audio) more accurately.
- [ ] Move hand tracking to a Web Worker with `OffscreenCanvas`.
- [ ] Tune the One Euro filter and prediction parameters per device.
- [ ] Use `latencyHint: 'interactive'` and evaluate a smaller AudioWorklet quantum where possible.

### Performance
- [ ] Run the reverb impulse response generation off the main thread.
- [ ] Cache the MediaPipe model and WASM files (service worker) for faster startup and offline use.
- [ ] Only redraw the note grid and HUD when needed; reduce canvas shadow cost.
- [ ] Render the piano keyboard efficiently (single canvas or SVG).

### Usability
- [ ] Show a loading state while the camera and model initialize.
- [ ] Clear error messages for denied camera permission, no camera, or unsupported browser.
- [ ] Let users choose the camera and swap which hand controls pitch and volume (left-handed mode).
- [ ] Choose the root note and more scales (minor, blues, modes).
- [ ] Persist settings in `localStorage`.
- [ ] Add a volume/pitch visual indicator and a master volume control.
- [ ] Keyboard and screen reader accessibility for the controls.

### UI
- [ ] Redesign layout and styling (consistent theme, spacing and typography).
- [ ] Polish the hand overlay and note grid.
- [ ] Move HUD debug info (fps, latency) into a toggleable panel.
- [ ] Add light/dark themes.

### Mobile
- [ ] Responsive layout for small screens and portrait orientation.
- [ ] Touch-friendly controls (larger buttons, sliders).
- [ ] Use the front camera and handle mobile camera constraints and lower resolutions.
- [ ] Optimize tracking cost for mobile CPUs/GPUs and handle iOS audio unlocking.
- [ ] PWA support (manifest and installability).
- [ ] Optional touch-only mode as a fallback when no hands are tracked.

### Tutorial
- [ ] First-run interactive tutorial (hand positions, pitch, volume, vibrato).
- [ ] Guided exercises, such as playing a simple melody on the C major scale.
- [ ] Tooltips for the controls and effects.
- [ ] A short "how it works" section or demo video.

### Other ideas
- [ ] Record audio output and export to WAV.
- [ ] MIDI output (and Web MIDI input for tuning reference).
- [ ] More waveforms and timbres, plus a filter or tremolo.
- [ ] Bundle MediaPipe locally (no CDN dependency).
- [ ] Add a build script/Makefile for the WASM module.
- [ ] Automated tests for DSP and quantization logic.
- [ ] Deploy a live demo (GitHub Pages).

## License

[MIT](LICENSE)
