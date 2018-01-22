#!/bin/bash
set -e
set -u

for icon in $(find * -type f -name *.svg -print)
  echo "    <file>$icon</file>" >> numix.gresource.xml
do
done

