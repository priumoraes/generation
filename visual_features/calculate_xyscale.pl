#!/bin/perl
# @author: Priscilla Moraes

use XML::Simple;
use Data::Dumper;
use File::Basename;

my $suggestionXMLfile = $ARGV[0]; 	# SuggestionLXX.xml file -> passed as second argument


my $xs1  = XML::Simple->new();
my $doc  = $xs1->XMLin($suggestionXMLfile);

my $xyscale = 'C:\\Users\\SIGHT\\Documents\\LineGraph\\volatility\\results\\xyscale.txt';

foreach my $samplepoint ( @{ $doc->{LineGraph}->{SamplePoint} } ) {
	# one sample point
	my $samplePointItem = {};
	$samplePointItem->{X} = $samplepoint->{Coord}->{X};
	push (@Xpoints, $samplepoint->{Coord}->{X});
	$samplePointItem->{Y} = $samplepoint->{Coord}->{Y};
	push (@Ypoints, $samplepoint->{Coord}->{Y});
	push( @samplePoints, $samplePointItem );
}

# getting coordinates from tickmarks
foreach my $tickmarkcoordinate ( @{ $doc->{LineGraph}->{YAxis}->{TickMark} } ) {
	push (@yAxis, $tickmarkcoordinate->{Coord}->{Y});
}

foreach my $tickmarkcoordinate ( @{ $doc->{LineGraph}->{XAxis}->{TickMark} } ) {
	push (@xAxis, $tickmarkcoordinate->{Coord}->{X});
}

$totalNumberSP = @samplePoints."";

my $pointA 				= @Ypoints[0];
my $pointB 				= @Ypoints[1];
my $up 					= 0;
my $down 				= 0;
my $change 				= 0;
my $segsize 			= 0;
my $weight				= 0;
my $sizeweight 			= 0;
my $frequencywejght 	= 0;
my $new_sizeweight 		= 0;
my $new_frequencywejght = 0;


$xtickmarks = @xAxis."";
$ytickmarks = @yAxis."";
@yAxis 		= sort {$a <=> $b} @yAxis;	# numerical sort
$amplitude 	= pop(@yAxis);				# getting the amplitude
@Xpoints	= sort {$a <=> $b} @Xpoints;
$test		= pop(@Xpoints);

if ($pointA < $pointB) {				#initializing variables to control de changes
	$up = 1;
} else {
	$down = 1;
}

# counting the number of times the graphic has a change in behaviour (goes up after been going down and vice-versa)
for ($i = 0; $i < $totalNumberSP; ++$i) {
	$pointA = @Ypoints[$i];
	$pointB = @Ypoints[$i + 1];
	if (!($pointB)){
		$pointB = $pointA;
	}
	if (($pointB > $pointA) && (!$up) && (($pointB - $pointA) > 3)){
		++$change;
		$up = 1;
		$down = 0;
		$weight += ($segsize / $amplitude);
		$segsize = $pointB - $pointA;
	} elsif (($pointB < $pointA) && (!$down) && (($pointA - $pointB) > 3)){
		++$change;
		$up = 0;
		$down = 1;
		$weight += ($segsize / $amplitude);
		$segsize = $pointA - $pointB;
	} else {
		$segsize += abs($pointB - $pointA);
	}
	#print "$segsize\n";
	#print "$amplitude\n";
	#print "$weight\n";
}

if (!($change)){   					# avoiding division by zero
	$sizeweight = 0;
} else {
	$sizeweight = $weight / $change;
}
if ($sizeweight > 1){				#testing because some times the graph surpass the y-axis
	$sizeweight = 1;
}
$frequencyweight 	= $change / $totalNumberSP;
$result 			= ($sizeweight + $frequencyweight) / 2;
$filename 			= basename($suggestionXMLfile);

#output results to a file
open FILE,">>$xyscale";
print FILE "$filename $result $frequencyweight $sizeweight\n";
close FILE;

print "Number of pixels in the x axis: $test\n";
print "Number of changes: $change\n";
print "Number of sample points: $totalNumberSP\n";
#print "Number of tickmarks on the YAxis: $ytickmarks\n";
#print "Number of tickmarks on the XAxis: $xtickmarks\n";
print "Frequency weight: $frequencyweight\n";
#print "Change size weight: $sizeweight\n";
print "Overall fluctuation percentage: $result\n";
