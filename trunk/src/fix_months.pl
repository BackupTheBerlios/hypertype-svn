#!/bin/bash

my $sFileToFix = ARGV[1];
die "Need file arg!\n" unless ($sFileToFix == "");

print $sFileToFix;

open (hMyFile, "+<$sFullPath");
$sText = join("", <hMyFile>);

$iReplacements = ($sText =~ s/