#
# c55x opcode table extractor
#
#    usage: pdftotext swpu067e.pdf - | perl extract.pl
# 

sub is_opcode
{
	my $token = shift;

	if (length($token) != 8) {
		return 0;
	}

	if ($token =~ /[#\[\(\]\)\,\.]/) {
		return 0;
	}

	if ($token ~~ [qw(ADDSUBCC DMAXDIFF DMINDIFF)]) {
		return 0;
	}

	return 1;
}

# seek for the table

while (my $line = <STDIN>) {
    last if $line =~ /6.1 Instruction Set Opcodes/;
}

# iterate over the items

while (my $line = <STDIN>) {
    next unless ($line =~ /^\s*[01]/);

    my @tokens = split(' ', $line);
    next unless (length($tokens[0]) == 8);

    my @values = ();
    foreach my $token (@tokens) {
	last unless (is_opcode($token));
	push @values, $token;
    }
    
    print join(' ', @values) . "\n" if @values;
}
