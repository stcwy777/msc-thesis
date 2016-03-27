<?php
/*****************************************************
* DBAccess.php
*
* Class defined in this file used as interface for
* PHPRPC server to visit MySQL databse
*
* @author   Wang Yun
* @date     4/23/2010
* @version  1.0
******************************************************/


/*
 * Class: DataSet
 * Data carrier as return unit in DBAccess
 */
class DataSet {
    var $rows;      // num of rows in results
    var $cols;      // num of fields in results
    var $data;      // Data array in 2-D
}

/*
 * Class: DBAccess
 * Interface for database acccess
 * Most of these interface accompished by inner api
 * for details check php mysql manual
 */
class DBAccess {
    var $m_connect;     // Link resource
    var $m_result;      // Result resource
    var $m_data;        // Data carrirer

    /**
    *connDatabase
    * Link specific MySQL database
    * @host:    server name
    * @usr:     login user
    * @pwd:     login password
    * @databse: database to use
    * @return:  return reason when error occurred
    */
    function connDatabase($host, $usr, $pwd, $database) {
        $this->m_connect = mysql_connect($host, $usr, $pwd);
        $ret = mysql_select_db($database, $this->m_connect);
        if ($ret == FALSE)
            return mysql_error();
		else
		    return "Connect OK";
    }

    /**
    *sendQuery
    * Send query to database
    * @sql:     query msg
    * @return:  return reason when error occurred
    */
    function sendQuery($sql) {
	    $this->m_result = mysql_query($sql, $this->m_connect);
        if ($this->m_result == FALSE)
            return mysql_error();
		else
		    return "Send OK";
    }

    /**
    *fetchRowAll
    * Get full data set from result
    * @return:  data carrirer structure
    *           reason of error
    */
    function fetchRowAll() {
        // Build carrier
        $this->m_data = new DataSet();
        // Get rows and columns of result set
        $this->m_data->rows = mysql_num_rows($this->m_result);
        $this->m_data->cols = mysql_num_fields($this->m_result);

        // Fetch each row from result
        for ($i = 0; $i < $this->m_data->rows; $i++) {
            $row = mysql_fetch_array($this->m_result);
            if ($row == NULL) {
                return mysql_error();
            }
            else
                $this->m_data->data[] = $row;   // Store this row
        }
        return $this->m_data;
    }

    /**
    *clearResource
    * Get full data set from result
    * @return:  data carrirer structure
    *           reason of error
    */
    function clearResource() {
        // Free result
        mysql_free_result($this->m_result);
        // Free link
        mysql_close($this->m_connect);
    }
}
?>