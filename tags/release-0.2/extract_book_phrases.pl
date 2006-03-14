#!/usr/bin/perl

die "Usage: extract_words.pl /path/to/input/file.txt" unless (@ARGV == 1);

die "Unable to open input file '".@ARGV[0]."': $!" unless open(hInputFile, @ARGV[0]);

# Slurp entire file
my @aLines = <hInputFile>;

# Filter out blank lines
@aLines = grep { /[A-Za-z]+/ } @aLines;

#Remove carriage returns and lump into one huge string
my $sEntireFile = join(" ", @aLines);

#Remove everything except simple words
$sEntireFile =~ s/[\r\n]/ /g;
$sEntireFile =~ s/ +/ /g;


my @aSentences;
while ($sEntireFile =~ /\.+ /)
{
   push @aSentences, $` . $&;
   $sEntireFile = $';
}
push @aSentences, $sEntireFile;

foreach (@aSentences)
{
   $_ =~ s/ $//g;
   print $_."\n";
}
