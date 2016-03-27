<?php
/*****************************************************
* /jinrong/bond/scatter_data.php
*
* Display data for flash scatter.swf
* 
* @author   Wang Yun
* @date     8/6/2010
* 
******************************************************/

require_once ("/var/www/html/config/config.php");    // Mid layer conf interface
require_once ("/var/www/html/php/phprpc_client.php");// PHPRPC client

// Format output flash variables parameters
function flash_data_format($name, $value) {
    echo "&" . $name . "=" . $value;
}
// Calc interger upper border of input
function upper_border($input) {
    $upper = (int)$input + 1;
    if (($upper - (double)$input) < 0.1)
    {
        $upper++;
    }
    return $upper;
}   
?>
<?
    /*
     * Main process
     */

    // Const variables
    $BSPLINE_NUM = 4;
    $NATLOG_NUM = 2;
    $INTERVAL_NUM = 10;
    $knots  = array((double)-1, (double)0, (double)1);      // Knots for B-spline fitting    
    
    // Input variables
    $yield = $_POST["yield"];                    // Yield type $_GET["yield"];
    $conditions = $_POST["conditions"];          // Extra conditions $_GET["cond"];
    $conditions = str_replace("\\", "", $conditions);
    $conditions = str_replace("A ", "A+", $conditions);
    //$yield = "pre_com_int";
    //$conditions = "credit >= 'A' and credit <= 'AAA' and zhesuanlv >= 0.5 and zhesuanlv <= 0.99";
    // Create PHPRPC_Client
    $midLayerAddr=GetMidLayerAddr();            // get mid layer address
    $client = new PHPRPC_Client($midLayerAddr.'jinrong/bond/bondinfo_server.php');

    // Data fields used in this scope 
    $data_fields = array("name", "code", "credit", "nowprice", 
                         "remain", "pre_com_int", "later_com_int", 
                         "deal", "addr", "zhesuanlv");
    // Query for bond informations
    $bond_info = $client->getBondinfo("Select * From zzj_bond Where (deal >= 0 or remain > 20) and " . $yield ." > 0 and " . $conditions ." Order By remain Desc", $yield);
    
    /* 
     * Fetch data from data base response result
     * Visit each field in data fields
     * then output flash recognizable data format
     */
    if ($bond_info->rows == 0)
    {
        echo "status=illegal";
    }
    else
    {
        echo "total_amount=" . $bond_info->rows;
        $max_yield = 0;
        $max_remain = $bond_info->data[0]["remain"];
        for ($i = 0; $i < $bond_info->rows; $i++) {
            for ($j = 0; $j < count($data_fields); $j++) {
                // Fetch field name & value 
                $field_name = $data_fields[$j]; 
                $field_value = $bond_info->data[$i][$field_name];
                // Output in such format "&name=value"
                flash_data_format($field_name . $i, $field_value);
            }
            //echo "----------------------<br>";   
            if ($max_yield < (double) $bond_info->data[$i][$yield]) {
                $max_yield = (double) $bond_info->data[$i][$yield];
            }
        }
        
        // Genreta max remain and yield
        $max_remain = upper_border($max_remain);
        $max_yield = upper_border($max_yield);

        echo "&max_remain=" . $max_remain;
        echo "&max_yield=" . $max_yield;
        
        // Generate knots array for b-spline
        if ($max_remain > 10) {
            $extra_num = 2;
        }
        else if ($max_remain > 6) {
            $extra_num = 1;
        }
        else {
            $extra_num = 0;
        }
        $BSPLINE_NUM += $extra_num;
        for ($i = 1; $i <= $extra_num; $i++) {
            $knots[] = (double) ($i * 4);
        }
        $knots[] = (double)($max_remain + 1);
        /*
        for ($i = 0; $i < $BSPLINE_NUM; $i++)
        {
            echo "<br>" . $knots[$i] . "<br>";
        }
        
        $param = $client->getFittingInfo("Select remain, " . $yield . " From zzj_bond Where " . $yield . " > 0 and " . $conditions . " and (deal > 1 or remain > ". ((int) $max_remain) .") Group By remain", $yield, $knots);
        */
        $param = $client->getFittingInfo("Select remain, " . $yield . " From zzj_bond Where " . $yield . " > 0 and " . $conditions . " and (deal > 1 or remain > 20) Group By remain", $yield, $knots);
        if ($param == false) {
            echo "error occured when computing fitting param" . "<br>";
        }

        /*
         * Output fitted parameters in flash format
         */
        // B-spline part
        //echo "Bspline parameters:<br>";
        $start_pos = 0;
        $end_pos = $BSPLINE_NUM;
        echo "&bspline_num=" . $BSPLINE_NUM ;
        //echo "Param Num: " . $BSPLINE_NUM . "<br>"; 
        for ($i = $start_pos; $i < $end_pos; $i++) {
            flash_data_format("bspline_param" . $i, $param[$i]);    
        }

        // Natural logarithm
        //echo "Natural logrithm parameters:<br>";
        $start_pos  = $BSPLINE_NUM;
        $end_pos = $start_pos + $NATLOG_NUM;
        echo "&natlog_num=" . $NATLOG_NUM;
        //echo "Param Num: " . $NATLOG_NUM . "<br>"; 
        for ($i = $start_pos; $i < $end_pos; $i++) {
            $seq = $i - $start_pos;
            flash_data_format("natlog_param" . $seq, $param[$i]);  
        }

        // Interval Greedy
        //echo "Interval Greedy parameters:<br>";
        $start_pos = $BSPLINE_NUM + $NATLOG_NUM;
        $end_pos = $start_pos + $INTERVAL_NUM;
        //echo "Param Num: " . $INTERVAL_NUM  * 2 . "<br>"; 
        echo "&interval_num=" . ($INTERVAL_NUM * 2);
        for ($i = $start_pos; $i < $end_pos; $i++) {
            $seq = $i - $start_pos;         // Sequence of interval
            // Dump coordinates of optimal point in each interval
            flash_data_format("interval_param_x" . ($seq), $param[$seq * 2 + $start_pos]);
            flash_data_format("interval_param_y" . ($seq), $param[$seq * 2 + 1 + $start_pos]);
        }
    }
?>
