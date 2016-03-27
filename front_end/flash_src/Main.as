/***************************************************************
 * Copyright (c) 2010.6  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * Yield Curve Scatter
 *
 * Main Scripts
 *
 **************************************************************/
package  
{
	////////////////////////////////////
	// Import Package
	////////////////////////////////////
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.geom.Matrix;	
	import flash.display.*;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flashx.textLayout.accessibility.TextAccImpl;
	import flash.text.engine.TabAlignment;	
	import flash.events.Event;
	import flash.text.engine.SpaceJustifier;
	import flash.filters.DropShadowFilter;		

	import flash.text.TextFormat;
	import fl.motion.MotionEvent;
	import fl.controls.CheckBox; 
	import fl.accessibility.CheckBoxAccImpl; 
	import flash.events.MouseEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLRequestMethod;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLVariables;
	import flash.events.Event;
	import flash.text.Font;
	import flash.sensors.Accelerometer;
	
	import org.bytearray.gif.player.GIFPlayer;

	// Main process
	public class Main 
	{
		////////////////////////////////////
		// Const Variables
		////////////////////////////////////

		// Style Control
		static public const BG_WIDTH:Number =  950;
		static public const BG_HEIGHT:Number = 505;		
		static public const LINE_PIXEL:Number = 1;
		static public const CURVE_PIXEL:Number = 2;
		static public const DASHED_LEN:Number = 5;
		static public const DASHED_SPACE:Number = 2;
		static public const TITLE_HEIGHT:Number = 30;
		static public const LABEL_WIDTH:Number = 40;
		static public const LABEL_LEFT_MARGIN:Number = 20;		
		static public const LABEL_TOP_MARGIN:Number = 5;				
		static public const TOP_BAR_HEIGHT:Number = 25;
		static public const BOT_BAR_HEIGHT:Number = 50;
		static public const TOP_LABEL_NUM:Number = 5;
		static public const TOP_BAR_MARGIN:Number = 0;
		static public const BOT_BAR_MARGIN:Number = 2;		
		static public const TOP_BAR_PADDING:Number = 10;
		static public const BAR_SPACE:Number = 90;
		static public const TIME_ADJUST:Number = 15;
		static public const LABEL_ADJUST:Number = 7;
		static public const FONT_SIZE:Number = 14;
		static public const NOTATION_SIZE:Number = 20;

		static public const BOARD_WIDTH:Number = 135;
		static public const BOARD_HEIGHT:Number = 180;
		static public const BOARD_MARGIN:Number = 20;		
		static public const CURVE_BOARD_X:Number = 10;		
		static public const CURVE_BOARD_Y:Number = 10;		
		static public const EXCEED:Number = 100;				
		static public const POINT_RADIUS:Number = 3.5;				
		static public const TRACE_RADIUS:Number = 4.5;						
		////////////////////////////////////
		// Class Member Variables
		////////////////////////////////////
		private var _mainMovieClip:MovieClip;		// MovieClip of flash document
		private var _stage:Stage;								// Stage of flash document
		private var _grid:Sprite;								// Background grid container
		private var _leftLabel:Sprite;						// Left label container
		private var _botLabel:Sprite;						// Right label container
		private	var _topBar:Sprite;							// Top bar container
		private var _botBar:Sprite;							// Bottom bar container
		private var _drawPen:GeneralGraphic;			// Graphic maker
		
		private var _verLength:Number;					// Length of vertical lines
		private var _horLength:Number;					// Length of horizontal lines
		private var _verSpace:Number;					// Space between vertical lines
		private var _horSpace:Number;					// Space between horizontal lines
		private var _verLineNum:Number;				// Space between vertical lines
		private var _horLineNum:Number;				// Space between horizontal lines
				
		private var _dataLoader:URLLoader;				// URL loader 
		private var _sourceData:URLVariables;		// Source data dumped by _dataLoader
		
		private var _natlogCurve:YieldCurve;			// Yield curve with natural logarithm fitting
		private var _bsplineCurve:YieldCurve;			// Yield curve with B-spline fitting 
		private var _traceLine:Sprite;						// Trace line
		private var _traceNatLog:Sprite;
		private var _traceBspline:Sprite;
		
		private var _bondInfoBoard:InfoBoard;		// Bond information board
		private var _curveInfoBoard:InfoBoard;		// Curve information board
		private var _maxYield:Number;					// Bond information board
		private var _maxRemain:Number;				// Curve information board
		
		private var paramObj:Object ;						// Input parameters

		private var _loadGifPlayer:GIFPlayer;
		////////////////////////////////////
		// Active Script Entrance
		////////////////////////////////////
		public function Main(stage:Stage, movieClip:MovieClip) 
		{
			////////////////////////////////////
			// Initialize stage information
			////////////////////////////////////

			_mainMovieClip = movieClip;
			_stage = stage;

			////////////////////////////////////
			// Initialize graphic objects
			////////////////////////////////////			
			_drawPen = new GeneralGraphic;
			_grid = new Sprite;
			_leftLabel = new Sprite;
			_botLabel = new Sprite;
			_topBar = new Sprite;
			_botBar = new Sprite;
			
			// Set bond information board
			var bondLabels = new Array("债券基本信息", "债券代码:", "债券简称:", "信用等级:", "当前价格:", "剩余年限:", "成交额:", "税前收益率:", "税后收益率:");			
			_bondInfoBoard = new InfoBoard(_mainMovieClip, BOARD_WIDTH, BOARD_HEIGHT, bondLabels, "bond");
			_bondInfoBoard.alpha = 0;

			var curveLabels = new Array("拟合数据信息", "剩余年限:", "对数收益率:","样条收益率:");			
			_curveInfoBoard = new InfoBoard(_mainMovieClip, BOARD_WIDTH, BOARD_HEIGHT / 2, curveLabels, "curve");
			_curveInfoBoard.alpha = 1;
			
			
			_stage.align = StageAlign.LEFT;										// Align to top and left coner
			_stage.scaleMode = StageScaleMode.NO_SCALE;			// Don't change scale			
			
			// Add backgrond objects to stage
			_stage.addChild(_grid);
			_stage.addChild(_leftLabel);
			_stage.addChild(_botLabel);
			_stage.addChild(_topBar);
			_stage.addChild(_botBar);
			
			_mainMovieClip.addEventListener(Event.REMOVED_FROM_STAGE, dispose);
			////////////////////////////////////
			// Load externel data
			///////////////////////////////////
			
			_dataLoader = new URLLoader;
			var sendData:URLVariables = new URLVariables();
			_sourceData = new URLVariables();

			paramObj= LoaderInfo(_mainMovieClip.root.loaderInfo).parameters;
			sendData.yield = String(paramObj["yield"]);
			sendData.conditions = String(paramObj["conditions"]);

			///////////////////////////////
			// Set Loading Effect
			///////////////////////////////
			//_loadGifPlayer = new GIFPlayer();
 			//_loadGifPlayer.load ( new URLRequest ("scatter/loading.gif") );
 			//_loadGifPlayer.loadBytes ( gifStream );
 			//_stage.addChild(_loadGifPlayer);
			//_loadGifPlayer.totalFrames;
			//_loadGifPlayer.play();
 			
			//_loadGifPlayer.x = 300;
			//_loadGifPlayer.y = 100;			
			
			_dataLoader.dataFormat = URLLoaderDataFormat.VARIABLES;	
			var request:URLRequest = new URLRequest("scatter/scatter_data.php");			// Set up request for target data file
			//var request:URLRequest = new URLRequest("flash_data");
			request.method = URLRequestMethod.POST;
			request.data = sendData;
			_dataLoader.load(request);

			_dataLoader.addEventListener(Event.COMPLETE, finishLoad);		// Wait for loading finish
		}
		
		// Dispose sources when object removed from stage
		public function dispose(event:Event):void
		{
			// Release dynamic attributes
			delete _sourceData["max_remain"];
			delete _sourceData["max_yield"];
			
			var pointAmount:Number = _sourceData["total_amount"];
			
			//  Release Data points
			for (var i:int = 0; i < pointAmount; i++)
			{
				_grid.removeChild(_mainMovieClip["point_" + i]);
				_grid.removeChild(_mainMovieClip["point_shadow" + i]);				
				_mainMovieClip["point_" + i].removeEventListener(MouseEvent.ROLL_OVER, _mainMovieClip["point_" + i].changeScale);				
				_mainMovieClip["point_" + i].removeEventListener(MouseEvent.ROLL_OUT, _mainMovieClip["point_" + i].removeTimer);												
				delete _mainMovieClip["point_" + i];
				delete _mainMovieClip["point_shadow" + i];
				delete _sourceData["name" + i];
				delete _sourceData["code" + i];
				delete _sourceData["credit" + i];
				delete _sourceData["addr" + i];
				delete _sourceData["nowprice" + i];
				delete _sourceData["remain" + i];
				delete _sourceData["pre_com_int" + i];
				delete _sourceData["later_com_int" + i];
				delete _sourceData["deal" + i];
				delete _sourceData["discount_rate" + i];				
			}			
			delete _sourceData["total_amount"];
			/*
			_stage.removeChild(_mainMovieClip["title"]);
			_stage.removeChild(_mainMovieClip["version"]);			
			delete _mainMovieClip["title"];
			delete _mainMovieClip["version"];		
			*/
			
			delete _mainMovieClip["leftLabel_notation"];
			
			for (i = 0; i < _horLineNum - 1; i++)
			{
				delete _mainMovieClip["leftLabel_" + i];
			}

			delete _mainMovieClip["botLabel_notation"];
			
			for (i = 0; i < _verLineNum; i++)
			{
				delete _mainMovieClip["botLabel_" + i];
			}
			
			var paramNum:Number = _sourceData["natlog_num"];
			
			for (i = 0; i < paramNum; i++)
			{
				delete _sourceData["natlog_param" + i];
			}
			delete _sourceData["natlog_num"];
			
			// Set parameters for B-spline fitting curve
			paramNum = _sourceData["bspline_num"];
			for (i = 0; i < paramNum; i++)
			{
				delete _sourceData["bspline_param" + i];
			}
			delete _sourceData["bspline_num"];
			
			// Remove child
			_stage.removeChild(_grid);
			_stage.removeChild(_leftLabel);
			_stage.removeChild(_botLabel);
			_stage.removeChild(_topBar);
			_stage.removeChild(_botBar);
			
			// Dispatch event
			_dataLoader.removeEventListener(Event.COMPLETE, finishLoad);
			_mainMovieClip.removeEventListener(Event.REMOVED_FROM_STAGE, dispose);			
		}
		
		public function finishLoad(event:Event):void
		{
			//_stage.removeChild(_loadGifPlayer);
			_sourceData = _dataLoader.data;		// Dump data
			if (_sourceData["status"] == "illegal")
			{
				setError();
			}
			else
			{
				//trace(_dataLoader.data["max_yield"]);
				_verLineNum = _sourceData["max_remain"];
				_horLineNum = _sourceData["max_yield"];
				_verLineNum ++;
				_horLineNum ++;
			
				setBackground();			// Build background
				setCurve();					// Build curves			
				setPoints();				// Build data points
				setLabels();				// Build labels		
			}
		}
		public function setError()
		{
			////////////////////////////////////
			// Draw Bars
			////////////////////////////////////
			var begPoint:Point = new Point;
			// Draw top bar
			_topBar.x = LABEL_WIDTH;
			_topBar.y = 0;
			// Top bar has margin
			begPoint.x = 0
			begPoint.y = TOP_BAR_MARGIN;
			// Draw and fill bar area
			_topBar.graphics.lineStyle(LINE_PIXEL, GeneralGraphic.COL_GREY);							
			_drawPen.fillRect(_topBar.graphics, begPoint, BG_WIDTH,  TOP_BAR_HEIGHT - TOP_BAR_MARGIN * 2, GeneralGraphic.COL_SLIGHT_GREY);
			
			begPoint.x = LABEL_WIDTH;
			begPoint.y = TOP_BAR_HEIGHT;			
			_mainMovieClip.graphics.lineStyle(LINE_PIXEL, GeneralGraphic.COL_GREY);							
			_drawPen.drawRect(_mainMovieClip.graphics, begPoint, BG_WIDTH, BG_HEIGHT);
			
			// Error Suggestion
			begPoint.x = 0
			begPoint.y = TOP_BAR_HEIGHT;			
 			var errSug:Loader = new Loader();
 			var url:String = "scatter/error.gif";
 			var urlReq:URLRequest = new URLRequest(url);
 			errSug.load(urlReq);
 			_stage.addChild(errSug);
			errSug.x = 400;
			errSug.y = 200;			
		}
		public function setBackground()
		{
			////////////////////////////////////
			// Draw Grid
			////////////////////////////////////
									
			var	begPoint:Point = new Point;					    // Begin point of a line to be drawn 
			var endPoint:Point = new Point;							// End point of a line to be dwawn

			var bgWidth:Number = 1000;								// Width of background stage
			var bgHeight:Number = 580;								// Height of background stage						
			_verLength = bgHeight - TOP_BAR_HEIGHT - 
			             BOT_BAR_HEIGHT ;									// Length of vertical lines
			_horLength = bgWidth -  LABEL_WIDTH - 10;						// Length of horizontal lines
			_verSpace = _horLength / (_verLineNum - 1);						// Space between vertical lines
			_horSpace = _verLength / (_horLineNum - 1);					// Space between horizontal lines

			
			// Set line color and pixel size
			_grid.graphics.lineStyle(LINE_PIXEL, GeneralGraphic.COL_GREY);
			_grid.x = LABEL_WIDTH;
			_grid.y = TOP_BAR_HEIGHT; // + TITLE_HEIGHT;

			// Set start point at (LABEL_WIDTH, TOP_BAR_HEIGHT)
			begPoint.x = 0;
			begPoint.y = 0;

			// Draw vertical lines
			for (var i:int = 0; i < _verLineNum; i++)
			{
				// Calc end of line
				endPoint.x = begPoint.x;
				endPoint.y = begPoint.y + _verLength;
				if (i == 0 || i == _verLineNum - 1)
				{
					_drawPen.drawLine(_grid.graphics, begPoint, endPoint);
				}
				else
				{
					// Draw this line
					_drawPen.drawDottedLine(_grid.graphics, begPoint, endPoint, DASHED_LEN, DASHED_SPACE);	
				}
				// Skip space to next line start
				begPoint.x += _verSpace;		
			}

			// Reset to start position
			begPoint.x = 0;
			begPoint.y = 0;

			// Draw horizontal lines almost the same as vertical ones
			for (i = 0; i < _horLineNum; i++)
			{
				endPoint.x = begPoint.x + _horLength;
				endPoint.y = begPoint.y;
				if (i == 0 || i == _horLineNum - 1)
				{
					_drawPen.drawLine(_grid.graphics, begPoint, endPoint);
				}
				else
				{
					// Draw this line
					_drawPen.drawDottedLine(_grid.graphics, begPoint, endPoint, DASHED_LEN, DASHED_SPACE);	
				}
				begPoint.y += _horSpace;
			}
			
			////////////////////////////////////
			// Draw Bars
			////////////////////////////////////
			
			// Draw top bar
			_topBar.x = LABEL_WIDTH;
			_topBar.y = 0;//TITLE_HEIGHT;
			// Top bar has margin
			begPoint.x = 0
			begPoint.y = TOP_BAR_MARGIN;
			// Draw and fill bar area
			_topBar.graphics.lineStyle(LINE_PIXEL, GeneralGraphic.COL_GREY);							
			_drawPen.fillRect(_topBar.graphics, begPoint, _horLength, TOP_BAR_HEIGHT - TOP_BAR_MARGIN * 2, GeneralGraphic.COL_SLIGHT_GREY);

			
			// Draw and fill bar area			
			//_botBar.graphics.lineStyle(LINE_PIXEL, GeneralGraphic.COL_GREY);				
			//_drawPen.fillRect(_botBar.graphics, begPoint, _horLength, BOT_BAR_HEIGHT - BOT_BAR_MARGIN * 2, GeneralGraphic.COL_LIGHT_GREY);
		}
		
		private function setPoints()
		{
			var pointAmount:Number = _sourceData["total_amount"];
			var centerPoint:Point = new Point;
			var dropShadow:DropShadowFilter = new DropShadowFilter();
			dropShadow.distance = 1;
			dropShadow.color = 0x333333;			
			dropShadow.hideObject = false;
			
			centerPoint.x = 0;
			centerPoint.y = 0;
				
			for (var i:int = 0; i < pointAmount; i++)
			{
				_mainMovieClip["point_" + i] = new DataPoint(_stage, _mainMovieClip, _bondInfoBoard);
				_mainMovieClip["point_" + i].setBondInfo(_sourceData["name" + i], _sourceData["code" + i], _sourceData["credit" + i].replace("A ", "A+"), _sourceData["addr" + i],
													 _sourceData["nowprice" + i], _sourceData["remain" + i], _sourceData["pre_com_int" + i],
													 _sourceData["later_com_int" + i], _sourceData["deal" + i], _sourceData["discount_rate" + i]);
				_mainMovieClip["point_" + i].x = _sourceData["remain" + i] / (_verLineNum - 1) * BG_WIDTH;
				_mainMovieClip["point_" + i].y = ((_horLineNum - 1 - _sourceData[String(paramObj["yield"]) + i]) / (_horLineNum - 1 )) * BG_HEIGHT;				
				//_mainMovieClip["point_" + i].y = ((_horLineNum - 1 - _sourceData["later_com_int" + i]) / (_horLineNum - 1 )) * BG_HEIGHT;				
				
				// Shadow effect
				_mainMovieClip["point_shadow" + i] =  new Sprite;
				_mainMovieClip["point_shadow" + i].x = _mainMovieClip["point_" + i].x;
				_mainMovieClip["point_shadow" + i].y = _mainMovieClip["point_" + i].y;
				_grid.addChild(_mainMovieClip["point_shadow" + i]);
				_drawPen.fillCircle(_mainMovieClip["point_shadow" + i].graphics, centerPoint, POINT_RADIUS, GeneralGraphic.COL_GREEN);								
				_mainMovieClip["point_shadow" + i].filters = new Array(dropShadow);											
	
				_grid.addChild(_mainMovieClip["point_" + i]);	
				_drawPen.fillCircle(_mainMovieClip["point_" + i].graphics, centerPoint, 3.5, GeneralGraphic.COL_GREEN);				
				_mainMovieClip["point_" + i].addEventListener(MouseEvent.ROLL_OVER, _mainMovieClip["point_" + i].changeScale);				
				_mainMovieClip["point_" + i].addEventListener(MouseEvent.ROLL_OUT, _mainMovieClip["point_" + i].removeTimer);									
			}
			
			// Set bond info board related to each points
			_grid.addChild(_bondInfoBoard);
			_bondInfoBoard.x = _horLength - BOARD_WIDTH - BOARD_MARGIN;
			_bondInfoBoard.y = _verLength - BOARD_HEIGHT - BOARD_MARGIN;
		}
		
		private function setLabels()
		{
			////////////////////////////////////
			// Set Label Text 
			////////////////////////////////////
			
			/*
			var titleFormat:TextFormat = new TextFormat;	
			titleFormat.font = '微软雅黑';
			titleFormat.size = 20;
			titleFormat.bold = true;
			
			// Set title			
			_mainMovieClip["title"]  = new TextField;
			_stage.addChild(_mainMovieClip["title"]);
			_mainMovieClip["title"].x = (_horLength  + LABEL_WIDTH) / 2 - 110;
			_mainMovieClip["title"].y = 2;
			_mainMovieClip["title"].width = 220;				// Default width not enough
			_mainMovieClip["title"].embedFonts = true;
			_mainMovieClip["title"].text = "企业债券收益率散点图";
			_mainMovieClip["title"].setTextFormat(titleFormat);
			_mainMovieClip["title"].selectable = false;
			//_mainMovieClip["title"].visible = false;
			

			var versionFormat:TextFormat = new TextFormat;	
			versionFormat.font = "6px2bus";
			versionFormat.size = 15;
			versionFormat.color = GeneralGraphic.COL_DARK_GREY;
			
			// Set Version
			_mainMovieClip["version"]  = new TextField;
			_stage.addChild(_mainMovieClip["version"]);
			_mainMovieClip["version"].x = _mainMovieClip["title"].x + _mainMovieClip["title"].width;
			_mainMovieClip["version"].y = 12;
			_mainMovieClip["version"].text = "BETA 1.0";
			_mainMovieClip["version"].embedFonts = true;
			_mainMovieClip["version"].setTextFormat(versionFormat);
			_mainMovieClip["version"].selectable = false;
			//_mainMovieClip["version"].visible = false;
			*/
			/*
			// Transform to bitmap
			var verPicData:BitmapData = new BitmapData(100, 20, true); 
			verPicData.draw(_mainMovieClip["version"]);

			var picVersion:Bitmap = new Bitmap(verPicData);
			picVersion.x = _mainMovieClip["version"].x;
			picVersion.y = _mainMovieClip["version"].y;
			_stage.addChild(picVersion);
			*/
			// Set left notation format
			var CheckFormat:TextFormat = new TextFormat;	
			CheckFormat.font = '微软雅黑';
			CheckFormat.size = 12;
			//CheckFormat.bold = true;
			CheckFormat.color = 0x222222;
			
			// Set checkBox of natura log fitting
			var	natLogCheck:ScatterCheckBox = new ScatterCheckBox(GeneralGraphic.COL_RED, _natlogCurve, null, null);
			natLogCheck.move(50, 1);
			natLogCheck.setStyle("textFormat", CheckFormat);			
			natLogCheck.setStyle("embedFonts", true);			
			natLogCheck.label = "对数曲线拟合";
			natLogCheck.width = 110;
			_topBar.addChild(natLogCheck);
			
			var	bSplineCheck:ScatterCheckBox = new ScatterCheckBox(GeneralGraphic.COL_DARK_BLUE, _bsplineCurve, null, null);
			bSplineCheck.move(200, 1);
			bSplineCheck.setStyle("textFormat", CheckFormat);			
			bSplineCheck.setStyle("embedFonts", true);			
			bSplineCheck.label = "分段样条拟合";
			bSplineCheck.width = 110;
			_topBar.addChild(bSplineCheck);
			
			var	curveBoardCheck:ScatterCheckBox = new ScatterCheckBox(GeneralGraphic.COL_DARK_YELLOW, null, _curveInfoBoard, _traceLine);
			curveBoardCheck.move(350, 1);
			curveBoardCheck.setStyle("textFormat", CheckFormat);			
			curveBoardCheck.setStyle("embedFonts", true);			
			curveBoardCheck.label = "拟合数据信息";
			curveBoardCheck.width = 110;
			_topBar.addChild(curveBoardCheck);
			
			// Set format of digits
			var digitFormat:TextFormat = new TextFormat;	
			//digitFormat.align = "right";
			digitFormat.font = "微软雅黑";
			digitFormat.bold = true;
			digitFormat.size = 12;
			digitFormat.color = GeneralGraphic.COL_DARK_GREY;
			
			// Set left notation format
			var NotationFormat:TextFormat = new TextFormat;	
			NotationFormat.font = '微软雅黑';
			NotationFormat.size = 12;
			NotationFormat.bold = true;
			NotationFormat.color = GeneralGraphic.COL_DARK_GREY;

			// Set left label
			_leftLabel.x = 0;
			_leftLabel.y = TOP_BAR_HEIGHT;// + TITLE_HEIGHT;
			
			// First set lable notation
			_mainMovieClip["leftLabel_notation"]  = new TextField;
			_leftLabel.addChild(_mainMovieClip["leftLabel_notation"]);
			_mainMovieClip["leftLabel_notation"].x = LABEL_WIDTH - 2 * LABEL_LEFT_MARGIN;
			_mainMovieClip["leftLabel_notation"].y = (_verLength  - TOP_BAR_HEIGHT) / 2;
			_mainMovieClip["leftLabel_notation"].embedFonts = true;			
			_mainMovieClip["leftLabel_notation"].text = "收益率(%)";
			_mainMovieClip["leftLabel_notation"].setTextFormat(NotationFormat);
			_mainMovieClip["leftLabel_notation"].visible = false;

			
			// Transform to bitmap
			var ltfPicData:BitmapData = new BitmapData(80, 20, false); 
			ltfPicData.draw(_mainMovieClip["leftLabel_notation"], null, null, null, null, false);;

			var picLeft:Bitmap = new Bitmap(ltfPicData);
			picLeft.x = _mainMovieClip["leftLabel_notation"].x;
			picLeft.y = _mainMovieClip["leftLabel_notation"].y;
			picLeft.rotation = -90;
			_leftLabel.addChild(picLeft);
			
			// Set left label digits
			for (var i:int = 0; i < _horLineNum - 1; i++)
			{
				// Build left index
				_mainMovieClip["leftLabel_" + i] = new TextField;
				_leftLabel.addChild(_mainMovieClip["leftLabel_" + i]);
	
				// Set text format position and content
				//_mainMovieClip["leftLabel_" + i].embedFonts = true;
				_mainMovieClip["leftLabel_" + i].width = 11;	
				_mainMovieClip["leftLabel_" + i].x = LABEL_WIDTH - FONT_SIZE;
				_mainMovieClip["leftLabel_" + i].y = i * _horSpace - LABEL_ADJUST * Number((i > 0));				
				_mainMovieClip["leftLabel_" + i].text = _horLineNum - 1 - i;
				_mainMovieClip["leftLabel_" + i].setTextFormat(digitFormat);			
				_mainMovieClip["leftLabel_" + i].selectable = false;
				_mainMovieClip["leftLabel_" + i].autoSize = TextFieldAutoSize.RIGHT;
			}
			
			_botLabel.x = LABEL_WIDTH;
			_botLabel.y = _verLength + TOP_BAR_HEIGHT;// + TITLE_HEIGHT;
			
			// Set bottom notation
			_mainMovieClip["botLabel_notation"]  = new TextField;
			_botLabel.addChild(_mainMovieClip["botLabel_notation"]);
			_mainMovieClip["botLabel_notation"].x = (_horLength - LABEL_WIDTH) / 2;
			_mainMovieClip["botLabel_notation"].y = LABEL_TOP_MARGIN + NOTATION_SIZE;
			_mainMovieClip["botLabel_notation"].embedFonts = true;
			_mainMovieClip["botLabel_notation"].text = "剩余年限(年)";
			_mainMovieClip["botLabel_notation"].setTextFormat(NotationFormat);
			_mainMovieClip["botLabel_notation"].selectable = false;
			//_mainMovieClip["botLabel_notation"].visible = false;
			/*
			// Transform to bitmap
			var botPicData:BitmapData = new BitmapData(90, 20, false); 
			botPicData.draw(_mainMovieClip["botLabel_notation"]);

			var picBot:Bitmap = new Bitmap(botPicData);
			picBot.x = _mainMovieClip["botLabel_notation"].x;
			picBot.y = _mainMovieClip["botLabel_notation"].y;
			_botLabel.addChild(picBot);
			*/
			// Set bot label digits
			for (i = 0; i < _verLineNum; i++)
			{
				// Build right index
				_mainMovieClip["botLabel_" + i] = new TextField;
				_botLabel.addChild(_mainMovieClip["botLabel_" + i]);	
				
				//_mainMovieClip["botLabel_" + i].embedFonts = true;
				_mainMovieClip["botLabel_" + i].x = i * _verSpace - LABEL_ADJUST;					;
				_mainMovieClip["botLabel_" + i].y = LABEL_TOP_MARGIN;
				_mainMovieClip["botLabel_" + i].text = String(i);	
				_mainMovieClip["botLabel_" + i].setTextFormat(digitFormat);
				_mainMovieClip["botLabel_" + i].selectable = false;
			}			
		}
		
		private function setCurve()
		{
			////////////////////////////////////
			// Draw Curve
			////////////////////////////////////
			var paramNum:Number;
			var begPoint:Point = new Point;
			var endPoint:Point = new Point;
			var center:Point = new Point;
			
			_natlogCurve = new YieldCurve(_verLineNum - 1, _horLineNum - 1, BG_WIDTH, BG_HEIGHT);
			_bsplineCurve = new YieldCurve(_verLineNum - 1, _horLineNum - 1, BG_WIDTH, BG_HEIGHT);
			//trace(_horLineNum - 1);
			_grid.addChild(_natlogCurve);
			_grid.addChild(_bsplineCurve);
			
			_natlogCurve.visible = true;
			_bsplineCurve.visible = true;			
			// Set parameters for natural logarithm fitting curve
			paramNum = _sourceData["natlog_num"];
			for (var i:int = 0; i < paramNum; i++)
			{
				_natlogCurve.addParam(_sourceData["natlog_param" + i]);
			}
			_natlogCurve.interpolation(_natlogCurve.interpFunc("natlog"));
			_natlogCurve.plot(GeneralGraphic.COL_RED);
			
			// Set parameters for B-spline fitting curve
			paramNum = _sourceData["bspline_num"];
			for (i = 0; i < paramNum; i++)
			{
				_bsplineCurve.addParam(_sourceData["bspline_param" + i]);
			}
			_bsplineCurve.interpolation(_bsplineCurve.interpFunc("bspline"));
			_bsplineCurve.plot(GeneralGraphic.COL_DARK_BLUE);			
			
			// Draw trace line
			_traceLine = new Sprite;
			_traceLine.visible = false;
			_traceLine.graphics.lineStyle(CURVE_PIXEL, GeneralGraphic.COL_DARK_YELLOW);
			_traceLine.x = 0;
			_traceLine.y = 0;
			_grid.addChild(_traceLine);
			
			begPoint.x = 0;
			begPoint.y = 0;
			
			endPoint.x = 0;
			endPoint.y = _verLength;
			_drawPen.drawLine(_traceLine.graphics, begPoint, endPoint);
			
			var dropShadow:DropShadowFilter = new DropShadowFilter();
			dropShadow.distance = 1;
			dropShadow.color = GeneralGraphic.COL_NEARLY_BLACK;			
			_traceLine.filters = new Array(dropShadow);						
			_stage.addEventListener(MouseEvent.MOUSE_MOVE, traceHandle);
			
			// Draw trace point;
			_traceNatLog = new Sprite;
			_traceBspline = new Sprite;
			center.x = 0;
			center.y = 0;
			
			_drawPen.fillCircle(_traceNatLog.graphics, center, TRACE_RADIUS, GeneralGraphic.COL_RED);
			_traceLine.addChild(_traceNatLog);
			_traceNatLog.visible = false;

			_drawPen.fillCircle(_traceBspline.graphics, center, TRACE_RADIUS, GeneralGraphic.COL_DARK_BLUE);
			_traceLine.addChild(_traceBspline);
			_traceBspline.visible = false;
			
			// Set curve board
			_grid.addChild(_curveInfoBoard);
			_curveInfoBoard.x = CURVE_BOARD_X;
			_curveInfoBoard.y = CURVE_BOARD_Y;		
			_curveInfoBoard.visible = false;
		}
		
		private function traceHandle(event:MouseEvent):void
		{
			var curveData:Array = new Array();
			var pos:int;
			
			// Move trace line
			if (_stage.mouseX >= LABEL_WIDTH  && _stage.mouseX <= (_horLength + LABEL_WIDTH))
			{
				_traceLine.x = _stage.mouseX - LABEL_WIDTH;
				pos = _traceLine.x ;

				//trace(pos);
				// Update data of curve information board
				curveData[0] = _natlogCurve.getCoVar(pos);
				//trace(curveData[0]);
				curveData[1] = _natlogCurve.getResVar(pos);
				curveData[2] = _bsplineCurve.getResVar(pos);				
				_curveInfoBoard.setTextContent(curveData);
				
				// Set trace point of natural logarithm
				if (curveData[1] == "数据越界" || curveData[1] == "未激活")
				{
					_traceNatLog.visible = false;
				}
				else
				{
					_traceNatLog.visible = true;					
					_traceNatLog.y = ((_horLineNum - 1- curveData[1]) / (_horLineNum - 1)) * BG_HEIGHT;
				}
				// Set trace point of B-spline
				if (curveData[2] == "数据越界" || curveData[2] == "未激活")
				{
					_traceBspline.visible = false;
				}
				else
				{
					_traceBspline.visible = true;					
					_traceBspline.y = ((_horLineNum - 1- curveData[2]) / (_horLineNum - 1)) * BG_HEIGHT;
				}				

			}
		}
	}
	
}
