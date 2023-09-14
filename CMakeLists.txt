# Copyright (c) 2023, Sveta Smirnova. All rights reserved.
# 
# This file is part of simple_rewrite_plugin.
#
# simple_rewrite_plugin is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# simple_rewrite_plugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with simple_rewrite_plugin.  If not, see <http://www.gnu.org/licenses/>.

IF(DEFINED MYSQL_NO_DASH_VERSION)
    IF("5.7.4" VERSION_LESS ${MYSQL_NO_DASH_VERSION})
    MYSQL_ADD_PLUGIN(simple_rewrite_plugin simple_rewrite_plugin.cc
      MODULE_ONLY MODULE_OUTPUT_NAME "simple_rewrite_plugin")
  ENDIF()
ENDIF()
