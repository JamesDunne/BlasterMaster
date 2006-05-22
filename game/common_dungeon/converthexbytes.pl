#!/usr/bin/perl
# Perl script to flip 8-bit hex bytes like 0xA0 to 0x0A  or 0x34 to 0x43.
# Must search thru all C files to find put_sprite...(..., ..., 0x##)

while ( defined($filename = glob("*.c")) ) {
	open (CFILE, $filename) ||
		die "Could not open $!";
	open (NEWCFILE, ">$filename.new") ||
		die "Could not create $filename.new";
	while (<CFILE>) {
		$line = $_;
		chomp ($line);
		if ( $line =~ /(.*)put_sprite(.*)\(([^,]*),\W*([^,]*),\W*([^,]*),\W*0x(.)(.)\)(.*)/ ) {
			print NEWCFILE "$1put_sprite$2($3, $4, $5, 0x$7$6)$8\n";
		} else {
			print NEWCFILE "$line\n";
		}
	}
	close (NEWCFILE);
	close (CFILE);
	if (-e "$filename.old") {
		print "$filename.old already exists!  Output file is still $filename.new\n";
	} else {
		rename ($filename, "$filename.old");
		rename ("$filename.new", $filename);
	}
}