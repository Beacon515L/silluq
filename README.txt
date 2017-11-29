## SILLUQ
# Tool for simply versifying markdown urtext and checking citation completion
by Liam Greenough (beacon515@gmail.com)

Silluq expects input text in stdin and directs output to stdout and messages to stderr. It can operate in realtime. It will terminate when stdin ends.

# Silluqification of regular text file
$ cat input.txt | silluq > bar.silluq

Here, input.txt is any regular text file and bar.silluq is the output urtext file. Silluqification refers to the process of rearranging a text file to make it easily citable. Physically, silluq adds line breaks after all sentence-final punctuation marks (by default !?.,).  The set of punctuation marks can be overridden:

$ cat input.txt | silluq -c "." > bar.silluq
will only linebreak at full stops.

Text is then cited by line number in the output file, e.g. (foo:420). The citation format is (text_designator:line_number), inclusive of the parentheses. The designator is completely arbitrary and not part of silluqification: it is purely used when citing the text to refer to different urtext files. The only constraint is that it not contain the colon or equals sign.

Citations can be ranges, e.g. (foo:1-2,6), (foo:3,5,6,1,5).

Silluqification is not mandatory, but helpful if texts are not already broken into many lines with relatively high granularity (e.g. the Bible). There are no constraints on line length in urtext files except that they cannot contain newlines (this being their delimiter).

# Verification of urtext citations

Given the following input.txt:
>According to some file, the sky is blue. (foo:1).

and the following bar.silluq:
>The sky is blue.
>The sea is green.

$ cat input.txt | silluq foo=bar.silluq

will produce:

    foo: 1/2 cited from 1 (50%)
    ---
    total: 1/2 cited from 1 (50%)

Silluq can operate on multiple urtext files:
$ cat input.txt | silluq foo=bar.silluq bar=foo.silluq

    foo: 1/2 cited from 1 (50%)
    bar: 0/8 cited from 1  (0%)
    ---
    total: 1/10 cited from 1 (10%)
