#!/bin/sh

exec 2>&1

set -e

debian/rules build-arch

test_exists() {
    #assertTrue "[ -f 'libAvKys/config.log' ]"
    log_file_exists="$(test -f libAvKys/config.log && echo 'file exists')"
    assertEquals 'file exists' "$log_file_exists"
}

test_equals() {
    quant_test_auto="$(find ./ -name test_auto | wc -l)"
    quant_succeeded="$(grep succeeded libAvKys/config.log | wc -l)"
    assertEquals "$quant_test_auto" "$quant_succeeded"
}

. shunit2
