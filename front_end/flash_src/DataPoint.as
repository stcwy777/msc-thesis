/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Draw Data Point
 *
 **************************************************************/
package  
{
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageScaleMode;
	import flash.display.StageAlign;	
	import flash.geom.Point;
	import com.greensock.TweenLite;	
	import fl.motion.MotionEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.utils.Timer;

	
/**
 * 	DataPoint  
 * 	Description
 */	
	public class DataPoint extends MovieClip{

		////////////////////////////////////
		// Const Variables
		////////////////////////////////////
		static public const HALO_NUM:Number = 5;
		
		
		////////////////////////////////////
		// Class Member Variables
		////////////////////////////////////

		private var _stage:Stage;									
		private var _mainMovieClip:MovieClip;							
		private var _drawPen:GeneralGraphic;	// Graphic maker
		private var _haloCenter:Point;			// Center of halo
		private var _show:Boolean = false;		// Halo effect control flag
		private var _relatedBoard:InfoBoard;
		public var _bondInfo:BondInfo;
		private var _boardTimer:Timer;
		
		public function DataPoint(stage:Stage, movieClip:MovieClip, infoBoard:InfoBoard) 
		{
			_stage = stage;
			_mainMovieClip = movieClip;
			_relatedBoard = infoBoard;			
 			
			_bondInfo = new BondInfo();
			_drawPen = new GeneralGraphic;
			_haloCenter = new Point;
			_haloCenter.x = 0;
			_haloCenter.y = 0;
			_boardTimer = new Timer(200, 1);
			
			/*
			for (var i:int = 0; i < HALO_NUM; i++)
			{
				_mainMovieClip["halo_" + i] = new Sprite;
				this.addChild(_mainMovieClip["halo_" + i]);
				_mainMovieClip["halo_" + i] .x = this.x;
				_mainMovieClip["halo_" + i] .y = this.y;
				
				_mainMovieClip["halo_" + i].graphics.lineStyle(1, 0x5AA0D2);
				_drawPen.drawCircle(_mainMovieClip["halo_" + i].graphics, _haloCenter, 5 + Math.sqrt((i + 1)) * 0.4);
				_mainMovieClip["halo_" + i].alpha = 0;							
			}
			*/
		}
		public function setBondInfo(name:String, code:String, credit:String, addr:String, nowprice:Number, remain:Number, 
									pre_com_int:Number, later_com_int:Number, deal:Number, discount_rate:Number)
		{
			_bondInfo.name = name;
			_bondInfo.code = code;
			_bondInfo.credit = credit;
			_bondInfo.addr = addr;	
			_bondInfo.nowprice = nowprice;
			_bondInfo.remain = remain;
			_bondInfo.pre_com_int = pre_com_int;	
			_bondInfo.later_com_int = later_com_int;
			_bondInfo.deal = deal;
			_bondInfo.discount_rate = discount_rate;	

		}
		/*
		public function showHalo(event:MouseEvent):void
		{
			_show = true;
			moveHalo(0);
		}
		public function endHalo(event:MouseEvent):void
		{
			_show = false;
		}		
		public function moveHalo(step:Number)
		{
			if (_show == true || step > 0)
			{
				if (step == 0)
				{
					TweenLite.to(_mainMovieClip["halo_0"], 0.15, {alpha:1, delay:0, onComplete:moveHalo, onCompleteParams:[1]});
				}
				else if (step <  HALO_NUM)
				{
					TweenLite.to(_mainMovieClip["halo_" + (step - 1)], 0.15, {alpha:0, delay:0});
					TweenLite.to(_mainMovieClip["halo_" + step], 0.15, {alpha:1, delay:0, onComplete:moveHalo, onCompleteParams:[step + 1]});					
				}
				else if (step == HALO_NUM)
				{
					TweenLite.to(_mainMovieClip["halo_" + (step - 1)], 0.15, {alpha:0, delay:0, onComplete:moveHalo, onCompleteParams:[0]});
				}
			}
		}					
		*/
		public function changeScale(event:MouseEvent):void
		{
			if (this.scaleX == 1)
			{
				enlargeScale(2, false);
				_boardTimer.start();
				_boardTimer.addEventListener(TimerEvent.TIMER_COMPLETE, showBoard);				
			}
		}
		
		private function enlargeScale(extent:Number, end:Boolean) 
		{
			TweenLite.to(this, 0.2, {scaleX:extent, scaleY:extent, delay:0, onComplete:revertScale, onCompleteParams:[end]});
		}
		
		private function revertScale(end:Boolean) 
		{
			if (end == true)
			{
				TweenLite.to(this, 0.5, {scaleX:1, scaleY:1, delay:0});						
			}
			else
			{
				TweenLite.to(this, 0.2, {scaleX:1, scaleY:1, delay:0, onComplete:enlargeScale, onCompleteParams:[1.5, true]});
			}
		}
		
		private function showBoard(e:TimerEvent):void
		{
			var bondInfo:Array = new Array(_bondInfo.code, _bondInfo.name, _bondInfo.credit, _bondInfo.nowprice, 
										   _bondInfo.remain , _bondInfo.deal, _bondInfo.pre_com_int, _bondInfo.later_com_int);
			_relatedBoard.setTextContent(bondInfo);
			TweenLite.to(_relatedBoard, 0.5, {alpha:1, delay:0});			
		}
		
		public function removeTimer(event:MouseEvent):void
		{		
			if (_boardTimer != null)
			{
				_boardTimer.removeEventListener(TimerEvent.TIMER_COMPLETE, showBoard);
			}
			//if (_relatedBoard.alpha == 1)
			{
				TweenLite.to(_relatedBoard, 0.5, {alpha:0, delay:0});							
			}
		}
	}
	
}
