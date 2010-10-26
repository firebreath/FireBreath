#/bin/bash

for fl in *.h; do
    curl http://npapi-headers.googlecode.com/svn/trunk/$fl > $fl
done
