**About FFTease 3.0**

FFTease is a collection of objects designed to facilitate spectral sound
processing in Max. The collection was designed by Eric Lyon and
Christopher Penrose in 1999, and has been maintained by Eric Lyon since
2003. 


**Compilation and Installation** 


This distribution comes with pre-compiled externals for Linux, macOS,
and Windows. These externals should reside in a folder called "fftease"
and be referenced in Pd patches prepended by "fftease/" to avoid name clashes
with other third-party externals. On macOS, the fftease folder should
be placed in the user folder "~/Documents/Pd/" to make the externals
available to Pd.


**Performance Considerations**


The default Pd audio buffer settings for both I/O vector size and signal
vector size will work fine for FFT sizes up to around 4096 or so. For
larger FFT sizes, adjusting the Pd signal vector size and I/O vector
size upward can dramatically improve performance. With larger FFT sizes,
the reported CPU load may fluctuate. This is because a large FFT is
being performed only once for several vectors worth of samples. The
default FFT size is 1024, and the default overlap factor is 8. The
maximum FFT size is 1073741824. 


**For Coders**


Full source code is included, so that intrepid coders can extend FFTease. 
The FFTease code is distributed under the MIT license to facilitate deployment 
to any combination of free, open-source, commercial, or closed-source projects.


**Acknowledgements**


This update of FFTease was supported by coding, advice, and bug reports from @porres, @umlaeute, and @Lucarda. 

Happy sonic exploration!

Eric Lyon
ericlyon@vt.edu
Blacksburg, Virginia
January, 2021
