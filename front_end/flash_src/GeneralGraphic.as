/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Basic Graphic Functions
 *
 **************************************************************/
package  {
	// Import packages
	import flash.display.BlendMode;
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.display.Shape;
	import flash.geom.Point;
/**
 * 	GenealGraphic  
 * 	Description
 */
	public class GeneralGraphic 
	{
		////////////////////////////////////
		// Const Variables
		////////////////////////////////////
		
		// Color Board
		static public const COL_BLACK:Number = 0x000000;
		static public const COL_NEARLY_BLACK:Number = 0x333333;		
		static public const COL_DARK_GREY:Number = 0x555555;		
		static public const COL_GREY:Number = 0xCCCCCC;
		static public const COL_LIGHT_GREY:Number = 0xEBEBEB;
		static public const COL_SLIGHT_GREY:Number = 0xF5F5F5;	
		static public const COL_GREEN:Number = 0x00CC00;
		static public const COL_LIGHT_BLUE:Number = 0x5AA0D2;		
		static public const COL_GREY_BLUE:Number = 0x486C90;
		static public const COL_DARK_BLUE:Number = 0x0066DD;
		static public const COL_DARK_RED:Number = 0xDC3912;
		static public const COL_RED:Number = 0xE60000;		
		static public const COL_DARK_YELLOW:Number = 0xFF9900;		


		public function GeneralGraphic() 
		{
			// constructor code
		}
		
    	/**
    	* drawDottedLine
    	* Draw a straight line
    	* @param graphics <b>Graphics</b> Graphics object
    	* @param begPoint <b>Point</b> begin position
    	* @param endPoint <b>Point</b> end position
    	*/
		public function drawLine(graphics:Graphics, begPoint:Point, endPoint:Point):void
		{
			// Judge legality of parameters
			if (!graphics || !begPoint || !endPoint)
			{
				return;
			}
			
			// Draw a straight line with graphics
			graphics.moveTo(begPoint.x, begPoint.y);
			graphics.lineTo(endPoint.x, endPoint.y);		
		}
		
    	/**
    	* drawDottedLine
    	* Draw a dotted line
    	* @param graphics <b>Graphics</b> graphics object
    	* @param begPoint <b>Point</b> begin position
    	* @param endPoint <b>Point</b> end position
    	* @param width <b>Number</b> width of dotted line
    	* @param space <b>Number</b> space of dot
    	*/
		public function drawDottedLine(graphics:Graphics, begPoint:Point, endPoint:Point, width:Number, space:Number):void
 		{
			// Judge legality of parameters
			if (!graphics || !begPoint || !endPoint || width <= 0 || space <= 0)
    		{
     			return;
			}

    		var curLen:Number = 3;					// Current length that has been drawn start with small offset
    		var lineStart:Point = new Point;		// Start point of inner line
			var lineEnd:Point = new Point;			// End point of inner line
    		var totalLen:Number = Point.distance(begPoint, endPoint);								// Calc total length from end to begin
    		var radian:Number = Math.atan2(endPoint.y - begPoint.y, endPoint.x - begPoint.x);		// Calc radian from end to begin		
		
    		// Draw dotted lines one by one
			while (curLen <= totalLen)
    		{
     			// Start point of current part
				lineStart.x = begPoint.x + Math.cos(radian) * curLen;
     			lineStart.y = begPoint.y + Math.sin(radian) * curLen;

				// Calc current length
     			curLen += width;						
				// Extra length of last part will be cut off
     			if (curLen > totalLen)
     			{
      				curLen = totalLen;
     			}
			
				// Line to end point of current part and draw the line
     			lineEnd.x = begPoint.x + Math.cos(radian) * curLen;
     			lineEnd.y = begPoint.y + Math.sin(radian) * curLen;
				drawLine(graphics, lineStart, lineEnd);
				
				// Add space between parts
     			curLen += space;		
    		}
   		}
		
    	/**
    	* drawRect
    	* Draw rectangle
    	* @param graphics <b>Graphics</b> graphics object
    	* @param begPoint <b>Point</b> begin position
    	* @param width <b>Number</b> width of rectangle
    	* @param length <b>Number</b> length of rectangle
    	*/
		public function drawRect(graphics:Graphics, begPoint:Point, width:Number, length:Number):void
 		{		
			graphics.drawRect(begPoint.x, begPoint.y, width, length);
		}
    	
		/**
    	* drawCircle
    	* Draw circle
    	* @param graphics <b>Graphics</b> graphics object
    	* @param begPoint <b>Point</b> begin position
    	* @param width <b>Number</b> width of rectangle
    	* @param length <b>Number</b> length of rectangle
    	*/
		public function drawCircle(graphics:Graphics, center:Point, radius:Number):void
 		{		
			graphics.drawCircle(center.x, center.y, radius);
		}
		
    	/**
    	* fillRect
    	* Fill rectangle area with specific color
    	* @param graphics <b>Graphics</b> graphics object
    	* @param begPoint <b>Point</b> begin position
    	* @param width <b>Number</b> width of rectangle
    	* @param length <b>Number</b> length of rectangle
		* @param color <b>Number</b> color of area
    	*/
		public function fillRect(graphics:Graphics, begPoint:Point, width:Number, height:Number, color:Number):void
 		{		
			graphics.beginFill(color);
			graphics.drawRect(begPoint.x, begPoint.y, width, height);
			graphics.endFill();
		}
		
    	/**
    	* fillCircle
    	* Fill rectangle area with specific color
    	* @param graphics <b>Graphics</b> graphics object
    	* @param center <b>Point</b> begin position
    	* @param radius <b>Number</b> width of rectangle
		* @param color <b>Number</b> color of area
    	*/
		public function fillCircle(graphics:Graphics, center:Point, radius:Number, color:Number):void
 		{		
			graphics.beginFill(color);
			graphics.drawCircle(center.x, center.y, radius);
			graphics.endFill();
		}				
	}
}