if(! -e fftease){
    mkdir("fftease");
}
while(<*>){
    chomp;
    if(/darwin$/ || /libfftease.pd_darwin.dylib/){
	`mv $_ fftease`;
    }
}
