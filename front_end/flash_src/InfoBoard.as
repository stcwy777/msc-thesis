/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Draw Info Board
 *
 **************************************************************/
package  {
	import flash.display.MovieClip;	
	import flash.display.Sprite;		
	import flash.geom.Point;
	import flash.filters.DropShadowFilter;
	import flash.text.TextField;	
	import flash.text.TextFormat;	
	import com.greensock.TweenLite;	
	import flash.display.BitmapData;	
	import flash.display.Bitmap;			
	import flash.events.Event;
		
	public class InfoBoard extends MovieClip{

		////////////////////////////////////
		// Const Variables
		////////////////////////////////////
		// Style Control
		static public const TEXT_MARGIN:Number = 5;	
		static public const TEXT_SPACING:Number = 18;
		static public const TEXT_INTERVAL:Number = 70;
		static public const NUM_LABEL:Number = 9;
		static public const NUM_CONTENT:Number = 8;		
		
		private var _mainMovieClip;
		private var _drawPen:GeneralGraphic;	// Graphic maker// 
		private var _baseBoard:Sprite;
		private	var _textBoard:Sprite;			//  
		private var _txtPicData:BitmapData;
		private var _boardID:String;
		
		public function InfoBoard(movieClip:MovieClip, width:Number, height:Number, textLabels:Array, id:String) 
		{			
			// constructor code
			_mainMovieClip = movieClip;
			_drawPen = new GeneralGraphic;
			_boardID = id;
			
			//_txtPicData = new BitmapData(width, height, true);
			_baseBoard = new Sprite;						
			_textBoard = new Sprite;
			
			this.addChild(_baseBoard);
			this.addChild(_textBoard);						

			// Test			
			var beginPoint:Point = new Point;
			var endPoint:Point = new Point;			
						
			beginPoint.x = 0;
			beginPoint.y = 0;
			_baseBoard.graphics.lineStyle(1, 0xFAFAED);
			_drawPen.fillRect(_baseBoard.graphics, beginPoint, width, height, 0xFAFAED);
			_baseBoard.alpha = 0.8;
			
			beginPoint.x = 0;
			beginPoint.y = 0;
			_baseBoard.graphics.lineStyle(2, 0X989898); //0X71B2CF);
			_drawPen.drawRect(_baseBoard.graphics, beginPoint, width, height);

			var dropShadow:DropShadowFilter = new DropShadowFilter();
			dropShadow.distance = 4;
			dropShadow.color = 0x333333;			
			/*
			dropShadow.distance = 0;
			dropShadow.angle = 45;
			dropShadow.color = 0x333333;
			dropShadow.alpha = 1;
			dropShadow.blurX = 10;
			dropShadow.blurY = 10;
			dropShadow.strength = 1;
			dropShadow.quality = 15;
			dropShadow.inner = false;
			dropShadow.knockout = false;
			dropShadow.hideObject = false;
 			*/
			_baseBoard.filters = new Array(dropShadow);			
			setTextLabel(textLabels);
			
			
			for ( var i:int = 0; i < textLabels.length; i++)
			{
				delete textLabels[i];
			}						
			
			this.addEventListener(Event.REMOVED_FROM_STAGE, dispose);			
		}
		
		public function dispose(event:Event):void
		{
			for (var i:int = 0; i < NUM_LABEL; i++)
			{
				_textBoard.removeChild(_mainMovieClip["label_" + _boardID + i]);
				delete _mainMovieClip["label_" + i];
			}
			for (i = 1; i <= NUM_CONTENT; i++)
			{
				_textBoard.removeChild(_mainMovieClip["content_" + _boardID + i]);
				delete _mainMovieClip["content_" + _boardID + i];
			}
			this.removeChild(_textBoard);
			this.removeChild(_baseBoard);
			_textBoard = null;
			_baseBoard = null;
			this.removeEventListener(Event.REMOVED_FROM_STAGE, dispose);
		}
		
		public function setTextLabel(labels:Array)
		{
			
			var titleFormat:TextFormat = new TextFormat;	
			titleFormat.font = '微软雅黑';
			titleFormat.size = 12;
			titleFormat.color = GeneralGraphic.COL_DARK_BLUE;
			titleFormat.bold = true;
			
			var labelFormat:TextFormat = new TextFormat;	
			labelFormat.font = '微软雅黑';
			//titleFormat.size = 20;
			labelFormat.color = GeneralGraphic.COL_NEARLY_BLACK;
			titleFormat.bold = true;

			var contentFormat:TextFormat = new TextFormat;	
			//contentFormat.font = '微软雅黑';
			//titleFormat.size = 20;
			
			//contentFormat.color = GeneralGraphic.COL_BLACK;
			//titleFormat.bold = true;
			contentFormat.color = GeneralGraphic.COL_RED;

			// Set Labels
			for (var i:int = 0; i < labels.length; i++)
			{
				// Build left index
				_mainMovieClip["label_" + _boardID + i] = new TextField;
				_textBoard.addChild(_mainMovieClip["label_" + _boardID + i]);
	
				// Set text format position and content
				//_mainMovieClip["leftLabel_" + _boardID + i].embedFonts = true;
				//_mainMovieClip["leftLabel_" + _boardID + i].width = LABEL_WIDTH;	
				_mainMovieClip["label_" + _boardID + i].x = TEXT_MARGIN;
				_mainMovieClip["label_" + _boardID + i].y = TEXT_SPACING * i;				
				_mainMovieClip["label_" + _boardID + i].embedFonts = true;
				_mainMovieClip["label_" + _boardID + i].text = labels[i];
				if (i == 0)
				{
					_mainMovieClip["label_" + _boardID + i].setTextFormat(titleFormat);
				}
				else
				{
					_mainMovieClip["label_" + _boardID + i].setTextFormat(labelFormat);								
				}
			}
			/*
			// Transform to bitmap
			_txtPicData.draw(_textBoard);

			var picText:Bitmap = new Bitmap(_txtPicData);
			picText.x = _textBoard.x;
			picText.y = _textBoard.y;
			this.addChild(picText);			
			*/
			for (i = 1; i < labels.length; i++)
			{
				// Set content
				_mainMovieClip["content_" + _boardID + i] = new TextField;
				_textBoard.addChild(_mainMovieClip["content_" + _boardID + i]);					
				_mainMovieClip["content_" + _boardID + i].x = TEXT_MARGIN + TEXT_INTERVAL;
				_mainMovieClip["content_" + _boardID + i].y = TEXT_SPACING * i;
			}
		}
		public function setTextContent(labels:Array)
		{
			var specialColor:TextFormat = new TextFormat;

			for (var i:int = 0; i < labels.length; i++)
			{
				_mainMovieClip["content_" + _boardID + (i + 1)].text = labels[i];
				
			}
			if (_boardID == "curve")
			{
				specialColor.color = GeneralGraphic.COL_RED;
				_mainMovieClip["content_" + _boardID + 2].setTextFormat(specialColor);
				specialColor.color = GeneralGraphic.COL_DARK_BLUE;
				_mainMovieClip["content_" + _boardID + 3].setTextFormat(specialColor);				
			}			
		}		
		
	}
	
}
