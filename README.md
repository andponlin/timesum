# timesum

## Overview

When you are working to a timesheet it can be quite tedious to track your time.  Some people maintain a sheet of paper where a list is maintained of when work started and stopped across various projects.  At the end of the day this has to be tallied and entered into some time-keeping software system.  Doing the tally is time-consuming and a bit tedious.

This tool is designed to help with this.

```
$ timesum "06:45-07:45" "8:45-12:30" "12:45-15:10"
07:10
```

It also handles items with an absolute HH:MM or a fractional value;

```
./timesum 07:30-10:30 1:30 1.25
05:45
```

## Building

This tool should build on most UNIX systems with a C compiler and make.  To build, just issue the `make` command and you should end up with a binary `timesum`.