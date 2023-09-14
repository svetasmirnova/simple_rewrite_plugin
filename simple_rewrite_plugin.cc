/*
* Copyright (c) 2023, Sveta Smirnova. All rights reserved.
* 
* This file is part of simple_rewrite_plugin.
*
* simple_rewrite_plugin is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* simple_rewrite_plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with simple_rewrite_plugin.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef MYSQL_SERVER
#define MYSQL_SERVER
#endif

#include <ctype.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <regex>

#include <mysql/plugin.h>
#include <mysql/plugin_audit.h>
#include <mysql/service_mysql_alloc.h>
#include <my_thread.h> // my_thread_handle needed by mysql_memory.h
#include <mysql/psi/mysql_memory.h>
#include <typelib.h>

using namespace std;
using namespace std::regex_constants;
static MYSQL_PLUGIN plugin_info_ptr;

/* instrument the memory allocation */
#ifdef HAVE_PSI_INTERFACE
static PSI_memory_key key_memory_simple_rewrite;

static PSI_memory_info all_rewrite_memory[]=
{
    { &key_memory_simple_rewrite, "simple_rewrite", 0, 0, PSI_DOCUMENT_ME } 
};

static int simple_rewrite_plugin_init(MYSQL_PLUGIN plugin_ref)
{
  plugin_info_ptr= plugin_ref;
  const char* category= "sql";
  int count;

  count= array_elements(all_rewrite_memory);
  mysql_memory_register(category, all_rewrite_memory, count);
  return 0; /* success */
}

#else

#define plugin_init NULL
#define key_memory_simple_rewrite PSI_NOT_INSTRUMENTED

#endif /* HAVE_PSI_INTERFACE */

/* Declarations */
void _rewrite_query(const struct mysql_event_parse *event_parse, char const* new_query);

/* System variables */

static const char* supported_actions[] = {
    "rewrite",
    "abort",
    NullS
};

static TYPELIB supported_actions_typelib = {
    array_elements(supported_actions) - 1,
    "supported_actions_typelib",
    supported_actions,
    NULL
};

enum supported_actions_t {
    REWRITE,
    ABORT,
};

static MYSQL_THDVAR_STR(pattern, /* pattern to rewrite */ PLUGIN_VAR_MEMALLOC, "Pattern to identify query that needs to be rewritten. Default: \"\"", NULL, NULL, "");
static MYSQL_THDVAR_STR(query, /* pattern to rewrite */ PLUGIN_VAR_MEMALLOC, "Rewritten query. Default: \"\"", NULL, NULL, "");
static MYSQL_THDVAR_ENUM(action, /* action to perform */ PLUGIN_VAR_RQCMDARG, "Action to perform: rewrite query or abort server. Possible values: rewrite|abort", NULL, NULL, REWRITE, &supported_actions_typelib);

static struct SYS_VAR *simple_rewrite_plugin_sys_vars[] = {
    MYSQL_SYSVAR(pattern),
    MYSQL_SYSVAR(query),
    MYSQL_SYSVAR(action),
    NULL
};

/* The job */
static int simple_rewrite(MYSQL_THD thd, mysql_event_class_t event_class,
                          const void *event)
{
  assert(event_class == MYSQL_AUDIT_PARSE_CLASS);

  const struct mysql_event_parse *event_parse=
    static_cast<const struct mysql_event_parse *>(event);

  if (event_parse->event_subclass != MYSQL_AUDIT_PARSE_PREPARSE)
    return 0;
  
  const char* pattern_value = (const char *) THDVAR(thd, pattern);
  const char* query_value   = (const char *) THDVAR(thd, query);
  ulong action_value = (ulong) THDVAR(thd, action);

  string subject= event_parse->query.str;
  string rewritten_query;

  switch(action_value) {
      case REWRITE:
          if (strlen(pattern_value) > 0 && strlen(query_value) > 0) {
            regex query_re(pattern_value, ECMAScript | icase);
            rewritten_query = regex_replace(subject, query_re, query_value);
            _rewrite_query(event_parse, rewritten_query.c_str());
          }
          break;
      case ABORT:
          if (strlen(pattern_value) > 0) {
            regex query_re(pattern_value, ECMAScript | icase);
            if (regex_match(subject, query_re)) {
                assert(0 == 1);
            }
          }
          break;
  }
  
  return 0;
}

static st_mysql_audit simple_rewrite_plugin_descriptor= {
  MYSQL_AUDIT_INTERFACE_VERSION,  /* interface version */
  nullptr,
  simple_rewrite,                 /* implements rewrite */
  { 0, 0, (unsigned long) MYSQL_AUDIT_PARSE_ALL,}
};

mysql_declare_plugin(simple_rewrite_plugin)
{
  MYSQL_AUDIT_PLUGIN,
  &simple_rewrite_plugin_descriptor,
  "simple_rewrite_plugin",
  "Sveta Smirnova",
  "Rewriting any querry to any query based on system variables",
  PLUGIN_LICENSE_GPL,
  simple_rewrite_plugin_init,
  nullptr,                        /* simple_rewrite_plugin_deinit - TODO */
  nullptr,
  0x0001,                         /* version 0.0.1      */
  nullptr,                        /* status variables   */
  simple_rewrite_plugin_sys_vars, /* system variables   */
  nullptr,                        /* config options     */
  0,                              /* flags              */
}
mysql_declare_plugin_end;


// private functions

void _rewrite_query(const struct mysql_event_parse *event_parse, char const* new_query)
{
  char *rewritten_query= static_cast<char *>(my_malloc(key_memory_simple_rewrite, strlen(new_query) + 1, MYF(0)));
  strncpy(rewritten_query, new_query, strlen(new_query));
  rewritten_query[strlen(new_query)]= '\0';
  event_parse->rewritten_query->str= rewritten_query;
  event_parse->rewritten_query->length=strlen(new_query);
  *((int *)event_parse->flags)|= (int)MYSQL_AUDIT_PARSE_REWRITE_PLUGIN_QUERY_REWRITTEN;
}


/* vim: set tabstop=2 shiftwidth=2 softtabstop=2: */
/* 
* :tabSize=2:indentSize=2:noTabs=true:
* :folding=custom:collapseFolds=1: 
*/
