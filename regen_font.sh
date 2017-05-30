#!/bin/sh

node_modules/.bin/fctx-compiler assets/LECO1976-Regular.svg -r "[0-9a-zA-Z \-/:%.]"
mv resources/LECO1976-Regular.ffont resources/LECO1976-Regular~aplite.ffont
node_modules/.bin/fctx-compiler assets/LECO1976-Regular.svg -r "[0-9a-zA-Z \-\\/:%°.\[\](){}<>]"
