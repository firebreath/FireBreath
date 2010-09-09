If you are changing any files in this directory, it should be things
that should have been in FireBreath to start out with -- things that
you might consider contributing back (whether you are actually going
to or not is your business).  It should not be things that are
specific to your project.

Things specific to your project should go into your project directory.
Most of the classes here that may need to be customized (such as the
PluginWindow, NpapiPlugin, or PluginCore classes) can be replaced in
your plugin by changing which class you return in your Factory.cpp
files in your plugin directory.
