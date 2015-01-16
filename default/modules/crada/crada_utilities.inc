<?php

//============================================================+
// File name   : crada_utilities.inc
// Begin       : 2014-10-14
// Last Update : 2014-10-14
//
// Description : Common php functions for CRADA Builder
//============================================================+
function crada_log_sql_error($e) {
  $filename = "/local/drupal/logs/crada.log";

  print_backtrace($filename);

  file_put_contents($filename, json_encode($e) . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  file_put_contents($filename,  "MYSQL Exception Details" . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  $output = print_r($document_roles, true);
  file_put_contents($filename, "MYSQL Error Number: ".$e->errorInfo[1] . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  file_put_contents($filename, $e->errorInfo[2] . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  file_put_contents($filename, "SQL QUERY: " . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  file_put_contents($filename, $e->query_string . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
  file_put_contents($filename, "ARGUMENTS: ".print_r($e->args, true) . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );

}

function crada_log($data, $debug_level=0) {

	$filename = "/local/drupal/logs/crada.log";
	$timestamp = date("Y-m-d H:i:s", time());
	$output = $timestamp . " " . $data ;

	if($debug_level > 0 ) {
		file_put_contents($filename,  "*******************************************" . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
	}

	file_put_contents($filename,  $output . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );

	if($debug_level > 0 ) {
    print_backtrace($filename);
	}

}


function print_backtrace($filename) {

  file_put_contents($filename, "debug_backtrace()" . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
    foreach (debug_backtrace() as $key => $value) {
      file_put_contents($filename,    $key . ": " . json_encode($value).PHP_EOL , FILE_USE_INCLUDE_PATH | FILE_APPEND );
    }

}

function crada_include() {
  //
  // Base include files
  //
  $module_path = drupal_get_path('module', 'crada');
  drupal_add_library('system','ui.dialog');
  drupal_add_css($module_path . '/crada.css');
  drupal_add_js($module_path . '/crada_helper_functions.js', array('cache' => false, 'preprocess' =>false));
  //drupal_add_css('sites/all/libraries/bootstrap/css/bootstrap.css'); //This messed everything up.
  drupal_add_css('sites/all/libraries/fontawesome/css/font-awesome.css');

}

function howLongAgo($unixTimestamp) {
/* Examples:
    12 hours 55 min ago
    1 week 2 days
    38 min ago
    1 week 2 day
    6 days 20 hours ago
    4 months 1 week 4 days 1 hour ago
*/
    $updated = date("Y-m-d H:i:s", $unixTimestamp);
    $current =date("Y-m-d H:i:s", time());
    $diff=date_diff(date_create($updated), date_create($current));
    $message = $diff->format("%R%a days");

    $year    = $diff->format('%y');
    $month    = $diff->format('%m');
    $day      = $diff->format('%d');
    $hour     = $diff->format('%h');
    $min      = $diff->format('%i');
    $sec      = $diff->format('%s');

    $year_label =($year == 1 ? 'year' : 'years');
    $month_label =($month == 1 ? 'month' : 'months');
    $month_label =($month == 1 ? 'month' : 'months');
    $day_label =($day == 1 ? 'day' : 'days');
    $hour_label =($hour == 1 ? 'hour' : 'hours');
    $min_label =($min == 1 ? 'min' : 'mins');
    $sec_label =($sec == 1 ? 'sec' : 'secs');
    //Less than a minute
    if(($year+$month+$day+$hour+$min) == 0) {
        $message = "$sec $sec_label ago";
    }
    //Less than an hour
    elseif(($year+$month+$day+$hour) == 0) {
        $message = "$min $min_label $sec $sec_label ago";
    }
    //Less than a day
    elseif(($year+$month+$day) == 0) {
        $message = "$hour $hour_label $min $min_label ago";
    }
    //Less than a month
    elseif(($year+$month) == 0) {
        $message = "$day $day_label $hour $hour_label ago";
    }
    //Less than a year
    elseif(($year) == 0) {
        $message = "$month $month_label $day $day_label";
    }
    //Over a year
    elseif(($year) > 0) {
        $message = " > $year $year_label";
    }

  return $message;
}
