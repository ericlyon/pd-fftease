
#!/usr/bin/perl                                                                 
$HOME = $ENV{"HOME"};
require "$HOME/PERL/libperl.pl";

while(<*>){
chomp;
if(/.c/){
print "$_ ";
}

}
print "\n";