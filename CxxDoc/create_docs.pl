#!/usr/bin/perl -w

# bring in the environment vars
use strict;
use Env qw(VALET_SRCROOT VALET_DOCROOT);

# check we have the variables
if (not $VALET_SRCROOT or not $VALET_DOCROOT) {
	print "create_docs.pl error: No roots specified\n";
	exit -1;
}

# make the eht directory
`mkdir eht`;

# move the eht files into the dir
my $find_results = `find $VALET_SRCROOT -name '*.eht' -printf "%p "`;
printf "Found: $find_results\n";
if ($find_results) {
	`cp $find_results eht`;
}

# run CxxDoc on the source
`CxxDoc -pn VALET -o $VALET_DOCROOT -p $VALET_SRCROOT -i $VALET_SRCROOT -eht eht -tc class -td docnode > /dev/null 2>&1`;

# delete the eht dir
`rm -rf eht`;