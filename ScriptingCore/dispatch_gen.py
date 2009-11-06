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
wl("")

# prologue

wl("// Generated: " + date.today().isoformat())
wl("")
wl("namespace FB")
wl("{")
ind_in()

# wrapper

wl("namespace detail")
wl("{")
ind_in()

wl("template<class T>")
wl("struct strip_const {")
ind_in()
wl("typedef T type;")
ind_out()
wl("};")
wl("template<class T>")
wl("struct strip_const<const T> {")
ind_in()
wl("typedef T type;")
ind_out()
wl("};")
wl("template<class T>")
wl("struct strip_ref {")
ind_in()
wl("typedef T type;")
ind_out()
wl("};")
wl("template<class T>")
wl("struct strip_ref<T&> {")
ind_in()
wl("typedef T type;")
ind_out()
wl("};")
wl("")

for i in range(max_args+1):
	s = "template<class C"
	for i2 in range(i): 
		s += ", typename T"+str(i2)
	wl(s + ", typename F>")
	wl("struct "+methodWrapStructName+str(i)+" {")
	ind_in()
	wl("typedef FB::variant result_type;")
	wl("F f;")
	wl(methodWrapStructName+str(i)+"(F f) : f(f) {}")
	wl("FB::variant operator()(C* instance, std::vector<FB::variant>& in)")
	wl("{")
	ind_in()
	wl("if(in.size() != "+str(i)+") throw FB::invalid_arguments();")
	s = "return (instance->*f)(";
	if i>0:
		s+="in[0].convert_cast<strip_const<strip_ref<T0>::type>::type>()"
	for i2 in range(1,i):
		s+= ", in["+str(i2)+"].convert_cast<strip_const<strip_ref<T"+str(i2)+">::type>::type>()"
	wl(s + ");")
	ind_out()
	wl("}")
	ind_out()
	wl("};")
	
ind_out()
wl("} // namespace detail")
wl("")
	
# make_wrapper

for i in range(max_args+1):
	typenames = ""
	if i>0:
		typenames += "typename T0"
		for i2 in range(1,i):
			typenames += ", typename T"+str(i2)
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
	
	wl("template<class C, typename R"+typenames_app+">")
	wl("inline "+callMethodFunctorName)
	wl(makeWrapperFunctionName+"(C* instance, R (C::*function)("+types+"))")
	wl("{")
	ind_in()
	wl("return boost::bind(FB::detail::"+methodWrapStructName+str(i)+"<C"+types_app+", R (C::*)("+types+")>(function), instance, _1);")
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
