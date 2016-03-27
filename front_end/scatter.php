<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<meta name="KEYWords" content="企债债券,债券散点图，收益率曲线">
<title>企业债券收益率曲线图 beta 1.0</title>
<style type="text/css">
<!--
.label
{
    margin-top: 10px;
    left: 10px;
    font-size: 12px;
    /*font-family:"黑体";*/
}
.criteria
{
    font-size: 12px;
    color: #FDFEFF;
}
.table
{
    border-collapse:collapse; 
    border-spacing:0; 
}
.table td
{
    padding: 2px;
    padding-left: 5px;
    padding: 3px\9;
}
.selectBox
{
    width: 70px;
}
.textInput
{
    width: 66px;
}
#addrSelect
{ 
    width: 80px;
    vertical-align:middle;
}
#typeSelect
{ 
    width: 80px;
    vertical-align:middle;
}

#bg
{ 
    position : relative; 
    margin-left: auto;
    margin-right: auto;    
    /*background-color: #EBF9EF;*/
    width: 1002px;
    height: 750px;
}
#filtTitle
{
    position : absolute; 
    background-image: url("scatter/title_bg.gif");
    /*border:solid 1px;*/
    width: 260px;
    height: 24px;
    top: 51px;
    left: 730px;
    z-index: 2;
}
#txtTitle
{   
    position : absolute; 
    /*\9 For IE6,7,8*/
    top: -1px;
    top: 1px\9;
    font-size: 14px;
    color: #222222;
}
#filter
{
    position: absolute; 
    top: 76px;
    left: 755px;
    background-image: url("scatter/filter_bg.png");
    background-repeat : no-repeat;
	/*border:solid 1px;*/
	border-color: #D3D3D3;
    height: 220px;
    width: 235px;
    display: none;
    color: #EEEEEE;
    z-index: 2;
}
#butSubmit
{
    position: absolute;
    left: 160px;
    left: 156px\9;   
    top: 185px;
    background-image: url("scatter/button_norm.gif");
    width: 55px;
    height: 25px;
    border-style: none;
    cursor: pointer;
    background-color: transparent;
}
#scatTitle
{
    position: absolute;
    top: 0px;
    left: 380px;
    width: 300px;
    height: 60px;
    background-image: url("scatter/scatter_title.gif");
    background-repeat : no-repeat;
}
#scatter
{
    position : absolute; 
    left: 0 px;
    top: 50px;
    width: 1000px;
    height: 580px;
    z-index: 1;
}
#copyrights
{
    position: absolute;
    top: 630px;
    left: 335px;
    width: 500px;
    height: 300px;
    background-image: url("scatter/copyrights.gif");
    background-repeat : no-repeat;
}
#flash
{
    z-index: 1;
}
-->
</style>
<script language="javascript">
function changeCursor(obj)
{
    obj.style.cursor = "pointer";
}

function changeButton()
{
    var obj = document.getElementById("butSubmit");
    obj.style.backgroundImage = "url(scatter/button_on.gif)";
}
function changeMenu(obj)
{
    var menu = document.getElementById("filter");
    
    menu.style.display = ((menu.style.display =='block') ? 'none' :'block');
    
    //alert(title_pic.src);

    return false;
}
</script>
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-20885916-1']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
</head>
<?php
	
	$addr = "all";
	$minCredit = "A";
	$maxCredit = "AAA";
	$type = "pre_com_int";
	$minDiscount = 0.0;
	$maxDiscount = 0.99;
	
	if(isset($_POST["addrSelect"]))
	{
		$addr = $_POST["addrSelect"];
	}
	
	if(isset($_POST["minCredit"]))
	{
		$minCredit = $_POST["minCredit"];
	}
	if(isset($_POST["maxCredit"]))
	{
		$maxCredit = $_POST["maxCredit"];
	}
	
	if(isset($_POST["minDiscount"]))
	{
		$minDiscount = $_POST["minDiscount"];
	}
	if(isset($_POST["maxDiscount"]))
	{
		$maxDiscount = $_POST["maxDiscount"];
	}
	
	if(isset($_POST["typeSelect"]))
	{
		$type = $_POST["typeSelect"];
	}
        $conditions = "credit >= '" . $minCredit . "' and credit <= '" . $maxCredit . "' and zhesuanlv >= " . $minDiscount . " and zhesuanlv <= " . $maxDiscount;
        if ($addr != "all")
        {
            $conditions = $conditions . " and addr = '" . $addr . "'";
        }

/*	echo $addr."\n"; 
	echo $minCredit."\n";
	echo $maxCredit."\n";
	echo $_SESSION["discount"]."\n";
	echo $_SESSION["discount2"]."\n";
	echo $pearn;
 */
        //echo $type . "<br>" . $conditions
?>
<body bgcolor="#FFFFFF">
<div id="bg">
    <div id="filtTitle" class="criteria" onmouseover="changeCursor(this);" onclick="changeMenu(this);">
    </div>
    <div id="filter">
        <form name="filterForm" id="filterForm" method="POST" action="scatter.php">
        <table  class="table" width="100%">
            <tr class=label><td><b>&nbsp基本参数</td></tr>
            <tr class="criteria">
            <td>&nbsp&nbsp上市地 </td>
            <td>
                <select name="addrSelect" id="addrSelect">
                    <option value="all" <?php if($addr == "all") echo "selected";?>>全部</option>
                    <option value="sz" <?php if($addr == "sz") echo "selected";?>>深市</option>
                    <option value="sh" <?php if($addr == "sh") echo "selected";?>>沪市</option>
                </select>
            </td>
            </tr>
            <tr class="criteria">
            <td>&nbsp&nbsp收益类型</td>
            <td>
                <select name="typeSelect" id="typeSelect">
                    <option value="later_com_int" <?php if($type == "later_com_int") echo "selected";?>>税后</option>
                    <option value="pre_com_int" <?php if($type == "pre_com_int") echo "selected";?>>税前</option>
                </select>            
            </td>
            </tr>
        </table>
        <BR>
        <table class="table" width="100%">
            <tr class="label">
                <td><b>&nbsp条件</td> 
                <td align="center"><b>最低</td> 
                <td align="center"><b>最高</td> 
            </tr>
            <tr class="criteria">
                <td>&nbsp&nbsp债券等级</td> 
                <td align="center">
                    <select name="minCredit" id="minCredit" class="selectBox">
                        <option value="A" <?php if($minCredit == "A") echo "selected";?>>A</option>
                        <option value="A+" <?php if($minCredit == "A+") echo "selected";?>>A+</option>
                        <option value="AA" <?php if($minCredit == "AA") echo "selected";?>>AA</option>
                        <option value="AA+" <?php if($minCredit == "AA+") echo "selected";?>>AA+</option>
                        <option value="AAA" <?php if($minCredit == "AAA") echo "selected";?>>AAA</option>
                    </select>   
                </td> 
                <td align="center">                    
                    <select name="maxCredit" id="maxCredit" class="selectBox">
                        <option value="A" <?php if($maxCredit == "A") echo "selected";?>>A</option>
                        <option value="A+" <?php if($maxCredit == "A+") echo "selected";?>>A+</option>
                        <option value="AA" <?php if($maxCredit == "AA") echo "selected";?>>AA</option>
                        <option value="AA+" <?php if($maxCredit == "AA+") echo "selected";?>>AA+</option>
                        <option value="AAA" <?php if($maxCredit == "AAA") echo "selected";?>>AAA</option>
                    </select>
                </td> 
            </tr>
            <tr class="criteria">
                <td>&nbsp&nbsp折算率</td> 
                <td align="center"><input name="minDiscount" type="text" value="<?php echo $minDiscount;?>" class="textInput"></td> 
                <td align="center"><input name="maxDiscount" type="text" value="<?php echo $maxDiscount;?>" class="textInput"></td> 
            </tr>
        </table>
        <input type="submit" id="butSubmit" value="" 
               onmouseover="this.style.backgroundImage = 'url(scatter/button_on.gif)';"
               onmouseout="this.style.backgroundImage = 'url(scatter/button_norm.gif)';">
        </form>
    </div>
    <div id="scatTitle"></div>
    <div id="scatter">
 	    <object  classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" 
                      codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=8,0,0,0" 
                      width="1000" height="580"  align="middle">
            <param name="allowScriptAccess" value="always" />
            <param name="movie" value="scatter/scatter.swf"/>
            <param name="quality" value="high" />
            <param name="wmode" value="opaque" />
            <param name="FlashVars" value="yield=<?php echo $type?>&conditions=<?php echo $conditions?>"/>   
            <embed class="flash" src="scatter/scatter.swf" width="1000" height="580" align="middle" 
                          quality="high" allowscriptaccess="always" 
                          FlashVars="yield=<?php echo $type?>&conditions=<?php echo $conditions?>" wmode="opaque"
                          type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" />    
        </object>   
    </div>
    <div id="copyrights"></div>
</div>
</body>
</html>
