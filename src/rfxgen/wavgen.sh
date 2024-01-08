#!/bin/bash

find . -type f -name "*.rfx" | cut -f 2 -d '.' | cut -f 2 -d '/' | while read -r file; do rfxgen --input ${file}.rfx --output ${file}.wav; done
