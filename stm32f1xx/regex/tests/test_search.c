#include <stdio.h>
#include "re.h"

void test_search(void)
{
    /* Standard int to hold length of match */
    int match_length;

    /* Standard null-terminated C-string to search: */
    const char* string_to_search = "ahem.. 'hello world !' ..";

    /* Compile a simple regular expression using character classes, meta-char and greedy + non-greedy quantifiers: */
    re_t pattern = re_compile( "[Hh]ello [Ww]orld\\s*[!]?" );

    /* Check if the regex matches the text: */
    int match_idx = re_matchp( pattern, string_to_search, &match_length );

    if ( match_idx != -1 )
    {
        printf( "match at idx %i, %i chars long.\n", match_idx, match_length );
    }
}
