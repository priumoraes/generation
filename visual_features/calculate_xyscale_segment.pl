#!/bin/perl
# @author: Priscilla Moraes

use XML::Simple;
use Data::Dumper;
use File::Basename;

my $resultParFile = $ARGV[0];		# the txt file like LBNresultPar.txt with parameters -> passed as first argument
open(FILE1, $resultParFile);
@lines = <FILE1>;					# Read it into an array
close(FILE1);						# Close the file

my $suggestionXMLfile = $ARGV[1]; 	# SuggestionLXX.xml file -> passed as second argument

my $xs1  = XML::Simple->new();
my $doc  = $xs1->XMLin($suggestionXMLfile);

my $xyscale_segment = 'C:\\Users\\SIGHT\\Documents\\LineGraph\\volatility\\results\\xyscale_segment.txt';
$filename = basename($suggestionXMLfile);

# subroutine to get the points from the segments
sub getPoints
{
	foreach $eachline (@_) 			#getting the parameters from LBNresultpar.txt file
	{
		@line_words = split(' ', $eachline);
		if ($line_words[0] eq 'Points'){
			($a, @segmentPoints) = @line_words; # $a receives the word "Points" and @segmentPoints receives the points
		}
	}
	return @segmentPoints;
}

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

#$xtickmarks = @xAxis."";
#$ytickmarks = @yAxis."";
@yAxis 		= sort {$a <=> $b} @yAxis;	# numerical sort
$amplitude 	= pop(@yAxis);				# getting the amplitude
@Xpoints	= sort {$a <=> $b} @Xpoints;
$test		= pop(@Xpoints);
#print "Number of sample points: $totalNumberSP\n";
&calculateResult(@Ypoints);

# this sub receives all the sample points: Ypoints
sub calculateResult
{
	local $xLargest 			= 0.6808; #those two values need to be in an external file, so we can learn new ones
	local $yBiggest				= 0.8740;
	@intervals 					= &getPoints(@lines); 	
	if ($intervals[0] != 1) {
		@allintervals = (1, @intervals);
	} else {
		@allintervals = @intervals;
	}
	if (@allintervals[$#allintervals] < $totalNumberSP) {
		push(@allintervals, $totalNumberSP);
	}
	print "Intervals: @intervals\n";
	print "Allintervals: @allintervals\n";
	for ($j = 0; $j < (@allintervals."" - 1); ++$j){
		$pointA 				= $_[$allintervals[$j] - 1]; 			 # @Ypoints[0]
		$pointB 				= $_[$allintervals[$j]];		 		 # @Ypoints[1]
		$beginning				= $allintervals[$j];
		$end					= $allintervals[$j + 1];
		$sizeweight 			= 0;
		$frequencyweight 		= 0;
		$new_sizeweight 		= 0;
		$new_frequencyweight	= 0;
		$change 				= 0;
		$up 					= 0;
		$down 					= 0;
		$segsize 				= 0;
		$weight					= 0;
		$result					= 0;
		print "PointA: $pointA\n";
		print "PointB: $pointB\n";
		print "J: $j\n";
		print "Beginning: $beginning\n";
		print "End: $end\n";
		if ($pointA < $pointB) {			#initializing variables to control the changes
			$up = 1;
		} else {
			$down = 1;
		}
		# counting the number of times the graphic has a change in behaviour (goes up after been going down and vice-versa)
		for ($i = $beginning; $i < $end; ++$i) {
			$pointA = $_[$i];
			$pointB = $_[$i + 1];
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
		}
		if (!($change)){   					# avoiding division by zero
			$sizeweight = 0;
		} else {
			$sizeweight = $weight / $change;
		}
		$frequencyweight = $change / $totalNumberSP;
		# TODO: add test if original result is bigger, if so keep it (replace values in config file)
		print $result 			= ($sizeweight + $frequencyweight) / 2;
		open FILE2,">>$xyscale_segment";
		print FILE2 "$filename $beginning $end $j $result $frequencyweight $sizeweight\n";
		close FILE2;
		print "Frequencyweight variable value: $frequencyweight\n";
		print "Change variable value: $change\n";
		print "Segment Size Weight: $new_sizeweight\n";
		print "Segment Frequency Weight: $new_frequencyweight\n";
		print "Segment fluctuation percentage: $result\n";
	}
	#print $result;
}

