#!/usr/bin/perl
# Script to create an entity out of game/common/Template.[ch].  Pass in the name of the entity.

# First, open up the Template files:
open (TEMPLATE_C, "<../common/Template.c") ||
 	die "Couldn't open ../common/Template.c for reading!\n";
open (TEMPLATE_H, "<../common/Template.h") ||
	die "Couldn't open ../common/Template.h for reading!\n";

$entname = @ARGV[0];
$lowname = "e_" . lc($entname);
$uppname = uc($entname) . "_C";

open (NEWENT_C, ">$entname.c") ||
	die "Couldn't open $entname.c for writing!\n";
open (NEWENT_H, ">$entname.h") ||
	die "Couldn't open $entname.h for writing!\n";

while (<TEMPLATE_C>) {
	# Read the line:
	$curline = $_;
	# Replace all occurances of Template with $entname:
	$curline =~ s/Template/$entname/g;
	$curline =~ s/e_template/$lowname/g;
	
	print NEWENT_C $curline;
}

while (<TEMPLATE_H>) {
	# Read the line:
	$curline = $_;
	# Replace all occurances of Template with $entname:
	$curline =~ s/TEMPLATE_C/$uppname/g;
	$curline =~ s/Template/$entname/g;
	$curline =~ s/e_template/$lowname/g;
	
	print NEWENT_H $curline;
}

close (NEWENT_H);
close (NEWENT_C);

close (TEMPLATE_H);
close (TEMPLATE_C);
