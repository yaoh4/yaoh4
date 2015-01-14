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

  // Time format is UNIX timestamp or
  // PHP strtotime compatible strings
function dateDiff($time1, $time2, $precision = 6) {
    // If not numeric then convert texts to unix timestamps
    if (!is_int($time1)) {
      $time1 = strtotime($time1);
    }
    if (!is_int($time2)) {
      $time2 = strtotime($time2);
    }

    // If time1 is bigger than time2
    // Then swap time1 and time2
    if ($time1 > $time2) {
      $ttime = $time1;
      $time1 = $time2;
      $time2 = $ttime;
    }

    // Set up intervals and diffs arrays
    $intervals = array('year','month','day','hour','minute','second');
    $diffs = array();

    // Loop thru all intervals
    foreach ($intervals as $interval) {
      // Create temp time from time1 and interval
      $ttime = strtotime('+1 ' . $interval, $time1);
      // Set initial values
      $add = 1;
      $looped = 0;
      // Loop until temp time is smaller than time2
      while ($time2 >= $ttime) {
        // Create new temp time from time1 and interval
        $add++;
        $ttime = strtotime("+" . $add . " " . $interval, $time1);
        $looped++;
      }
      $time1 = strtotime("+" . $looped . " " . $interval, $time1);
      $diffs[$interval] = $looped;
    }

    $count = 0;
    $times = array();
    // Loop thru all diffs
    foreach ($diffs as $interval => $value) {
      // Break if we have needed precission
      if ($count >= $precision) {
break;
      }
      // Add value and interval
      // if value is bigger than 0
      if ($value > 0) {
// Add s if value is not 1
if ($value != 1) {
  $interval .= "s";
}
// Add value and interval to times array
$times[] = $value . " " . $interval;
$count++;
      }
    }

    // Return string with times
    return implode(", ", $times);
  }

