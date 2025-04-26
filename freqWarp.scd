s.boot;
(
var fftsize = 2048;  // more FFT bins might sound better?
var numFXBins = (fftsize/2);  // half of FFT size
~warpBuf = Buffer.alloc(s, numFXBins);
~spectBus = Bus.audio(s, 1);
z = Buffer.read(s, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
SynthDef(\freqWarp, {
	|rate = 1, buf|
	var in, chain;
	in = PlayBuf.ar(1, z, rate, loop: 1);
	Out.ar(~spectBus, in);
	chain = FFT(LocalBuf(fftsize), in);
	chain = PV_FreqWarp(chain, buf);
	Out.ar(0, IFFT(chain).dup); //tends to clips a bit
}).add;
)
(
var fftsize = 2048;
var numFXBins = (fftsize/2);
~bufFunc = {
	//e = Env([0, 1], [1], \lin).discretize(numFXBins).as(Array);//original
	//e = Env([0, 0.5, 1], [0.4, 0.6], [-1, 1]).discretize(numFXBins).as(Array);
	//e = Env([0.1, 0.3, 1], [1, 1], [-5, 5]).discretize(numFXBins).as(Array);
	e = Env([exprand(0.01, 0.013), exprand(0.99, 0.17), exprand(0.1, 0.21)], [exprand(0.5, 0.8), exprand(0.1, 0.9)], [1, 1]).discretize(numFXBins).as(Array);
	//e = Env.triangle(0.1).discretize(numFXBins).as(Array);
	e = (e * numFXBins).asInteger;
	e.postln;
	e.plot;
	~warpBuf.sendCollection(e);
}.value;


)


//two spectrograms to visually show differences in spectrum
(
x = Synth(\freqWarp, [\rate, 1, \buf, ~warpBuf]);
w = Window.new("FreqWarp", Rect(10,10, 600, 600)).front;
a = Spectrogram.new(w, Rect(10, 10, 580, 280), highfreq:10000);
b = Spectrogram.new(w, Rect(10, 300, 580, 280), highfreq:10000);
a.start;
a.background_(Color.black);
a.color_(Color.green);

b.start;
b.inbus_(~spectBus);
b.background_(Color.black);
b.color_(Color.white);
)







