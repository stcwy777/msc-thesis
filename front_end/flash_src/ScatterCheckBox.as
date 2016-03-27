/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Draw Scatter Check Box
 *
 **************************************************************/
package  {
	import fl.controls.CheckBox; 
	import fl.accessibility.CheckBoxAccImpl; 
	import flash.display.MovieClip;
	import flash.display.Sprite;	
	import flash.geom.Point;
	import flash.sampler.Sample;
	import flash.events.MouseEvent;
	
	public class ScatterCheckBox extends CheckBox{

		private var _drawPen:GeneralGraphic;	// Graphic maker		
		private var _linkedCurve:YieldCurve;
		private var _linkedBoard:InfoBoard;		
		private var _traceLine:Sprite;		
		
		public function ScatterCheckBox(color:Number,  linkedCurve:YieldCurve, linkedBoard:InfoBoard, traceLine:Sprite) {
			// constructor code
			var begPoint:Point  = new Point;
			var bgGround:Sprite = new Sprite;
			
			_drawPen = new GeneralGraphic;	// Graphic maker
			_linkedCurve = linkedCurve;
			_linkedBoard = linkedBoard;
			_traceLine = traceLine;		
			
			begPoint.x = -5;
			begPoint.y = 1;			
			bgGround.graphics.lineStyle(1, GeneralGraphic.COL_GREY_BLUE);
			_drawPen.drawRect(bgGround.graphics, begPoint, 110, 21);			
			
			begPoint.x = -5;
			begPoint.y = 1;			
			bgGround.graphics.lineStyle(1, color);			
			_drawPen.fillRect(bgGround.graphics, begPoint, 5, 21, color);						
			this.addChild(bgGround);
			this.addEventListener(MouseEvent.CLICK,updateGrid); 

			
			if (_linkedCurve == null)
			{
				this.selected = false;			
			}
			else
			{
				this.selected = true;
			}
		}
		
		public function updateGrid(event:MouseEvent):void
		{
			if (this.selected  == true)
			{
				if (_linkedCurve != null)
				{
					_linkedCurve.visible = true;
				}
				else
				{
					_linkedBoard.visible = true;
					_traceLine.visible = true;					
				}
			}
			else
			{
				if (_linkedCurve != null)
				{
					_linkedCurve.visible = false;
				}
				else
				{
					_linkedBoard.visible = false;
					_traceLine.visible = false;
				}				
			}
		}
	}
	
}
