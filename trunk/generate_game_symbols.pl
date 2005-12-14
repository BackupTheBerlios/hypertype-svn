#!/usr/bin/perl

my $sChars = "`~!@#$%^&*()-_=+";
my @aChars = split(//, $sChars);

foreach $sFirstChar (@aChars)
{
   foreach $sSecondChar (@aChars)
   {
      foreach $sThirdChar (@aChars)
      {
         print $sFirstChar.$sSecondChar.$sThirdChar."\n";
      }
   }
}