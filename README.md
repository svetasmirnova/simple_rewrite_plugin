# simple_rewrite_plugin
Introduction
============

Query rewriter plugin that rewrites any query to any other query. This plugin should be used for testing purposes only and not in production! It can easily crash your MySQL server.

Compling
========

Place plugin directory into plugin subdirectory of MySQL source directory, then cd to MySQL source directory and compile server. Plugin will be compiled automatically. You need to use GCC 4.9 or newer  or any other compiler which supports [std::regex] () to compile plugin.

Installation
============

Copy simple_rewrite_plugin.so into plugin directory of your MySQL server, then login and type:

    INSTALL PLUGIN simple_rewrite_plugin SONAME 'simple_rewrite_plugin.so';

Uninstallation
==============

Connect to MySQL server and run:

    UNINSTALL PLUGIN simple_rewrite_plugin;
    
Usage examples
==============

    SET simple_rewrite_plugin_action='rewrite';
    -- You can use regular expressions here
    SET simple_rewrite_plugin_pattern='(SET NAMES) \"?([[:alnum:]]+)\"?';
    -- Regular expressions placeholders are supported
    SET simple_rewrite_plugin_query='$1 $2$2';

    -- Do not rewrite the query, intentionally crash the server instead.
    SET simple_rewrite_plugin_action='abort';
