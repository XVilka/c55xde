#
# c55x opcode table extractor (1b)
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

# iterate over the items

while (my $line = <STDIN>) {
    next unless ($line =~ /^\s*[01]/);

    my @tokens = split(' ', $line);
    next unless (length($tokens[0]) == 8);

    my @i_list = grep { is_opcode($_) } @tokens;
    my @s_list = grep { not is_opcode($_) } @tokens;

    printf("%s\t%s\n", join('', reverse @i_list), join(' ', @s_list));
}
