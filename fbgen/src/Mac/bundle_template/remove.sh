#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"
rm -rf @BNDL_NAME@

G_MANIFEST_PATH=/Library/Google/Chrome/NativeMessagingHosts/@PLG_CRX_NAME@.json
if [ -f "$G_MANIFEST_PATH" ]; then
    rm -f "$G_MANIFEST_PATH"
fi

L_MANIFEST_PATH=~/Library/Application\ Support/Google/Chrome/NativeMessagingHosts/@PLG_CRX_NAME@.json
if [ -f "$L_MANIFEST_PATH" ]; then
    rm -f "$L_MANIFEST_PATH"
fi
