#/bin/bash

for fl in *.h; do
    curl http://npapi-sdk.googlecode.com/svn/trunk/headers/$fl > $fl
done
