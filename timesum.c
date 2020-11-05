/*
 * Copyright 2020, Andrew Lindesay <apl@lindesay.co.nz>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usage.h"

#define REGEX_RANGE "^([0-9]{1,2}):([0-9]{1,2})-([0-9]{1,2}):([0-9]{1,2})$"
#define REGEX_ABSOLUTE "^([0-9]{1,2}):([0-9]{1,2})$"
#define REGEX_DECIMAL "^[0-9]+\\.[0-9]+|[0-9]+$"

#define MAX_MATCHES 5

#ifndef TIMESUM_VERSION
#define TIMESUM_VERSION "?"
#endif

typedef struct regexes regexes;
struct regexes {
	regex_t range;
	regex_t	absolute;
	regex_t	decimal;
};

void usage() {
	for(int i = 0; i < usage_txt_len; i++) {
		putc(usage_txt[i], stdout);
	}
	putc('\n', stdout);
}

/*
This function will check to see if either of the supplied switches; either the short or
the long form are present in the arguments.
*/

int is_switches(int argc, char** argv, const char* shortswitch, const char* longswitch) {
	for (int i = 1; i < argc; i++) {
		if (0 == strcmp(argv[i], shortswitch) || 0 == strcmp(argv[i], longswitch)) {
			return 1;
		}
	}

	return 0;
}

/*
Is the user asking for help?
*/

int is_help(int argc, char** argv) {
	return is_switches(argc, argv, "-h", "--help");
}

/*
Is the user asking for the version?
*/

int is_version(int argc, char** argv) {
	return is_switches(argc, argv, "-v", "--version");
}

int init_a_regex(regex_t* rex, const char* expression) {
	if (0 != regcomp(rex, expression, REG_EXTENDED)) {
		printf("! unable to compile the regex expression [%s]\n", expression);
		return 1;
	}
	return 0;
}

/*
This is a setup step that runs when the program starts up.  It will load in all of the
regular expressions that drive the application's pattern matching on the arguments.
*/

int init_regex(regexes* rexs) {
	if ( (0 != init_a_regex(&(rexs->range), REGEX_RANGE))
		|| (0 != init_a_regex(&(rexs->absolute), REGEX_ABSOLUTE))
		|| (0 != init_a_regex(&(rexs->decimal), REGEX_DECIMAL)) ) {
		return 1;
	}
	return 0;
}

/*
This will parse the supplied text as an hour and will convert that to minutes and store
the result in the provided int pointer.  If the hour is invalid then it will write an
error to stderr and return 1.  If it is successful then it will return 0.
*/

int to_minutes_from_hour_string(const char* text, int* mins) {
	*mins = atoi(text);
	if (*mins > 23) {
		fprintf(stderr, "! hour value [%s] > 23\n", text);
		return 1;
	}
	*mins *= 60;
	return 0;
}

/*
This will parse the supplied text as minutes and store the result in the integer supplied.
If the minutes were out of range then it will write an error message and return 1 otherwise
it will return 0.
*/

int to_minutes_from_mins_string(const char* text, int* mins) {
	*mins = atoi(text);
	if (*mins > 59) {
		fprintf(stderr, "! minutes value [%s] > 59\n", text);
		return 1;
	}
	return 0;
}

/*
The text will be something like "07:45-11:19".  The pmatches are from a regular expression
match where 0 is the match of the whole text.  1,2 and 3,4 are the first and second
hour:minute respectively.  The absolute difference between the two times is written in
minutes to the supplied integer pointer.  It will return 1 if there was a problem and 0 if
the minutes were written to the integer pointer.
*/

int to_minutes_from_range(const char* text, const regmatch_t* pmatch, int* mins) {
	int mins1h;
	int mins1m;
	int mins2h;
	int mins2m;

	if ( (0 != to_minutes_from_hour_string(&text[pmatch[1].rm_so], &mins1h))
		|| (0 != to_minutes_from_mins_string(&text[pmatch[2].rm_so], &mins1m))
		|| (0 != to_minutes_from_hour_string(&text[pmatch[3].rm_so], &mins2h))
		|| (0 != to_minutes_from_mins_string(&text[pmatch[4].rm_so], &mins2m)) ) {
		return 1;
	}

	int mins1 = mins1h + mins1m;
	int mins2 = mins2h + mins2m;

	if (mins1 < mins2) {
		int tmp = mins2;
		mins2 = mins1;
		mins1 = tmp;
	}

	*mins = mins1 - mins2;
	return 0;
}

/*
This is handling a situation where the text is HH:MM.  It will convert this to minutes
and write it into the supplied integer pointer.  It will return 1 on failure and 0 on
success.  The match 0 covers the whole text, match 1 covers the hours and match 2 covers
the minutes.
*/

int to_minutes_from_absolute(const char* text, const regmatch_t* pmatch, int* mins) {
	int minsh;
	int minsm;

	if ( (0 != to_minutes_from_hour_string(&text[pmatch[1].rm_so], &minsh))
		|| (0 != to_minutes_from_mins_string(&text[pmatch[2].rm_so], &minsm)) ) {
		return 1;
	}

	*mins = minsh + minsm;
	return 0;
}

int ipow10(int n) {
	int result = 1;
	while(n > 0) {
		result *= 10;
		n--;
	}
	return result;
}

/*
This will take something like "2.25" and will convert that into minutes such as 135 and if
it is successful it will write it into the supplied integer pointer.
*/

int to_minutes_from_decimal(const char* text, int* mins) {
	int minsh;
	int minsf = 0;
		// fractional part

	minsh = atoi(text) * 60;

	const char* dot = strchr(text, '.');

	if (NULL != dot) {
		minsf = (atoi(dot + 1) * 6) / ipow10(strlen(dot) - 2);
	}

	*mins = minsh + minsf;
	return 0;
}

/*
Returns 1 if the pmatch covers the entire length of the text.
*/

int matches_whole_string(const char* text, const regmatch_t* pmatch) {
	if (0 != pmatch[0].rm_so || strlen(text) != pmatch[0].rm_eo) {
		return 0;
	}
	return 1;
}

/*
This function will figure out what sort of text is being presented by the pattern
that matches it.  It will then convert the text into minutes and write it into the
supplied integer pointer returning 1 on failure and 0 on success.
*/

int to_minutes(const char* text, const regexes* rexs, int* mins) {
	regmatch_t pmatch[MAX_MATCHES];
	if (0 == regexec(&(rexs->range), text, MAX_MATCHES, pmatch, 0)) {
		if (1 == matches_whole_string(text, pmatch)) {
			return to_minutes_from_range(text, pmatch, mins);
		}
	}
	if (0 == regexec(&(rexs->absolute), text, MAX_MATCHES, pmatch, 0)) {
		if (1 == matches_whole_string(text, pmatch)) {
			return to_minutes_from_absolute(text, pmatch, mins);
		}
	}
	if (0 == regexec(&(rexs->decimal), text, MAX_MATCHES, pmatch, 0)) {
		if (1 == matches_whole_string(text, pmatch)) {
			return to_minutes_from_decimal(text, mins);
		}
	}
	fprintf(stderr, "! bad item [%s]\n", text);
	return 1;
}

int main(int argc, char** argv) {
	if (is_help(argc, argv)) {
		usage();
		return 0;
	}
	if (is_version(argc, argv)) {
		printf("%s\n", TIMESUM_VERSION);
		return 0;
	}
	regexes rexs;
	if (0 != init_regex(&rexs)) {
		return 1;
	}
	int mins = 0;
	for (int i = 1; i < argc; i++) {
		int minsi;
		if (0 != to_minutes(argv[i], &rexs, &minsi)) {
			return 1;
		}
		mins += minsi;
	}
	printf("%02d:%02d\n", mins / 60, mins % 60);
	return 0;
}
