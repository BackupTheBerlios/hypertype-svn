#!/usr/bin/perl

die "Usage: extract_words.pl /path/to/input/file.txt letters" unless (@ARGV == 2);

die "Unable to open input file '".@ARGV[0]."': $!" unless open(hInputFile, @ARGV[0]);

# Slurp entire file
my @aLines = <hInputFile>;

# Filter out blank lines
@aLines = grep { /[A-Za-z]+/ } @aLines;

#Remove carriage returns and lump into one huge string
my $sEntireFile = join(" ", @aLines);

#Remove everything except simple words
#$sEntireFile =~ s/[^A-Za-z]/ /g;
$sEntireFile =~ s/["?!\r\n:\(\)]/ /g;
$sEntireFile =~ s/ +/ /g;


my @aSentences = split(/[;\.] /, $sEntireFile);
$sChars = @ARGV[1];
#print $sChars;
@aSentences = grep { /^([$sChars ]*)$/i } @aSentences;



foreach (@aSentences)
{
   if (length $_ > 20)
   {
      print lc($_).".\n";
   }
}
