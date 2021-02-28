**About FFTease 3.0**

FFTease is a collection of objects designed to facilitate spectral sound
processing in Max and Pd. The collection was designed by Eric Lyon and
Christopher Penrose in 1999, and has been maintained by Eric Lyon since
2003. 

**List of Objects**

- bthresher~ similar to thresher~ but with more control
- burrow~ a cross-referenced filtering object
- cavoc~ an 8-rule cellular automata that generates spectra
- cavoc27~ a 27-rule cellular automata object
- centerring~ a spectral modulation object
- codepend~ a classic block convolution object
- cross~ a cross synthesis object with gating
- dentist~ a partial knockout object
- disarrain~ an interpolating version of disarray~
- disarray~ a spectral redistribution object
- drown~ a noise reduction (or increase) object
- enrich~ an oscillator resynthesis with a user-designed waveform
- ether~ another spectral compositing object
- leaker~ a sieve-based cross fader
- mindwarp~ a spectral formant warping object
- morphine~ a morphing object
- multyq~ a four band filter
- pileup~ a spectral accumulation object
- pvcompand~ a spectral compressor/expander object
- pvgrain~ a spectrum analyzer for granular resynthesis
- pvharm~ a harmonizer
- pvoc~ an additive synthesis phase vocoder
- pvtuner~ a spectrum quantizer for tuning to arbitrary scales
- pvwarp~ a non-linear frequency warper
- pvwarpb~ a non-linear frequency warper with a user-accessible warp array
- reanimator~ an audio texture mapper
- resent~ similar to residency~ but with independent bin control
- residency~ a spectral sampler useful for time scaling
- residency_buffer~ a spectral sampler that writes analysis data to a Pd array
- schmear~ a spectral smear object
- scrape~ a noise reduction (or increase) object with frequency control
- shapee~ a frequency shaping object
- swinger~ a phase swapping object
- taint~ a cross synthesis object
- thresher~ an amplitude/frequency sensitive gating object
- vacancy~ a spectral compositing object
- xsyn~ a cross synthesis with compression object


**Compilation and Installation** 


All necessary files are contained in the folder "fftease," compiled for
various flavors of Linux, macOS, and Windows. Download the appropriate
package and move the folder "fftease" to a location accessible by Pd. On
macOS, that location is typically "~/Documents/Pd/externals/".
Alternatively, FFTease can be installed via Deken. Under the Pd menu
item Help -> Find externals, search for "fftease" and then install it.
You could also compile FFTease directly from the code in this
repository. 


**Performance Considerations**


The default Pd audio buffer settings for both I/O vector size and signal
vector size will work fine for FFT sizes up to around 4096 or so. For
larger FFT sizes, adjusting the Pd signal vector size and I/O vector
size upward can dramatically improve performance. With larger FFT sizes,
the reported CPU load may fluctuate. This is because a large FFT is
being performed only once for several vectors worth of samples. The
default FFT size is 1024, and the default overlap factor is 8. The
maximum FFT size is 1073741824. 


**Acknowledgements**


This update of FFTease was supported by coding, advice, and bug reports from @porres, @umlaeute, and @Lucarda. 

Happy sonic explorations!

Eric Lyon  
ericlyon@vt.edu  
Blacksburg, Virginia  
February 28, 2021
