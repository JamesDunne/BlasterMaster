#!/usr/bin/perl
# Script to add the entity to the build process

if (length (@ARGV) < 1) {
	print "linkinent.pl <entity name> <entity class #>\n";
	print "  <entity class #> can be 0xXX where XX is the class # in HEX, or can be just plain decimal\n";
	print "  <entity name> is the name of the entity to add, in proper capitalisation.\n";
	exit();
}

$entname = @ARGV[0];
$entnum = @ARGV[1];
$uppname = uc($entname);
$path = "map00";

# Modify the classes.h file to define the CLASS_ enum and #include the .h file:
open (CLASSES_H, "<../common/classes.h") || die "Couldn't open classes.h\n";
open (OUT_CLASSES_H, ">../common/classes.h2") || die "Couldn't open classes.h2 for writing\n";

while (<CLASSES_H>) {
	# Read the line:
	$line = $_;
	# Write back out the line:
	print OUT_CLASSES_H $line;
	if (substr($line, 0, 74) eq "///---Do not change this line!! It is used by scripts to add entities!!---") {
		# First tag-line is for CLASS_ definitions:
		if (!$wrote_class) {
			print OUT_CLASSES_H "\tCLASS_$uppname = $entnum,\n";
			$wrote_class = TRUE;
		} else {
			# Second tag-line is for #include:
			print OUT_CLASSES_H "#include \"../$path/$entname.h\"\n";
		}
	}
}

close (OUT_CLASSES_H);
close (CLASSES_H);

# Modify the interface.c file:

open (INTERFACE_C, "<interface.c") || die "Couldn't open interface.c\n";
open (OUT_INTERFACE_C, ">interface.c2") || die "Couldn't open interface.c2 for writing\n";

while (<INTERFACE_C>) {
	# Read the line:
	$line = $_;
	# Write back out the line:
	print OUT_INTERFACE_C $line;
	if (substr($line, 0, 78) eq "///---Do not change this line!! It is used by scripts to add new entities!!---") {
		print OUT_INTERFACE_C "\tDEFINE_CLASS(CLASS_$uppname, ClassProperties_$entname, SPAWNFLAG_MAPSPAWNABLE)\n";
	}
}

close (OUT_INTERFACE_C);
close (INTERFACE_C);

# Now open the Makefile and add the object file to the OBJS list:

open (MAKEFILE, "<Makefile") || die ("Could not open Makefile\n");
open (OUT_MAKEFILE, ">Makefile.2") || die ("Could not write to Makefile.2\n");

while (<MAKEFILE>) {
	$line = $_;
	# Found the start of 'OBJS = ' line:
	if ($line =~ m/^OBJS\s=\s(.*)[\n\\]/) {
		
		# Read in the first line the object filenames:
		@objs = $line =~ m/([^\s]+)/g;
		
		# Remove trailing \:
		$#objs--;
		
		# Read lines until we dont have a trailing \:
		while ($line = <MAKEFILE>) {
			push (@objs, $line =~ m/([^\s]+)/g);
			# If we don't have a trailing \, then quit, otherwise remove it from the list:
			if ($objs[$#objs] ne "\\") { last; } else { $#objs--; }
		}
		
		# Add the entity object file to the end:
		push(@objs, "$entname.o");
		
		# Now, output the list:
		$i = -2;
		foreach $x (@objs) {
			print OUT_MAKEFILE "$x ";
			if ($i++ >= 3) {
				$i = 0;
				print OUT_MAKEFILE "\\\n\t";
			}
		}
		print OUT_MAKEFILE "\n";
	} else {
		print OUT_MAKEFILE $line;
	}
}

close (OUT_MAKEFILE);
close (MAKEFILE);

system ("gcc -MM -I../../common -I../common $entname.c >> Makefile.2");

# Now, overwrite the old files with the new ones:
unlink "../common/classes.h";
unlink "interface.c";
unlink "Makefile";
rename "../common/classes.h2", "../common/classes.h";
rename "interface.c2", "interface.c";
rename "Makefile.2", "Makefile";

# Done.
