FireBreath
==========

Home page: [http://www.firebreath.org](http://www.firebreath.org)

FireBreath is a cross-platform browser plugin framework. It supports ActiveX and NPAPI on Windows, Mac OS X, and Linux (probably other unix-derivitives as well).

[![Flattr this project](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=taxilian&url=https://github.com/firebreath/FireBreath&title=FireBreath&language=&tags=github&category=software)

Getting the source
==================

After checking out the source, make sure you run:

    git submodule update --init --recursive

in the project root to get the linked repositories.  The main one you want is the firebreath-boost repository.  You don't need firebreath-boost if you use WITH_SYSTEM_BOOST.

Using Subversion
================

If you choose to use subversion to check out FireBreath, make sure you also check out the firebreath-boost project as src/3rdParty/boost/ (you'll have to remove the existing directory).

Note on Building Windows Projects
=================================

In order to build Windows projects, you need:
 * [WiX Toolset 3.7](http://wix.codeplex.com/releases/view/99514) (higher versions cause problems)
 * [ATLHarvester](https://wix.codeplex.com/SourceControl/network/forks/roberthyang/NewHeatExtension/latest#ATLHarvesterExtension/readme.txt) extension, which adds x64 support to Heat.
 * Python 2.7 with lxml

Good luck!
