#!/usr/bin/perl

die "Usage: CapitalizeLines.pl /path/to/file.txt\n" unless (@ARGV == 1);

die "Unable to read input file: $!\n" unless open(hInputFile, @ARGV[0]);

@aEntireFileLines = <hInputFile>;

my @aCapitalizedLines;

foreach $sOrigLine (@aEntireFileLines)
{
	push(@aCapitalizedLines, ucfirst($sOrigLine));
}

close (hInputFile);

die "Unable to write to output file: $!" unless open (hOutputFile, ">@ARGV[0]");

foreach $sCapLine (@aCapitalizedLines)
{
	print hOutputFile $sCapLine;
}
