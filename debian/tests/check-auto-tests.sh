#!/bin/sh

set -e

debian/rules override_dh_auto_configure
dh_auto_build -a -O--buildsystem=qmake

if [ ! -f 'libAvKys/config.log' ]
then
	exit 1
fi

quant_test_auto="$(find ./ -name test_auto | wc -l)"
quant_succeeded="$(grep succeeded libAvKys/config.log | wc -l)"

if [ "$quant_test_auto" -ne "$quant_succeeded" ]
then
    exit 1
fi

echo "$quant_test_auto == $quant_succeeded"

exit 0
