#!/bin/sh
# =============================================================================
# Copyright 2020, Andrew Lindesay <apl@lindesay.co.nz>.
# All rights reserved. Distributed under the terms of the MIT License.
# =============================================================================

testreturn=0

runAndFail() {
  input=$1
  ./timesum ${input}

  if [ $? -eq 0 ]; then
    echo "! expected failure when running [${input}]"
    testreturn=1
    return
  fi

  echo "[${input}] failed as expected -- OK"
  testreturn=0

  return
}

runAndCheck() {
  input=$1
  expected=$2
  actual=$(./timesum ${input})

  if [ $? != 0 ]; then
    echo "! failed to run [${input}]"
    testreturn=1
    return
  fi

  if [ ${actual} != ${expected} ]; then
    echo "! was expecting [${input}] --> [${expected}] but was [${actual}]"
    testreturn=1
    return
  fi

  echo "[${input}] --> [${expected}] -- OK"
  testreturn=0

  return
}

result=0

runAndCheck '07:30-5:45 1:15 1.75' '04:45'
result=$(expr ${result} + ${testreturn})
runAndCheck '1234.333' '1234:19'
result=$(expr ${result} + ${testreturn})
runAndCheck '1234.333334' '1234:20'
result=$(expr ${result} + ${testreturn})
runAndCheck '00:15-00:05' '00:10'
result=$(expr ${result} + ${testreturn})
runAndCheck '00:5-00:15' '00:10'
result=$(expr ${result} + ${testreturn})
runAndCheck '00:5-00:05' '00:00'
result=$(expr ${result} + ${testreturn})
runAndCheck '10:10' '10:10'
result=$(expr ${result} + ${testreturn})

runAndFail '25:10'
result=$(expr ${result} + ${testreturn})
runAndFail '10:61'
result=$(expr ${result} + ${testreturn})
runAndFail '.123'
result=$(expr ${result} + ${testreturn})
runAndFail '25:19-10:20'
result=$(expr ${result} + ${testreturn})

if [ ${result} -eq 0 ]; then
  echo "all tests passed"
else
  echo "${result} tests failed"
fi

exit ${result}
