\W
INSTALL PLUGIN simple_rewrite_plugin SONAME 'simple_rewrite_plugin.so';
SHOW VARIABLES LIKE 'simple_rewrite_plugin%';
SET simple_rewrite_plugin_pattern='(SET NAMES) \"?([[:alnum:]]+)\"?';
SET NAMES utf8mb4;
SET simple_rewrite_plugin_query='$1 $2$2';
SHOW VARIABLES LIKE 'simple_rewrite_plugin%';
SET NAMES utf8mb4;
SELECT 'Setting plugin action to "abort"';
SET simple_rewrite_plugin_action='abort';
SHOW VARIABLES LIKE 'simple_rewrite_plugin%';
SET NAMES utf8mb4;
