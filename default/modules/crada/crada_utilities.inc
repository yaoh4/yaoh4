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
	$output = $timestamp . " " . $data;

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
  drupal_add_http_header('X-UA-Compatible', 'IE=edge');

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

/**
 * Gibberish AES, a PHP Implementation
 * See Gibberish AES javascript encryption library, @link https://github.com/mdp/gibberish-aes
 *
 * This implementation is based on initial code proposed by nbari at dalmp dot com
 * @link http://www.php.net/manual/en/function.openssl-decrypt.php#107210
 *
 * OpenSSL php extension is required */
class GibberishAES
{
    protected $_nKeySize = 256;            // The key size in bits
    protected static $valid_key_sizes = array(128, 192, 256);   // Sizes in bits

    function __construct()
    {}

    /**
     * Encrypt AES (256, 192, 128)
     * @param $string string
     * @param $key string algorithm encryption
     * @return string base64 encoded encrypted cipher
     */
    function encrypt($string, $key)
    {
        $salt = openssl_random_pseudo_bytes(8);

        $salted = '';
        $dx = '';

        // Lengths in bytes:
        $key_length = (int) ($this->_nKeySize / 8);
        $block_length = 16; // 128 bits, iv has the same length.
        // $salted_length = $key_length (32, 24, 16) + $block_length (16) = (48, 40, 32)
        $salted_length = $key_length + $block_length;

        while (strlen($salted) < $salted_length)
        {
            $dx = md5($dx.$key.$salt, true);
            $salted .= $dx;
        }

        $key = substr($salted, 0, $key_length);
        $iv = substr($salted, $key_length, $block_length);

        return base64_encode('Salted__' . $salt . openssl_encrypt($string, "aes-".$this->_nKeySize."-cbc", $key, true, $iv));
    }

    /**
     * Decrypt AES (256, 192, 128)
     * @param $string base64 encoded cipher
     * @param $key string algorithm encryption
     * @return dencrypted string
     */
    function decrypt($string, $key)
    {
        // Lengths in bytes:
        $key_length = (int) ($this->_nKeySize / 8);
        $block_length = 16;

        $data = base64_decode($string);
        $salt = substr($data, 8, 8);
        $encrypted = substr($data, 16);

        /**
         * From https://github.com/mdp/gibberish-aes
         *
         * Number of rounds depends on the size of the AES in use
         * 3 rounds for 256
         *     2 rounds for the key, 1 for the IV
         * 2 rounds for 128
         *     1 round for the key, 1 round for the IV
         * 3 rounds for 192 since it's not evenly divided by 128 bits
         */
        $rounds = 3;
        if (128 === $this->_nKeySize)
        {
            $rounds = 2;
        }

        $data00 = $key.$salt;
        $md5_hash = array();
        $md5_hash[0] = md5($data00, true);
        $result = $md5_hash[0];

        for ($i = 1; $i < $rounds; $i++)
        {
            $md5_hash[$i] = md5($md5_hash[$i - 1].$data00, true);
            $result .= $md5_hash[$i];
        }

        $key = substr($result, 0, $key_length);
        $iv = substr($result, $key_length, $block_length);

        return openssl_decrypt($encrypted, "aes-".$this->_nKeySize."-cbc", $key, true, $iv);
    }

    /**
     * Sets the key-size for encryption/decryption in number of bits
     * @param  $nNewSize int The new key size. The valid integer values are: 128, 192, 256 (default) */
    function setMode($nNewSize)
    {
        if (is_null($nNewSize) || empty($nNewSize) || !is_int($nNewSize) || !in_array($nNewSize, self::$valid_key_sizes))
            return;

        $this->_nKeySize = $nNewSize;
    }
}

function br2nl($string)
{
    return preg_replace('/\<br(\s*)?\/?\>/i', "\n", $string);
}
