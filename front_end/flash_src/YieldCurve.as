/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Draw Yield Curve
 *
 **************************************************************/
package  {
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageScaleMode;
	import flash.filters.DropShadowFilter;	
	import flash.display.StageAlign;	
	import flash.geom.Point;
	import com.greensock.TweenLite;	
	import fl.motion.MotionEvent;
	import flash.events.MouseEvent;
	import flash.events.Event;
	
/**
 * 	DataPoint  
 * 	Description
 */		
	public class YieldCurve extends MovieClip
	{
		////////////////////////////////////
		// Const Variables
		////////////////////////////////////
		static public const CUBIC:Number = 3;
		static public const EXCEED:Number = 100;		
		////////////////////////////////////
		// Class Member Variables
		////////////////////////////////////
		
		private var _coVars:Array;
		private var _resVars:Array;
		private var _paramVect:Array;
		private var _knots:Array;
		private var _drawPen:GeneralGraphic;	// Graphic maker
		private var _maxCoVar:Number;
		private var _maxResVar:Number;
		private var _bgWidth:Number;
		private var _bgHeight:Number;
		
		public function YieldCurve(maxCoVar:Number, maxResVar:Number, bgWidth:Number, bgHeight:Number) 
		{
			// constructor code
			_coVars = new Array;
			_resVars = new Array;
			_paramVect = new Array;
			_drawPen = new GeneralGraphic;
			_maxCoVar = maxCoVar;
			_maxResVar = maxResVar;
			_bgWidth = bgWidth;
			_bgHeight = bgHeight;
			
			
			//_knots = new Array(-1, -1, 0, 1, 4, 8, 26);
			_knots = new Array(-1, -1, 0, 1);
			var extraNum :int = 0;
			
			// Knots build ad hoc
			if (_maxCoVar > 10)
			{
				extraNum = 2; 
			}
			else if (_maxCoVar >= 7)
			{
				extraNum = 1;
			}
			else
			{
				extraNum = 0;
			}
			
			for (var i:Number = 1; i <= extraNum; i++)
			{
				_knots.push( i * 4);
			}
			_knots.push(_maxCoVar + 1);
			
			//trace(_knots);
			var step:Number = _maxCoVar / bgWidth;
			for (i = 0; i <= _maxCoVar; i += step)
			{
				_coVars.push(i);
			}
			if (_coVars.length < (bgWidth + 1))
			{
				_coVars.push(_maxCoVar);
			}
			this.addEventListener(Event.REMOVED_FROM_STAGE, dispose);			
		}
		
		public function dispose(event:Event):void
		{
			for ( var i:int = 0; i < _coVars.length; i++)
			{
				delete _coVars[i];
			}
			for ( i = 0; i < _resVars.length; i++)
			{
				delete _resVars[i];
			}
			for ( i = 0; i < _paramVect.length; i++)
			{
				delete _paramVect[i];
			}
			this.removeEventListener(Event.REMOVED_FROM_STAGE, dispose);						
		}
		public function addParam(newParam) 
		{
			_paramVect.push(newParam);
		}
		
		public function interpolation(func:Function)
		{
			for (var i:int = 0; i < _coVars.length; i++)
			{
				_resVars[i] = 0;
				for (var j:int = 0; j < _paramVect.length; j++)
				{
					_resVars[i] += _paramVect[j] * func(j, _coVars[i]);
				}
				if (_resVars[i] > _maxResVar)
				{
					_resVars[i] = EXCEED;
				}
				
			}
		}
		
		// Function Closure
		public function interpFunc(method:String):Function
		{
			function natLogFit(seq:Number, coVar:Number)
			{
				var retVar:Number;
				
				if (seq == 0)
				{
					retVar = 1;
				}
				else
				{
					retVar = Math.log(coVar);
				}
				return retVar;
			}
			
			function bsplineFit(seq:Number, coVar:Number)
			{
				var retVar:Number;
				
				if (seq == 0)
				{
					retVar = 1;
				}
				else
				{
					retVar = bspline(_knots, seq, coVar);
				}
				return retVar;
			}
			
			if (method == "natlog")
			{
				return natLogFit;
			}
			else 
			{
				return bsplineFit;
			}
		}
		
		public function bspline(knots:Array, seq:Number, coVar:Number)
		{
    		var resVar:Number = 0;         		// Response value of covariable
    		var tempDividend:Number;        // Temp value of dividend
    		var tempDivisor:Number;			// Temp value of divispr
    		var upperBond:Number;           // Uppder bond of knots sequence

   	 		// Expection upperbond sequence of knot
    		upperBond = seq + CUBIC + 1;    
    		// Fix to knots' amount if expection uppderbond exceed the amount
    		if (upperBond > (knots.length - 1) )
    		{
        		upperBond = knots.length - 1; 
    		}


    		/*
     		 * Here we have (upperBond - seq) sub interval
             * If covariable less than knots[seq] or great than knots[upperBond]
     	     * B-spline will return 0.
             * Otherwise, return response value based on each sub domain contains covariable  
             */
			 
			if (coVar > knots[seq] && coVar <= knots[upperBond])
    		{
        		for (var i:Number = seq; i <= upperBond; i++)
        		{
            		// Stop when covariable less than left bond of current sub interval
            		if (coVar < knots[i])
            		{
                		break;
            		}
            		else
            		{
                		// Initialize temp value according to B-spline equation 
                		tempDividend = Math.pow((coVar - knots[i]), CUBIC);
                		tempDivisor = 1;
                		// Visit each kont in [seq, upperBond]     
                		for (var j:Number = seq; j <= upperBond; j++)
                		{
                    		// Skip same knot
                    		if ( j == i)
                    		{
                       			continue;
                    		}
                    		// Multiply distance from every different knot to current one
                    		else
                    		{
                        		tempDivisor *= (knots[j] - knots[i]);
                    		}
           	     		}
            		}
            		// Acculate value in each sub interval
            		resVar += tempDividend / tempDivisor;
        		}
    		} 
    		return resVar;
		}
		
		public function plot(color:Number)
		{
			var beginPoint:Point = new Point;
			var endPoint:Point = new Point;
			
			this.graphics.lineStyle(2, color);			
			
			for (var i:int = 0; i < (_coVars.length - 1); i++)
			{ 
				if (_resVars[i] < 0)
				{
					continue;
					
				}
				else if (_resVars[i] == EXCEED)
				{
					continue;
				}
				beginPoint.x = _coVars[i] / _maxCoVar * _bgWidth;
				beginPoint.y = ((_maxResVar - _resVars[i]) / _maxResVar) * _bgHeight;
				//trace(_maxResVar);
				//trace(_resVars[i]);

				endPoint.x = _coVars[i + 1] / _maxCoVar * _bgWidth;
				endPoint.y = ((_maxResVar - _resVars[i+1]) / _maxResVar) * _bgHeight;
				_drawPen.drawLine(this.graphics, beginPoint, endPoint);
			}
			
			// Add shadow drop effect
			var dropShadow:DropShadowFilter = new DropShadowFilter();
			dropShadow.distance = 1;
			dropShadow.color = GeneralGraphic.COL_NEARLY_BLACK;			
			this.filters = new Array(dropShadow);			
		}
		public function getCoVar(pos:int)
		{
			var retVar:Number = _coVars[pos];
			return retVar.toFixed(5);
		}
		
		public function getResVar(pos:int)
		{
			var retVar:Number = _resVars[pos];
			var errStr:String;
			
			if (this.visible == false)
			{
				errStr = "未激活";
				return errStr;
			}
			else if (retVar < 0 || retVar == EXCEED)
			{
				errStr = "数据越界";
				return errStr;
			}
			else
			{
				return retVar.toFixed(5);
			}
		}
	}
	
}
