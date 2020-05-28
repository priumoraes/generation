#!/bin/perl
# @author: Priscilla Moraes
# This calculates X_largest and Y_biggest for scale
# Parameter: Directory where the Suggestion files are present

use strict 'vars';
use File::Glob ':glob';
use File::Basename;
print "program start\n";
print `pwd`;

my @a = bsd_glob("C:\\Users\\SIGHT\\Documents\\MATLAB\\data\\xml\\SuggestionL*.xml");
my %xmlname=();
foreach my $b (@a)
{
	if ( basename($b) =~ /(SuggestionL\d+).xml/) {
	$xmlname{$1}=0;
	}
}
#print keys(%jpgname)+0;
#print @a;
foreach my $x (keys(%xmlname)) {
	my $command="C:\\Users\\SIGHT\\Documents\\LineGraph\\volatility\\calculate_xyscale.pl C:\\Users\\SIGHT\\Documents\\MATLAB\\data\\xml\\$x.xml";
	print STDERR $x;
	#print $command;
	#exit;
	system($command);
}
