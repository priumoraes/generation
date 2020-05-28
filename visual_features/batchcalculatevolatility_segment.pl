#!/bin/perl
# @author: Priscilla Moraes
# This calculates percentage of fluctuation for each segment in all the graphs

use strict 'vars';
use File::Glob ':glob';
use File::Basename;
print "program start\n";
print `pwd`;

my @a = bsd_glob("C:\\Users\\SIGHT\\Documents\\MATLAB\\data\\xml\\SuggestionL*.xml");
my %xmlname=();
my $number = "";

foreach my $b (@a)
{
	if ( basename($b) =~ /(SuggestionL\d+).xml/) {
	$xmlname{$1}=0;
	}
}
#print keys(%jpgname)+0;
#print @a;
foreach my $x (keys(%xmlname)) {
	if ($x =~ /(\d+)/g){
		$number = $1;
	}
	my $command="C:\\Users\\SIGHT\\Documents\\LineGraph\\volatility\\calculatevolatility_segment.pl C:\\Users\\SIGHT\\Documents\\LineGraph\\LinegraphSpreadsheet\\Training\\L${number}BNresultPar.txt C:\\Users\\SIGHT\\Documents\\MATLAB\\data\\xml\\$x.xml";
	print STDERR $x;
	#print $command;
	#exit;
	system($command);
}
