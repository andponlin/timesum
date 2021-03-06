/*
 * Copyright 2020, Andrew Lindesay <apl@lindesay.co.nz>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

package main

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestTryToMinutesForAbsoluteTime_happyDays(t *testing.T) {
	// GIVEN
	const input = "10:45"

	// WHEN
	hit, mins := tryToMinutesForAbsoluteTime(input)

	// THEN
	assert.Equal(t, true, hit)
	assert.Equal(t, 645, mins)
}

func TestTryToMinutesForDecimal_happyDays(t *testing.T) {
	// GIVEN
	const input = "2.75"

	// WHEN
	hit, mins := tryToMinutesForDecimal(input)

	// THEN
	assert.Equal(t, true, hit)
	assert.Equal(t, 165, mins)
}

func TestTryToMinutesForTimeMinusTime_happyDays(t *testing.T) {
	// GIVEN
	const input = "13:30-10:45"

	// WHEN
	hit, mins := tryToMinutesForTimeMinusTime(input)

	// THEN
	assert.Equal(t, true, hit)
	assert.Equal(t, 165, mins)
}
