#!/usr/bin/perl -w

# create_docs.pl

# script to check out the source modules and build docs for each supported arch

# 13/09/2001 - Warren Moore
# $Id: create_docs.pl,v 1.1 2002/04/07 13:48:52 vap-warren Exp $
# Copyright 2000-2001 Vapour Technology Ltd.

# bring in the environment vars
use strict;
use Env qw(VALET_DOCROOT VALET_CVSOPTIONS);

#=== global vars
my $proj_name = "VALET";
my $mod_prefix = "Vapour/";
my $vmake_cmd = "VMake.pl -doc -o $VALET_DOCROOT -a ../VArch/arch.list";

#=== error function
sub error {
	my $text = shift(@_);
	print "create_docs.pl error: $text\n";
	exit -1;
}

#=== main start
# check we have the variables
if (not $VALET_DOCROOT) {
	error("No doc root specified\nDO NOT RUN DIRECTLY - Edit and run build_local_docs.sh");
}
if (not $VALET_CVSOPTIONS) {
	error("No cvs options specified");
}
my @cvs_options = split / /, $VALET_CVSOPTIONS;

# check for and read the modules.list file
if (not open (MODULE_LIST, "<modules.list")) {
	error("Unable to open modules.list");
}

# read through the list
my @modules;
MOD_READ_SKIP: while (<MODULE_LIST>) {
	# skip comments and blank lines
	next MOD_READ_SKIP if /^\s*\#/;
	next MOD_READ_SKIP if /^\s*$/;
	# remove any trailing newlines
	chomp;
	# add the module to the list
	push @modules, $_;
}

# all done, close the list
close(MODULE_LIST);

# check out and read the supported architecture list
my @cvs_args = ("cvs", @cvs_options, "co", "VArch");
my $result = 0xFFFF & system @cvs_args;
if ($result != 0) {
	error("Unable to checkout VArch module");
}

# check for and read the modules.list file
if (not open (ARCH_LIST, "<VArch/arch.list")) {
	error("Unable to open arch.list");
}

# open the doc site arch list
my $web_listname = $VALET_DOCROOT . "arch.list";
if (not open (WEB_ARCH_LIST, ">$web_listname")) {
	error("Unable to open web arch list");
}

# read through the list
my @arch;
my @arch_desc;
ARCH_READ_SKIP: while (<ARCH_LIST>) {
	# skip comments and blank lines
	next ARCH_READ_SKIP if /^\s*\#/;
	next ARCH_READ_SKIP if /^\s*$/;
	# remove any trailing newlines
	chomp;
	# look for the values, and push them onto the correct lists
	if (/^([\w\d]+)\s+(.+)$/) {
		# write the data to the site arch list
		print WEB_ARCH_LIST "$1\t$2\n";
		# store all but noarch in the gen list
		if ($1 ne "noarch") {
			push @arch, $1;
			push @arch_desc, $2;
		}
	}
}

# all done, close the lists
close(ARCH_LIST);
close(WEB_ARCH_LIST);

# make the eht directory
if (not mkdir "eht", 0700) {
	error("Unable to create eht dir");
}

# check out all the modules and pull in the eht's
foreach my $mod_name (@modules) {
	my $mod_fullname = $mod_prefix . $mod_name;
	# checkout the module
	@cvs_args = ("cvs", @cvs_options, "co", $mod_fullname);
	my $result = 0xFFFF & system @cvs_args;
	if ($result != 0) {
		error("Unable to checkout $mod_fullname module");
	}
	
	#=== run vmake to generate noarch source tree
	# using VMake Perl script
	print `cd $mod_prefix && $vmake_cmd noarch ; cd ..`;
	
	# pull all the eht's in the dir
	my $find_results = `find $mod_fullname/. -name '*.eht'`;
	if ($find_results) {
		$find_results =~ s/\n/ /g;
		`cp $find_results eht`;
	}
	# run CxxDoc for the default architecture
	#`CxxDoc -pn $proj_name -o $VALET_DOCROOT -p $mod_prefix -i $mod_prefix -eht eht -tc class -td docnode > /dev/null 2>&1`;
	`CxxDoc -pn $proj_name -o $VALET_DOCROOT -p $mod_prefix -i $mod_prefix -eht eht -tc class -td docnode`;
}

# save us from processing the eht's again
unlink <eht/*>;

# build source trees for each arch and generate doc trees
foreach my $arch_name (@arch) {
	# delete the initial source tree
	my @rm_args = ("rm", "-rf", $mod_prefix);
	my $result = 0xFFFF & system @rm_args;
	if ($result != 0) {
		error("Unable to delete source tree during arch build '$arch_name'");
	}
	
	# check out the modules and generate the correct source trees
	foreach my $mod_name (@modules) {
		my $mod_fullname = $mod_prefix . $mod_name;
		# checkout the module
		@cvs_args = ("cvs", @cvs_options, "co", $mod_fullname);
		my $result = 0xFFFF & system @cvs_args;
		if ($result != 0) {
			error("Unable to checkout $mod_fullname module during arch build '$arch_name'");
		}
		
		#=== run vmake to generate the relevant source tree
		# using VMake Perl script
		print `cd $mod_prefix && $vmake_cmd $arch_name; cd ..`;
		
	}

	# run CxxDoc to generate the doc tree in the right place
	my $doc_root = $VALET_DOCROOT . "arch/" . $arch_name;
	`CxxDoc -pn $proj_name -o $doc_root -p $mod_prefix -i $mod_prefix -eht eht -tc class -td docnode > /dev/null 2>&1`;
}
