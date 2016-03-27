<?php 
/*****************************************************
* bondinfo_server.php
*
* PHPRPC server of bond information application
*
* @author   Wang Yun
* @date     4/23/2010
* @version  1.0
* @edit history by Wang Yun
* 1) Use database connnect variable defined in config file  
*                                    6/9/2010
* 2) Add interface for curve fitting
*                                    8/6/2010
* 3) Removed credential information to host codes in github
*                                    3/26/2016
******************************************************/


/*
 * Requried files
 */
require_once ("/var/www/html/php/phprpc_server.php");
require_once ("./DBAccess.php");
require_once ("/var/www/html/config/finance_config.php");

/**
*getBondInfo
* Released function for client to get Bond info 
* @sql:     Database operation query
* @return:  return reason when error occurred
*/
function getBondInfo($sql) {
    $dbAccessor = new DBAccess();
    // Link variables read from config
	global $bondinfo_dbname;
	global $bondinfo_dbhost;
	global $bondinfo_dbuser;
	global $bondinfo_dbpw;
	// Link database
    $dbAccessor->connDatabase($bondinfo_dbhost, $bondinfo_dbuser, $bondinfo_dbpw, $bondinfo_dbname);
    // Set encoding method
    $dbAccessor->sendQuery("set names utf8");
    // Send client query
    $dbAccessor->sendQuery($sql);
    // Return result
    $ret = $dbAccessor->fetchRowAll();
    // Clear
    $dbAccessor->clearResource();
    return $ret;
}
function getFittingInfo($sql, $yield, $knots) {
    $data_x = array();              // Fitting co-variables would be 'remain' field 
    $data_y = array();              // Fitting response variables would be 'pre_com_int' or 'later_com_int'    
    $fitting_info = getBondInfo($sql);
    
    $data_x[0] = -1;
    $data_y[0] = 0;
    for ($i = 0; $i < $fitting_info->rows; $i++) {
        // By the same time dump 'remain' & 'pre_com_int' / 'later_com_int' into data_x & data_y
        $data_x[($i + 1)] = (double) $fitting_info->data[$i]["remain"];
        $data_y[($i + 1)] = (double) $fitting_info->data[$i][$yield];
    }

    $param = yield_curve_fitting($data_x, $data_y, $knots);         // Solve curve fitting by extension lib

    return $param;
}

// Common steps to start PHPRPC server
$server = new PHPRPC_Server(); 
$server->add('getBondInfo');
$server->add('getFittingInfo');
$server->setDebugMode(true); 
$server->start();
?> 