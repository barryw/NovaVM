const EVENT_STRIDE = 5;
const EVENT_SID1_WRITE = 1;
const EVENT_SID2_WRITE = 2;
const EVENT_WTS_NOTE_ON = 16;
const EVENT_WTS_NOTE_OFF = 17;
const EVENT_WTS_VOLUME = 18;
const EVENT_WTS_PANNING = 19;
const EVENT_WTS_PITCH_BEND = 20;
const EVENT_WTS_ALL_NOTES_OFF = 21;
const EVENT_WTS_MASTER_VOLUME = 22;
const EVENT_WTS_RESET_EFFECTS = 23;
const EVENT_RESET = 31;

class NovaAudioProcessor extends AudioWorkletProcessor {
    constructor(options) {
        super();

        const processorOptions = options.processorOptions || {};
        this.cpuHz = clampNumber(processorOptions.cpuHz || 1000000, 100000, 12000000);
        this.cpuCyclesPerSample = this.cpuHz / sampleRate;
        this.prebufferCycles = this.cpuHz * 0.05;
        this.cpuCycle = 0;
        this.started = false;
        this.events = [];
        this.eventIndex = 0;

        this.sid1 = new SidChip(sampleRate);
        this.sid2 = new SidChip(sampleRate);
        this.wts = new WtsSynth(sampleRate);

        this.port.onmessage = event => this.handleMessage(event.data || {});
    }

    handleMessage(message) {
        switch (message.type) {
            case "configure":
                this.cpuHz = clampNumber(message.cpuHz || this.cpuHz, 100000, 12000000);
                this.cpuCyclesPerSample = this.cpuHz / sampleRate;
                this.prebufferCycles = this.cpuHz * 0.05;
                break;
            case "events":
                this.enqueueEvents(message.events);
                break;
            case "reset":
                this.reset();
                break;
        }
    }

    enqueueEvents(eventData) {
        if (!eventData || eventData.length < EVENT_STRIDE) return;

        for (let i = 0; i + EVENT_STRIDE - 1 < eventData.length; i += EVENT_STRIDE) {
            const lo = eventData[i] >>> 0;
            const hi = eventData[i + 1] >>> 0;
            const cycle = lo + hi * 4294967296;
            this.events.push(cycle, eventData[i + 2] | 0, eventData[i + 3] | 0, eventData[i + 4] | 0);
        }
    }

    reset() {
        this.sid1.reset();
        this.sid2.reset();
        this.wts.reset();
        this.events.length = 0;
        this.eventIndex = 0;
        this.started = false;
        this.cpuCycle = 0;
    }

    process(_inputs, outputs) {
        const output = outputs[0];
        const left = output[0];
        const right = output.length > 1 ? output[1] : output[0];

        this.ensureStarted();

        for (let i = 0; i < left.length; i++) {
            this.applyDueEvents();

            const sidSample = (this.sid1.renderSample() + this.sid2.renderSample()) * 0.42;
            const wtsSample = this.wts.renderSample();
            const mixedLeft = clampAudio(sidSample + wtsSample.left);
            const mixedRight = clampAudio(sidSample + wtsSample.right);

            left[i] = mixedLeft;
            right[i] = mixedRight;
            this.cpuCycle += this.cpuCyclesPerSample;
        }

        this.compactEvents();
        return true;
    }

    ensureStarted() {
        if (this.started) return;
        if (this.eventIndex < this.events.length) {
            this.cpuCycle = Math.max(0, this.events[this.eventIndex] - this.prebufferCycles);
            this.started = true;
        }
    }

    applyDueEvents() {
        while (this.eventIndex < this.events.length && this.events[this.eventIndex] <= this.cpuCycle) {
            const kind = this.events[this.eventIndex + 1];
            const arg0 = this.events[this.eventIndex + 2];
            const arg1 = this.events[this.eventIndex + 3];
            this.applyEvent(kind, arg0, arg1);
            this.eventIndex += 4;
        }
    }

    applyEvent(kind, arg0, arg1) {
        switch (kind) {
            case EVENT_SID1_WRITE:
                this.sid1.write(arg0, arg1);
                break;
            case EVENT_SID2_WRITE:
                this.sid2.write(arg0, arg1);
                break;
            case EVENT_WTS_NOTE_ON:
            {
                const value0 = arg1 & 0xFFFF;
                const value1 = (arg1 >>> 16) & 0xFFFF;
                this.wts.noteOn(arg0, value0 & 0xFF, (value0 >>> 8) & 0xFF, value1);
                break;
            }
            case EVENT_WTS_NOTE_OFF:
                this.wts.noteOff(arg0);
                break;
            case EVENT_WTS_VOLUME:
                this.wts.setVolume(arg0, arg1 & 0xFF);
                break;
            case EVENT_WTS_PANNING:
                this.wts.setPanning(arg0, arg1 & 0xFF);
                break;
            case EVENT_WTS_PITCH_BEND:
                this.wts.setPitchBend(arg0, arg1 & 0xFFFF);
                break;
            case EVENT_WTS_ALL_NOTES_OFF:
                this.wts.allNotesOff();
                break;
            case EVENT_WTS_MASTER_VOLUME:
                this.wts.masterVolume = (arg1 & 0xFF) / 255;
                break;
            case EVENT_WTS_RESET_EFFECTS:
                this.wts.masterVolume = 1;
                break;
            case EVENT_RESET:
                this.sid1.reset();
                this.sid2.reset();
                this.wts.reset();
                break;
        }
    }

    compactEvents() {
        if (this.eventIndex < 4096) return;
        this.events.splice(0, this.eventIndex);
        this.eventIndex = 0;
    }
}

class SidChip {
    constructor(outputSampleRate) {
        this.outputSampleRate = outputSampleRate;
        this.sidCyclesPerSample = 985248 / outputSampleRate;
        this.registers = new Uint8Array(32);
        this.channels = [new SidChannel(), new SidChannel(), new SidChannel()];

        this.channels[0].syncTarget = this.channels[1];
        this.channels[1].syncTarget = this.channels[2];
        this.channels[2].syncTarget = this.channels[0];
        this.channels[0].syncSource = this.channels[2];
        this.channels[1].syncSource = this.channels[0];
        this.channels[2].syncSource = this.channels[1];

        this.reset();
    }

    reset() {
        this.registers.fill(0);
        this.registers[0x1D] = 0x0F;
        this.registers[0x1E] = 0x0F;
        this.registers[0x1F] = 0x0F;
        for (const channel of this.channels) channel.reset();
        this.cycleAccumulator = 0;
        this.prevBandPass = 0;
        this.prevLowPass = 0;
    }

    write(register, value) {
        if (register < 0 || register >= 32) return;
        this.registers[register] = value & 0xFF;
    }

    renderSample() {
        this.readRegistersIntoChannels();

        let cyclesToRun = Math.ceil(this.sidCyclesPerSample - this.cycleAccumulator);
        if (cyclesToRun < 1) cyclesToRun = 1;

        for (const channel of this.channels) channel.clock(cyclesToRun);
        for (const channel of this.channels) {
            if (channel.doSync && (channel.syncTarget.waveform & 0x02) !== 0)
                channel.syncTarget.resetAccumulator();
        }

        this.cycleAccumulator += cyclesToRun;
        if (this.cycleAccumulator >= this.sidCyclesPerSample)
            this.cycleAccumulator -= this.sidCyclesPerSample;

        const masterVol = (this.registers[0x18] & 0x0F) / 22.5;
        const filterSelect = this.registers[0x18] & 0x70;
        const filterCtrl = this.registers[0x17];
        let cutoff = 0.05 + 0.85 * (Math.sin((this.registers[0x16] / 255.0 - 0.5) * Math.PI) * 0.5 + 0.5);
        cutoff = Math.pow(cutoff, 1.3);
        const resonance = this.registers[0x17] > 0x3F ? 7.0 / (this.registers[0x17] >> 4) : 1.75;

        return clampAudio(this.mixSample(filterCtrl, filterSelect, cutoff, resonance, masterVol));
    }

    readRegistersIntoChannels() {
        for (let i = 0; i < 3; i++) {
            const ofs = i * 7;
            const channel = this.channels[i];
            channel.frequency = this.registers[ofs] | (this.registers[ofs + 1] << 8);
            channel.pulse = this.registers[ofs + 2] | (this.registers[ofs + 3] << 8);
            channel.waveform = this.registers[ofs + 4];
            channel.ad = this.registers[ofs + 5];
            channel.sr = this.registers[ofs + 6];
            channel.voiceVolume = this.registers[0x1D + i];
        }
    }

    mixSample(filterCtrl, filterSelect, cutoff, resonance, masterVol) {
        let output = 0;
        let filterInput = 0;

        for (let ch = 0; ch < 3; ch++) {
            const channel = this.channels[ch];
            const chOut = channel.getOutput() * ((channel.voiceVolume & 0x0F) / 15);
            if ((filterCtrl & (1 << ch)) === 0)
                output += chOut;
            else
                filterInput += chOut;
        }

        let temp = filterInput + this.prevBandPass * resonance + this.prevLowPass;
        if ((filterSelect & 0x40) !== 0)
            output -= temp;

        temp = this.prevBandPass - temp * cutoff;
        this.prevBandPass = temp;
        if ((filterSelect & 0x20) !== 0)
            output -= temp;

        temp = this.prevLowPass + temp * cutoff;
        this.prevLowPass = temp;
        if ((filterSelect & 0x10) !== 0)
            output += temp;

        return output * masterVol;
    }
}

class SidChannel {
    constructor() {
        this.syncTarget = null;
        this.syncSource = null;
        this.adsrRateTable = [9, 32, 63, 95, 149, 220, 267, 313, 392, 977, 1954, 3126, 3907, 11720, 19532, 31251];
        this.sustainLevels = [0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF];
        this.expTargetTable = [
            1, 30, 30, 30, 30, 30, 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
        ];
        this.reset();
    }

    reset() {
        this.frequency = 0;
        this.ad = 0;
        this.sr = 0;
        this.pulse = 0;
        this.waveform = 0;
        this.voiceVolume = 0x0F;
        this.doSync = false;
        this.state = 2;
        this.accumulator = 0;
        this.noiseGenerator = 0x7FFFF8;
        this.adsrCounter = 0;
        this.adsrExpCounter = 0;
        this.volumeLevel = 0;
    }

    resetAccumulator() {
        this.accumulator = 0;
    }

    clock(cycles) {
        if ((this.waveform & 0x01) !== 0) {
            if (this.state === 2)
                this.state = 0;
        } else {
            this.state = 2;
        }

        let adsrCycles = cycles;
        while (adsrCycles > 0) {
            const adsrTarget = this.state === 0
                ? this.adsrRateTable[this.ad >> 4]
                : this.state === 1
                    ? this.adsrRateTable[this.ad & 0x0F]
                    : this.adsrRateTable[this.sr & 0x0F];

            const cyclesUntilTarget = this.adsrCounter < adsrTarget
                ? adsrTarget - this.adsrCounter
                : 0x8000 + adsrTarget - this.adsrCounter;
            const adsrCyclesNow = Math.min(adsrCycles, cyclesUntilTarget);
            this.adsrCounter = (this.adsrCounter + adsrCyclesNow) & 0x7FFF;

            if (this.adsrCounter === adsrTarget) {
                this.adsrCounter = 0;

                if (this.state === 0) {
                    this.adsrExpCounter = 0;
                    this.volumeLevel = (this.volumeLevel + 1) & 0xFF;
                    if (this.volumeLevel === 0xFF)
                        this.state = 1;
                } else if (this.state === 1) {
                    const target = this.volumeLevel < 0x5D ? this.expTargetTable[this.volumeLevel] : 1;
                    this.adsrExpCounter++;
                    if (this.adsrExpCounter >= target) {
                        this.adsrExpCounter = 0;
                        if (this.volumeLevel > this.sustainLevels[this.sr >> 4])
                            this.volumeLevel--;
                    }
                } else if (this.volumeLevel > 0) {
                    const target = this.volumeLevel < 0x5D ? this.expTargetTable[this.volumeLevel] : 1;
                    this.adsrExpCounter++;
                    if (this.adsrExpCounter >= target) {
                        this.adsrExpCounter = 0;
                        this.volumeLevel--;
                    }
                }
            }

            adsrCycles -= adsrCyclesNow;
        }

        if ((this.waveform & 0x08) !== 0) {
            this.accumulator = 0;
            return;
        }

        if (this.frequency === 0)
            return;

        if ((this.waveform & 0x80) === 0 && (this.syncTarget.waveform & 0x02) === 0) {
            this.accumulator = (this.accumulator + this.frequency * cycles) & 0xFFFFFF;
            return;
        }

        let accumulatorCycles = cycles;
        while (accumulatorCycles > 0) {
            let accumulatorCyclesNow = accumulatorCycles;

            if ((this.waveform & 0x80) !== 0) {
                if ((this.accumulator & 0xFFFFF) < 0x80000)
                    accumulatorCyclesNow = Math.min(accumulatorCyclesNow, Math.floor((0x80000 - (this.accumulator & 0xFFFFF)) / this.frequency) + 1);
                else
                    accumulatorCyclesNow = Math.min(accumulatorCyclesNow, Math.floor((0x180000 - (this.accumulator & 0xFFFFF)) / this.frequency) + 1);
            }

            if ((this.syncTarget.waveform & 0x02) !== 0) {
                if (this.accumulator < 0x800000)
                    accumulatorCyclesNow = Math.min(accumulatorCyclesNow, Math.floor((0x800000 - this.accumulator) / this.frequency) + 1);
                else
                    accumulatorCyclesNow = Math.min(accumulatorCyclesNow, Math.floor((0x1800000 - this.accumulator) / this.frequency) + 1);
            }

            const lastAccumulator = this.accumulator;
            this.accumulator = (this.accumulator + this.frequency * accumulatorCyclesNow) & 0xFFFFFF;

            if ((this.waveform & 0x80) !== 0 &&
                (lastAccumulator & 0x80000) === 0 &&
                (this.accumulator & 0x80000) !== 0) {
                let temp = this.noiseGenerator;
                const step = (temp & 0x400000) ^ ((temp & 0x20000) << 5);
                temp <<= 1;
                if (step > 0) temp |= 1;
                this.noiseGenerator = temp & 0x7FFFFF;
            }

            this.doSync = (lastAccumulator & 0x800000) === 0 && (this.accumulator & 0x800000) !== 0;
            accumulatorCycles -= accumulatorCyclesNow;
        }
    }

    getOutput() {
        if (this.volumeLevel === 0)
            return 0;

        let waveOutput = 0;
        switch (this.waveform & 0xF0) {
            case 0x10:
                waveOutput = this.triangle();
                break;
            case 0x20:
                waveOutput = this.sawtooth();
                break;
            case 0x40:
                waveOutput = this.pulseWave();
                break;
            case 0x50:
            {
                const tri = this.triangle();
                const pul = this.pulseWave();
                waveOutput = ((pul & tri & (tri >> 1)) & (tri << 1)) << 1;
                if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                break;
            }
            case 0x60:
            {
                const saw = this.sawtooth();
                const pul = this.pulseWave();
                waveOutput = ((pul & saw & (saw >> 1)) & (saw << 1)) << 1;
                if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                break;
            }
            case 0x70:
            {
                const triSaw = this.triangle() & this.sawtooth();
                const pul = this.pulseWave();
                waveOutput = ((pul & triSaw & (triSaw >> 1)) & (triSaw << 1)) << 1;
                if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                break;
            }
            case 0x80:
                waveOutput = this.noise();
                break;
        }

        return ((waveOutput | 0) - 0x8000) * this.volumeLevel / 16777216;
    }

    triangle() {
        const temp = this.accumulator ^ ((this.waveform & 0x04) !== 0 ? this.syncSource.accumulator : 0);
        return ((temp >= 0x800000 ? (this.accumulator ^ 0xFFFFFF) : this.accumulator) >> 7) & 0xFFFF;
    }

    sawtooth() {
        return this.accumulator >> 8;
    }

    pulseWave() {
        return ((this.accumulator >> 12) >= (this.pulse & 0x0FFF)) ? 0xFFFF : 0;
    }

    noise() {
        const n = this.noiseGenerator;
        return ((n & 0x100000) >> 5) +
            ((n & 0x40000) >> 4) +
            ((n & 0x4000) >> 1) +
            ((n & 0x800) << 1) +
            ((n & 0x200) << 2) +
            ((n & 0x20) << 5) +
            ((n & 0x04) << 7) +
            ((n & 0x01) << 8);
    }
}

class WtsSynth {
    constructor(outputSampleRate) {
        this.outputSampleRate = outputSampleRate;
        this.voices = Array.from({ length: 8 }, () => new WtsVoice());
        this.masterVolume = 1;
    }

    reset() {
        for (const voice of this.voices) voice.reset();
        this.masterVolume = 1;
    }

    noteOn(voiceIndex, note, velocity, instrument) {
        const voice = this.voices[voiceIndex & 7];
        voice.note = note;
        voice.velocity = Math.max(1, velocity) / 127;
        voice.instrument = instrument & 0xFFFF;
        voice.phase = 0;
        voice.envelope = 0;
        voice.active = true;
        voice.releasing = false;
    }

    noteOff(voiceIndex) {
        const voice = this.voices[voiceIndex & 7];
        voice.releasing = true;
    }

    setVolume(voiceIndex, volume) {
        this.voices[voiceIndex & 7].volume = (volume & 0xFF) / 255;
    }

    setPanning(voiceIndex, pan) {
        this.voices[voiceIndex & 7].pan = (pan & 0xFF) / 255;
    }

    setPitchBend(voiceIndex, bend) {
        this.voices[voiceIndex & 7].pitchBend = bend & 0xFFFF;
    }

    allNotesOff() {
        for (const voice of this.voices) voice.releasing = true;
    }

    renderSample() {
        let left = 0;
        let right = 0;

        for (const voice of this.voices) {
            if (!voice.active) continue;

            const attackStep = 1 / (0.005 * this.outputSampleRate);
            const releaseStep = 1 / (0.09 * this.outputSampleRate);
            if (voice.releasing) {
                voice.envelope -= releaseStep;
                if (voice.envelope <= 0) {
                    voice.reset();
                    continue;
                }
            } else if (voice.envelope < 1) {
                voice.envelope = Math.min(1, voice.envelope + attackStep);
            }

            const bendSemitones = ((voice.pitchBend - 0x8000) / 8192) * 2;
            const frequency = 440 * Math.pow(2, (voice.note - 69 + bendSemitones) / 12);
            const value = oscillatorSample(voice.phase, voice.instrument) *
                voice.envelope *
                voice.velocity *
                voice.volume *
                this.masterVolume *
                0.18;

            const pan = voice.pan;
            left += value * Math.cos(pan * Math.PI / 2);
            right += value * Math.sin(pan * Math.PI / 2);

            voice.phase += frequency / this.outputSampleRate;
            voice.phase -= Math.floor(voice.phase);
        }

        return { left: clampAudio(left), right: clampAudio(right) };
    }
}

class WtsVoice {
    constructor() {
        this.reset();
        this.volume = 1;
        this.pan = 0.5;
        this.pitchBend = 0x8000;
    }

    reset() {
        this.active = false;
        this.releasing = false;
        this.note = 60;
        this.velocity = 0;
        this.instrument = 0;
        this.phase = 0;
        this.envelope = 0;
    }
}

function oscillatorSample(phase, instrument) {
    switch (instrument & 0x03) {
        case 1:
            return phase * 2 - 1;
        case 2:
            return phase < 0.5 ? 1 : -1;
        case 3:
            return 1 - Math.abs(phase * 4 - 2);
        default:
            return Math.sin(phase * Math.PI * 2);
    }
}

function clampAudio(value) {
    if (value > 1) return 1;
    if (value < -1) return -1;
    return value;
}

function clampNumber(value, min, max) {
    if (!Number.isFinite(value)) return min;
    return Math.max(min, Math.min(max, value));
}

registerProcessor("nova-audio-processor", NovaAudioProcessor);
