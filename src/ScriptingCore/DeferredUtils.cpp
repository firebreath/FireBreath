/**********************************************************\ 
 Original Author: Richard Bateman (taxilian)

 Created:    Mar 4, 2015
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2015 Richard Bateman and the FireBreath Dev Team
\**********************************************************/

#include "DeferredUtils.h"

using FB::variantPromise;
using FB::VariantPromiseList;

struct whenData
{
    whenData(size_t totalCount) : results(totalCount), doneCount(0), totalCount(totalCount) {}
    bool settled() { return doneCount >= totalCount; }
    FB::VariantList results;
    size_t doneCount;
    size_t totalCount;
};

variantPromise FB::whenAllPromises(VariantPromiseList args, std::function<variantPromise(VariantList)> cb) {
    FB::variantDeferred retDfd;
    auto count = args.size();
    std::shared_ptr<whenData> data{ std::make_shared<whenData>(count) };
    
    size_t n = 0;
    for (auto arg : args) {
        arg.done([n, data, cb, retDfd](FB::variant var) -> void {
            data->results[n] = var;
            data->doneCount++;
            if (data->settled()) {
                // The VariantList is fully resolved!
                try {
                    retDfd.resolve(cb(data->results));
                } catch (std::exception e) {
                    auto ep = std::current_exception();
                    retDfd.reject(ep);
                }
            }
        });
        arg.fail([retDfd](std::exception_ptr e) -> void {
            retDfd.reject(e);
        });
        ++n;
    }
    if (!n) {
        retDfd.resolve(cb(FB::VariantList()));
    }
    return retDfd.promise();
}
