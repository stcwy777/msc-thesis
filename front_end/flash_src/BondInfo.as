/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Bond Info Structure
 *
 **************************************************************/
package  {
	
    public class BondInfo {

	public var code:String;
	public var name:String;
	public var credit:String;
	public var addr:String;	
	public var nowprice:Number;
	public var remain:Number;
	public var pre_com_int:Number;	
	public var later_com_int:Number;
	public var deal:Number;
	public var discount_rate:Number;	

		public function BondInfo() {
			// constructor code
		}

	}
	
}
