/*
 * Copyright 2021, Andrew Lindesay <apl@lindesay.co.nz>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

package main

import (
	"fmt"
	"os"
	"regexp"
	"strconv"
	"strings"
)

var regexTimeMinusTime = regexp.MustCompile("^(\\d+):(\\d+)-(\\d+):(\\d+)$")
var regexTimeAbsolute = regexp.MustCompile("^(\\d+):(\\d+)$")
var regexTimeDecimal = regexp.MustCompile("^[\\d]+(\\.[\\d]+)?$")

func main() {

	if anyFlagPresentInArgs([]string{"-v", "--version"}) {
		fmt.Printf("%s\n", version)
		os.Exit(0)
	}

	if anyFlagPresentInArgs([]string{"-h", "--help"}) {
		fmt.Printf("%s\n", help)
		os.Exit(0)
	}

	minutes := 0
	for _, arg := range os.Args[1:] {
		if ok, argMinutes := toMinutes(arg); ok {
			minutes += argMinutes
		} else {
			fmt.Printf("! bad item [%s]\n", arg)
			os.Exit(1)
		}
	}
	fmt.Printf("%02d:%02d\n", minutes/60, minutes%60)
}

// Given the arguments to the program, this function will check to see
// if any of the supplied flags are present in the arguments.

func anyFlagPresentInArgs(flags []string) bool {
	for _, arg := range os.Args[1:] {
		for _, flag := range flags {
			if flag == arg {
				return true
			}
		}
	}
	return false
}

func toMinutes(str string) (bool, int) {
	if ok, mins := tryToMinutesForTimeMinusTime(str); ok {
		return true, mins
	}
	if ok, mins := tryToMinutesForAbsoluteTime(str); ok {
		return true, mins
	}
	if ok, mins := tryToMinutesForDecimal(str); ok {
		return true, mins
	}

	return false, 0
}

// parses and expression such as "2:45" and converts it to minutes; in this example,
// 165 minutes.

func tryToMinutesForAbsoluteTime(str string) (bool, int) {
	match := regexTimeAbsolute.FindStringSubmatch(str)

	if nil != match && match[0] == str {
		hours := trimAndAtoi(match[1])
		mins := trimAndAtoi(match[2])
		if mins <= 59 && hours <= 23 {
			return true, (hours * 60) + mins
		}
	}
	return false, 0
}

// parses an expression such as "2.75" and returns the quantity of minutes.  In
// this case it would be 2:45 or 165 minutes.

func tryToMinutesForDecimal(str string) (bool, int) {
	match := regexTimeDecimal.FindStringSubmatch(str)

	if nil != match && match[0] == str {
		dotIdx := strings.Index(str, ".")

		if -1 == dotIdx || dotIdx == len(str)-1 {
			return true, trimAndAtoi(str) * 60
		}

		hours := trimAndAtoi((str)[:dotIdx])
		frac := trimAndAtoi((str)[dotIdx+1:])
		fracStrLen := (len(str) - dotIdx) - 1
		return true, (hours * 60) + ((frac * 6) / iPow10(fracStrLen-1))
	}
	return false, 0
}

// parses an expression such as "12:30-10:30" and returns the quantity of minutes
// between these two times.

func tryToMinutesForTimeMinusTime(str string) (bool, int) {
	match := regexTimeMinusTime.FindStringSubmatch(str)

	if nil != match && match[0] == str {
		aHours := trimAndAtoi(match[1])
		aMins := trimAndAtoi(match[2])
		bHours := trimAndAtoi(match[3])
		bMins := trimAndAtoi(match[4])

		if aMins <= 59 && bMins <= 59 && aHours <= 23 && bHours <= 23 {
			aMinsTotal := (aHours * 60) + aMins
			bMinsTotal := (bHours * 60) + bMins
			return true, iAbs(bMinsTotal - aMinsTotal)
		}
	}
	return false, 0
}

// this function will trim away any leading whitespace and zeros.  Then it will
// parse the string as an integer.  If it fails to parse then it will return
// zero rather than error.

func trimAndAtoi(str string) int {
	value, err := strconv.Atoi(strings.TrimLeft(str, "0 "))
	if nil == err {
		return value
	}
	return 0
}

// returns the absolute value of the supplied integer.

func iAbs(v int) int {
	if v < 0 {
		return -v
	}
	return v
}

func iPow10(v int) int {
	result := 10
	for i := 1; i < v; i++ {
		result *= 10
	}
	return result
}
