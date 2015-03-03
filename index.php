<?php

/**
 * @file
 * The PHP page that serves all page requests on a Drupal installation.
 *
 * The routines here dispatch control to the appropriate handler, which then
 * prints the appropriate page.
 *
 * All Drupal code is released under the GNU General Public License.
 * See COPYRIGHT.txt and LICENSE.txt.
 */
//drupal_add_http_header('X-UA-Compatible', 'IE=edge');
header('X-UA-Compatible: IE=edge');
//echo "hello";
//exit();

/**
 * Root directory of Drupal installation.
 */
define('DRUPAL_ROOT', getcwd());

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
/*
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past
drupal_add_http_header('X-UA-Compatible', 'IE=edge');
drupal_add_http_header('Status', '410 Gone');
echo "HELLO";
drupal_exit();
*/
drupal_add_http_header('X-UA-Compatible', 'IE=edge');
menu_execute_active_handler();
