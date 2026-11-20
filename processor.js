class ThereminProcessor extends AudioWorkletProcessor {
    constructor() {
        super();
        this.exports = null;
        this.outPtr = 0;
        this.capacity = 1024;   // samples reserved in WASM memory (normal quantum is 128)
        this.view = null;

        this.freq = 440.0;
        this.amp = 0.0;
        this.drive = 3.5;

        this.port.onmessage = (e) => {
            if (e.data.type === 'wasm') {
                // Proxy: satisfies any WASI import by returning 0
                const dummyWASI = new Proxy({}, { get: () => () => 0 });
                const importObject = {
                    env: { emscripten_resize_heap: () => false },
                    wasi_snapshot_preview1: dummyWASI
                };

                WebAssembly.instantiate(e.data.wasmBytes, importObject)
                    .then(module => {
                        const ex = module.instance.exports;
                        this.outPtr = ex.malloc(this.capacity * 4);
                        this.exports = ex;
                        console.log("WASM loaded. Ready to play!");
                    })
                    .catch(err => console.error("Error instanciando WASM:", err));

            } else if (e.data.type === 'controls') {
                this.freq = e.data.freq;
                this.amp = e.data.amp;
            }
        };
    }

    // Recreate the view if WASM memory grew (the old buffer becomes invalid)
    getView() {
        const buf = this.exports.memory.buffer;
        if (!this.view || this.view.buffer !== buf) {
            this.view = new Float32Array(buf, this.outPtr, this.capacity);
        }
        return this.view;
    }

    process(inputs, outputs) {
        if (!this.exports) return true;

        const output = outputs[0];
        const n = output[0].length;
        if (n > this.capacity) return true;

        // sampleRate is global in the AudioWorkletGlobalScope
        this.exports.processAudio(this.outPtr, n, this.freq, this.amp, this.drive, sampleRate);

        const src = this.getView().subarray(0, n);
        for (const ch of output) ch.set(src);   // mono -> all channels
        return true;
    }
}

registerProcessor('theremin-processor', ThereminProcessor);