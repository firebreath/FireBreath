# ############################################################
# Original Author: Georg Fritzsche
#
# Created:    November 6, 2009
# License:    Dual license model; choose one of two:
#             New BSD License
#             http://www.opensource.org/licenses/bsd-license.php
#             - or -
#             GNU Lesser General Public License, version 2.1
#             http://www.gnu.org/licenses/lgpl-2.1.html
#
# Copyright 2009 Georg Fritzsche, Firebreath development team
# ############################################################

#!/usr/bin/python

import time
from datetime import date

def tern(a,b,c):
    i = 0
    if(a): i = 1
    return (b,c)[i]

# ######################################

max_args = 10
tab = "\t"
endl = "\n"
headerFileName          = "MethodConverter.h"
includeGuardName        = "METHOD_CONVERTER_H"
methodWrapStructName    = "method_wrapper"
makeWrapperFunctionName = "make_method"
callMethodFunctorName   = "FB::CallMethodFunctor"

indent = 0;
def ind_in():
    global indent
    indent += 1
def ind_out():
    global indent
    indent -= 1
def ind():
    return tab*indent

# ######################################
    
f = open(headerFileName, "w")

def wl(s):
    f.write(ind()+s+endl)

# legal

wl("/**********************************************************\\")
wl("Original Author: Georg Fritzsche")
wl("Generated on: " + date.today().isoformat())
wl("License:      Dual license model; choose one of two:")
wl("              New BSD License")
wl("              http://www.opensource.org/licenses/bsd-license.php")
wl("              - or -")
wl("              GNU Lesser General Public License, version 2.1")
wl("              http://www.gnu.org/licenses/lgpl-2.1.html")
wl("")
wl("Copyright 2009 Georg Fritzsche, Firebreath development team")
wl("\\**********************************************************/")
wl("")
    
# start
    
wl("")
wl("#if !defined("+includeGuardName+")")
wl("#define "+includeGuardName)
wl("")
wl("#if defined(_MSC_VER)")
wl("#  pragma once")
wl("#endif")
wl("")

# includes

wl("#include <boost/function.hpp>")
wl("#include <boost/bind.hpp>")
wl("#include \"ConverterUtils.h\"")
wl("")

# prologue

wl("namespace FB")
wl("{")
ind_in()

# wrapper

wl("namespace detail { namespace methods")
wl("{")
ind_in()

wl("using FB::detail::plain_type;")
wl("using FB::convertArgument;")
wl("")

for i in range(max_args+1):
    for voidReturn in [False, True]:
        s = "template<class C"
        if not voidReturn:
            s += ", class R"
        for i2 in range(i): 
            s += ", class T"+str(i2)
        wl(s + ", class F>")
        
        s = "struct "+methodWrapStructName+str(i)
        if voidReturn: # specializing on void return type
            s += "<C, void"
            for i2 in range(i): 
                s += ", T"+str(i2)
            s += ", F>"
        wl(s)
        
        wl("{")
        ind_in()
        
        wl("typedef FB::variant result_type;")
        wl("F f;")
        wl(methodWrapStructName+str(i)+"(F f) : f(f) {}")
        
        wl("FB::variant operator()(C* instance, const FB::VariantList& in)")
        wl("{")
        ind_in()
        if i<1:
            wl("if(in.size() != 0)")
        else:
            wl("typedef typename plain_type<T"+str(i-1)+">::type TLast;")
            wl("if(!checkArgumentCount<TLast>(in, "+str(i)+"))") 
        wl(tab+"return FB::variant();")
        
        if voidReturn: # specializing on void return type
            s = "(instance->*f)("
        else:
            s = "return (instance->*f)(";
    #    if i>0:
    #        s+="in[0].convert_cast<typename FB::detail::plain_type<T0>::type>()"
    #    for i2 in range(1,i):
    #        s+= ", in["+str(i2)+"].convert_cast<typename FB::detail::plain_type<T"+str(i2)+">::type>()"
        if i<1:
            wl(s+");")
        else:
            wl(s)
            ind_in()
            s = "convertArgument<typename plain_type<T0>::type>(in[0], 1)"
            if i>1:
                for i2 in range(1,i-1):
                    wl(s+",")
                    s = "convertArgument<typename plain_type<T"+str(i2)+">::type>(in["+str(i2)+"], "+str(i2+1)+")"
                wl(s+",")
            wl("convertLastArgument<TLast>(in, "+str(i)+"));")
            ind_out()
        if voidReturn: # specializing on void return type 
            wl("return FB::variant();")
        ind_out()
        wl("}")
        ind_out()
        wl("};")
    
ind_out()
wl("} } // namespace detail::methods")
wl("")
    
# make_wrapper

for i in range(max_args+1):
    for constness in ['', ' const']:
        typenames = ""
        if i>0:
            typenames += "class T0"
            for i2 in range(1,i):
                typenames += ", class T"+str(i2)
        typenames_app = ""
        if i>0:
            typenames_app = ", "+typenames
        types = ""
        if i>0:
            types += "T0"
            for i2 in range(1,i):
                types += ", T"+str(i2)
        print " * "+types
        types_app = ""
        if i>0: 
            types_app = ", "+types
        
        wl("template<class C, class R"+typenames_app+">")
        wl("inline "+callMethodFunctorName)
        wl(makeWrapperFunctionName+"(C* instance, R (C::*function)("+types+")"+constness+")")
        wl("{")
        ind_in()
        wl("return boost::bind(FB::detail::methods::"+methodWrapStructName+str(i)+"<C, R"+types_app+", R (C::*)("+types+")"+constness+">(function), instance, _1);")
        ind_out()
        wl("}")
        wl("")

# epilogue

ind_out()
wl("} // namespace FB")
wl("")

wl("#endif //"+includeGuardName)
wl("")

f.close()
