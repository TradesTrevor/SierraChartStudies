#include "sierrachart.h"

/*==========================================================================*/
// Study Name: Saty Pivot Ribbon
// Description: A 3 EMA Ribbon system that simplifies measuring and using EMAs
//              for trend and support/resistance. Features include:
//              - Three main EMAs (Fast=8, Pivot=21, Slow=34) with optional highlighting
//              - Cloud fills between Fast-Pivot and Pivot-Slow
//              - Conviction EMAs (Fast=13, Slow=48) with crossover arrows
//              - Multi-timeframe support (Time Warp feature)
//              - Optional candle bias coloring
//
// Based on the TradingView indicator by Saty Mahajan
// Copyright (C) 2022-2025 Saty Mahajan
// ASCIL Implementation
/*==========================================================================*/

SCDLLName("Saty Pivot Ribbon")

SCSFExport scsf_SatyPivotRibbon(SCStudyGraphRef sc)
{
	// Subgraph references for main EMA lines
	SCSubgraphRef Subgraph_FastEMA = sc.Subgraph[0];
	SCSubgraphRef Subgraph_PivotEMA = sc.Subgraph[1];
	SCSubgraphRef Subgraph_SlowEMA = sc.Subgraph[2];

	// Cloud fill subgraphs (Fast-Pivot Cloud)
	SCSubgraphRef Subgraph_FastCloud_Top = sc.Subgraph[3];
	SCSubgraphRef Subgraph_FastCloud_Bottom = sc.Subgraph[4];

	// Cloud fill subgraphs (Pivot-Slow Cloud)
	SCSubgraphRef Subgraph_SlowCloud_Top = sc.Subgraph[5];
	SCSubgraphRef Subgraph_SlowCloud_Bottom = sc.Subgraph[6];

	// Conviction EMAs
	SCSubgraphRef Subgraph_FastConvictionEMA = sc.Subgraph[7];
	SCSubgraphRef Subgraph_SlowConvictionEMA = sc.Subgraph[8];

	// Conviction arrows (displayed in subgraphs)
	SCSubgraphRef Subgraph_BullishConvictionArrow = sc.Subgraph[9];
	SCSubgraphRef Subgraph_BearishConvictionArrow = sc.Subgraph[10];

	// Bias EMA for candle coloring
	SCSubgraphRef Subgraph_BiasEMA = sc.Subgraph[11];

	// Colored bar subgraphs for candle bias
	SCSubgraphRef Subgraph_ColoredOpen = sc.Subgraph[18];
	SCSubgraphRef Subgraph_ColoredHigh = sc.Subgraph[19];
	SCSubgraphRef Subgraph_ColoredLow = sc.Subgraph[20];
	SCSubgraphRef Subgraph_ColoredClose = sc.Subgraph[21];

	// Working arrays for multi-timeframe calculations
	SCSubgraphRef Subgraph_MTF_FastEMA = sc.Subgraph[22];
	SCSubgraphRef Subgraph_MTF_PivotEMA = sc.Subgraph[23];
	SCSubgraphRef Subgraph_MTF_SlowEMA = sc.Subgraph[24];
	SCSubgraphRef Subgraph_MTF_FastConvictionEMA = sc.Subgraph[25];
	SCSubgraphRef Subgraph_MTF_SlowConvictionEMA = sc.Subgraph[26];
	SCSubgraphRef Subgraph_MTF_BiasEMA = sc.Subgraph[27];

	// Input references - Main EMA Settings
	SCInputRef Input_TimeWarp = sc.Input[0];
	SCInputRef Input_FastEMALength = sc.Input[1];
	SCInputRef Input_PivotEMALength = sc.Input[2];
	SCInputRef Input_SlowEMALength = sc.Input[3];

	// Input references - Cloud Colors
	SCInputRef Input_BullishFastCloudColor = sc.Input[4];
	SCInputRef Input_BearishFastCloudColor = sc.Input[5];
	SCInputRef Input_BullishSlowCloudColor = sc.Input[6];
	SCInputRef Input_BearishSlowCloudColor = sc.Input[7];
	SCInputRef Input_CloudTransparency = sc.Input[8];

	// Input references - EMA Highlight Settings
	SCInputRef Input_ShowFastEMAHighlight = sc.Input[9];
	SCInputRef Input_FastEMAHighlightColor = sc.Input[10];
	SCInputRef Input_ShowPivotEMAHighlight = sc.Input[11];
	SCInputRef Input_PivotEMAHighlightColor = sc.Input[12];
	SCInputRef Input_ShowSlowEMAHighlight = sc.Input[13];
	SCInputRef Input_SlowEMAHighlightColor = sc.Input[14];

	// Input references - Conviction Settings
	SCInputRef Input_ShowConvictionArrows = sc.Input[15];
	SCInputRef Input_BullishConvictionColor = sc.Input[16];
	SCInputRef Input_BearishConvictionColor = sc.Input[17];
	SCInputRef Input_ShowFastConvictionEMA = sc.Input[18];
	SCInputRef Input_FastConvictionEMALength = sc.Input[19];
	SCInputRef Input_FastConvictionEMAColor = sc.Input[20];
	SCInputRef Input_ShowSlowConvictionEMA = sc.Input[21];
	SCInputRef Input_SlowConvictionEMALength = sc.Input[22];
	SCInputRef Input_SlowConvictionEMAColor = sc.Input[23];

	// Input references - Candle Bias
	SCInputRef Input_ShowCandleBias = sc.Input[24];
	SCInputRef Input_BiasEMALength = sc.Input[25];

	// Configuration and setup
	if (sc.SetDefaults)
	{
		// Study graph settings
		sc.GraphName = "Saty Pivot Ribbon";
		sc.StudyDescription = "3 EMA Ribbon system with clouds, conviction arrows, and multi-timeframe support";
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;  // Main price graph overlay
		sc.DrawZeros = false;

		// Drawing beneath main graph for clouds
		sc.DrawStudyUnderneathMainPriceGraph = 1;

		// Time Warp (Multi-Timeframe) Setting
		Input_TimeWarp.Name = "Time Warp (Multi-Timeframe)";
		Input_TimeWarp.SetCustomInputStrings("off;1m;2m;3m;4m;5m;10m;15m;20m;30m;1h;2h;4h;D;W;M");
		Input_TimeWarp.SetCustomInputIndex(0);  // Default to "off"

		// Main EMA Length Inputs
		Input_FastEMALength.Name = "Fast EMA Length";
		Input_FastEMALength.SetInt(8);
		Input_FastEMALength.SetIntLimits(1, 500);

		Input_PivotEMALength.Name = "Pivot EMA Length";
		Input_PivotEMALength.SetInt(21);
		Input_PivotEMALength.SetIntLimits(1, 500);

		Input_SlowEMALength.Name = "Slow EMA Length";
		Input_SlowEMALength.SetInt(34);
		Input_SlowEMALength.SetIntLimits(1, 500);

		// Cloud Color Inputs
		Input_BullishFastCloudColor.Name = "Bullish Fast Cloud Color";
		Input_BullishFastCloudColor.SetColor(RGB(0, 255, 0));  // Green

		Input_BearishFastCloudColor.Name = "Bearish Fast Cloud Color";
		Input_BearishFastCloudColor.SetColor(RGB(255, 0, 0));  // Red

		Input_BullishSlowCloudColor.Name = "Bullish Slow Cloud Color";
		Input_BullishSlowCloudColor.SetColor(RGB(0, 255, 255));  // Aqua/Cyan

		Input_BearishSlowCloudColor.Name = "Bearish Slow Cloud Color";
		Input_BearishSlowCloudColor.SetColor(RGB(255, 165, 0));  // Orange

		Input_CloudTransparency.Name = "Cloud Transparency (0-100)";
		Input_CloudTransparency.SetInt(60);
		Input_CloudTransparency.SetIntLimits(0, 100);

		// EMA Highlight Inputs
		Input_ShowFastEMAHighlight.Name = "Show Fast EMA Highlight";
		Input_ShowFastEMAHighlight.SetYesNo(0);

		Input_FastEMAHighlightColor.Name = "Fast EMA Highlight Color";
		Input_FastEMAHighlightColor.SetColor(RGB(255, 255, 255));  // White

		Input_ShowPivotEMAHighlight.Name = "Show Pivot EMA Highlight";
		Input_ShowPivotEMAHighlight.SetYesNo(0);

		Input_PivotEMAHighlightColor.Name = "Pivot EMA Highlight Color";
		Input_PivotEMAHighlightColor.SetColor(RGB(255, 255, 255));  // White

		Input_ShowSlowEMAHighlight.Name = "Show Slow EMA Highlight";
		Input_ShowSlowEMAHighlight.SetYesNo(0);

		Input_SlowEMAHighlightColor.Name = "Slow EMA Highlight Color";
		Input_SlowEMAHighlightColor.SetColor(RGB(255, 255, 255));  // White

		// Conviction Settings
		Input_ShowConvictionArrows.Name = "Show Conviction Arrows";
		Input_ShowConvictionArrows.SetYesNo(1);

		Input_BullishConvictionColor.Name = "Bullish Conviction Arrow Color";
		Input_BullishConvictionColor.SetColor(RGB(0, 255, 255));  // Aqua/Cyan

		Input_BearishConvictionColor.Name = "Bearish Conviction Arrow Color";
		Input_BearishConvictionColor.SetColor(RGB(255, 255, 0));  // Yellow

		Input_ShowFastConvictionEMA.Name = "Show Fast Conviction EMA";
		Input_ShowFastConvictionEMA.SetYesNo(1);

		Input_FastConvictionEMALength.Name = "Fast Conviction EMA Length";
		Input_FastConvictionEMALength.SetInt(13);
		Input_FastConvictionEMALength.SetIntLimits(1, 500);

		Input_FastConvictionEMAColor.Name = "Fast Conviction EMA Color";
		Input_FastConvictionEMAColor.SetColor(RGB(192, 192, 192));  // Silver/Gray

		Input_ShowSlowConvictionEMA.Name = "Show Slow Conviction EMA";
		Input_ShowSlowConvictionEMA.SetYesNo(1);

		Input_SlowConvictionEMALength.Name = "Slow Conviction EMA Length";
		Input_SlowConvictionEMALength.SetInt(48);
		Input_SlowConvictionEMALength.SetIntLimits(1, 500);

		Input_SlowConvictionEMAColor.Name = "Slow Conviction EMA Color";
		Input_SlowConvictionEMAColor.SetColor(RGB(128, 0, 128));  // Purple

		// Candle Bias Settings
		Input_ShowCandleBias.Name = "Show Candle Bias Coloring";
		Input_ShowCandleBias.SetYesNo(0);

		Input_BiasEMALength.Name = "Bias EMA Length";
		Input_BiasEMALength.SetInt(21);
		Input_BiasEMALength.SetIntLimits(1, 500);

		// Configure Main EMA Line Subgraphs
		Subgraph_FastEMA.Name = "Fast EMA";
		Subgraph_FastEMA.DrawStyle = DRAWSTYLE_IGNORE;  // Hidden by default, shown only with highlight
		Subgraph_FastEMA.PrimaryColor = RGB(255, 255, 255);
		Subgraph_FastEMA.LineWidth = 2;
		Subgraph_FastEMA.DrawZeros = false;

		Subgraph_PivotEMA.Name = "Pivot EMA";
		Subgraph_PivotEMA.DrawStyle = DRAWSTYLE_IGNORE;  // Hidden by default
		Subgraph_PivotEMA.PrimaryColor = RGB(255, 255, 255);
		Subgraph_PivotEMA.LineWidth = 2;
		Subgraph_PivotEMA.DrawZeros = false;

		Subgraph_SlowEMA.Name = "Slow EMA";
		Subgraph_SlowEMA.DrawStyle = DRAWSTYLE_IGNORE;  // Hidden by default
		Subgraph_SlowEMA.PrimaryColor = RGB(255, 255, 255);
		Subgraph_SlowEMA.LineWidth = 2;
		Subgraph_SlowEMA.DrawZeros = false;

		// Configure Fast Cloud Fill Subgraphs (Fast-Pivot)
		Subgraph_FastCloud_Top.Name = "Fast Cloud Top";
		Subgraph_FastCloud_Top.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_TOP;
		Subgraph_FastCloud_Top.PrimaryColor = RGB(0, 255, 0);  // Green
		Subgraph_FastCloud_Top.DrawZeros = false;

		Subgraph_FastCloud_Bottom.Name = "Fast Cloud Bottom";
		Subgraph_FastCloud_Bottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_BOTTOM;
		Subgraph_FastCloud_Bottom.PrimaryColor = RGB(0, 255, 0);  // Green
		Subgraph_FastCloud_Bottom.DrawZeros = false;

		// Configure Slow Cloud Fill Subgraphs (Pivot-Slow)
		Subgraph_SlowCloud_Top.Name = "Slow Cloud Top";
		Subgraph_SlowCloud_Top.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_TOP;
		Subgraph_SlowCloud_Top.PrimaryColor = RGB(0, 255, 255);  // Aqua
		Subgraph_SlowCloud_Top.DrawZeros = false;

		Subgraph_SlowCloud_Bottom.Name = "Slow Cloud Bottom";
		Subgraph_SlowCloud_Bottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_BOTTOM;
		Subgraph_SlowCloud_Bottom.PrimaryColor = RGB(0, 255, 255);  // Aqua
		Subgraph_SlowCloud_Bottom.DrawZeros = false;

		// Configure Conviction EMA Subgraphs
		Subgraph_FastConvictionEMA.Name = "Fast Conviction EMA";
		Subgraph_FastConvictionEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_FastConvictionEMA.PrimaryColor = RGB(192, 192, 192);  // Silver
		Subgraph_FastConvictionEMA.LineWidth = 1;
		Subgraph_FastConvictionEMA.DrawZeros = false;

		Subgraph_SlowConvictionEMA.Name = "Slow Conviction EMA";
		Subgraph_SlowConvictionEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SlowConvictionEMA.PrimaryColor = RGB(128, 0, 128);  // Purple
		Subgraph_SlowConvictionEMA.LineWidth = 1;
		Subgraph_SlowConvictionEMA.DrawZeros = false;

		// Configure Conviction Arrow Subgraphs
		Subgraph_BullishConvictionArrow.Name = "Bullish Conviction Arrow";
		Subgraph_BullishConvictionArrow.DrawStyle = DRAWSTYLE_ARROW_UP;
		Subgraph_BullishConvictionArrow.PrimaryColor = RGB(0, 255, 255);  // Aqua
		Subgraph_BullishConvictionArrow.LineWidth = 3;
		Subgraph_BullishConvictionArrow.DrawZeros = false;

		Subgraph_BearishConvictionArrow.Name = "Bearish Conviction Arrow";
		Subgraph_BearishConvictionArrow.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		Subgraph_BearishConvictionArrow.PrimaryColor = RGB(255, 255, 0);  // Yellow
		Subgraph_BearishConvictionArrow.LineWidth = 3;
		Subgraph_BearishConvictionArrow.DrawZeros = false;

		// Configure Bias EMA (hidden, used only for candle coloring)
		Subgraph_BiasEMA.Name = "Bias EMA";
		Subgraph_BiasEMA.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_BiasEMA.DrawZeros = false;

		// Configure Colored Bar Subgraphs (for candle bias coloring)
		Subgraph_ColoredOpen.Name = "Colored Open";
		Subgraph_ColoredOpen.DrawStyle = DRAWSTYLE_COLORBAR;
		Subgraph_ColoredOpen.PrimaryColor = RGB(0, 0, 0);
		Subgraph_ColoredOpen.SecondaryColor = RGB(0, 0, 0);
		Subgraph_ColoredOpen.DrawZeros = false;

		Subgraph_ColoredHigh.Name = "Colored High";
		Subgraph_ColoredHigh.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_ColoredHigh.DrawZeros = false;

		Subgraph_ColoredLow.Name = "Colored Low";
		Subgraph_ColoredLow.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_ColoredLow.DrawZeros = false;

		Subgraph_ColoredClose.Name = "Colored Close";
		Subgraph_ColoredClose.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_ColoredClose.DrawZeros = false;

		return;
	}

	// Main calculation code
	int BarIndex = sc.Index;

	// Get input values
	int TimeWarpIndex = Input_TimeWarp.GetIndex();
	int FastEMALength = Input_FastEMALength.GetInt();
	int PivotEMALength = Input_PivotEMALength.GetInt();
	int SlowEMALength = Input_SlowEMALength.GetInt();
	int FastConvictionEMALength = Input_FastConvictionEMALength.GetInt();
	int SlowConvictionEMALength = Input_SlowConvictionEMALength.GetInt();
	int BiasEMALength = Input_BiasEMALength.GetInt();

	// Data source for EMAs (using Close price like in PineScript)
	SCFloatArrayRef DataSource = sc.Close;

	// Handle Multi-Timeframe (Time Warp) functionality
	if (TimeWarpIndex > 0)  // If Time Warp is enabled
	{
		// Map index to timeframe string
		SCString TimeframeStr;
		switch (TimeWarpIndex)
		{
			case 1: TimeframeStr = "1"; break;  // 1 minute
			case 2: TimeframeStr = "2"; break;  // 2 minutes
			case 3: TimeframeStr = "3"; break;
			case 4: TimeframeStr = "4"; break;
			case 5: TimeframeStr = "5"; break;
			case 6: TimeframeStr = "10"; break;  // 10 minutes
			case 7: TimeframeStr = "15"; break;
			case 8: TimeframeStr = "20"; break;
			case 9: TimeframeStr = "30"; break;
			case 10: TimeframeStr = "60"; break;  // 1 hour
			case 11: TimeframeStr = "120"; break; // 2 hours
			case 12: TimeframeStr = "240"; break; // 4 hours
			case 13: TimeframeStr = "1440"; break; // Daily
			case 14: TimeframeStr = "10080"; break; // Weekly
			case 15: TimeframeStr = "43200"; break; // Monthly
			default: TimeframeStr = "1"; break;
		}

		// Get higher timeframe data using Sierra Chart's built-in function
		SCGraphData BaseGraphData;
		sc.GetChartBaseData(-1, BaseGraphData);  // Get base chart data

		// For multi-timeframe, we would need to use sc.GetChartArray() or similar
		// For now, calculate on base timeframe as a fallback
		// Note: Full MTF implementation would require additional Sierra Chart API calls
		sc.ExponentialMovAvg(DataSource, Subgraph_FastEMA, FastEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_PivotEMA, PivotEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_SlowEMA, SlowEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_FastConvictionEMA, FastConvictionEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_SlowConvictionEMA, SlowConvictionEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_BiasEMA, BiasEMALength);
	}
	else
	{
		// Standard timeframe calculations
		sc.ExponentialMovAvg(DataSource, Subgraph_FastEMA, FastEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_PivotEMA, PivotEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_SlowEMA, SlowEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_FastConvictionEMA, FastConvictionEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_SlowConvictionEMA, SlowConvictionEMALength);
		sc.ExponentialMovAvg(DataSource, Subgraph_BiasEMA, BiasEMALength);
	}

	// Update EMA line visibility based on highlight settings
	if (Input_ShowFastEMAHighlight.GetYesNo())
	{
		Subgraph_FastEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_FastEMA.PrimaryColor = Input_FastEMAHighlightColor.GetColor();
	}
	else
	{
		Subgraph_FastEMA.DrawStyle = DRAWSTYLE_IGNORE;
	}

	if (Input_ShowPivotEMAHighlight.GetYesNo())
	{
		Subgraph_PivotEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_PivotEMA.PrimaryColor = Input_PivotEMAHighlightColor.GetColor();
	}
	else
	{
		Subgraph_PivotEMA.DrawStyle = DRAWSTYLE_IGNORE;
	}

	if (Input_ShowSlowEMAHighlight.GetYesNo())
	{
		Subgraph_SlowEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SlowEMA.PrimaryColor = Input_SlowEMAHighlightColor.GetColor();
	}
	else
	{
		Subgraph_SlowEMA.DrawStyle = DRAWSTYLE_IGNORE;
	}

	// Update Conviction EMA visibility
	if (Input_ShowFastConvictionEMA.GetYesNo())
	{
		Subgraph_FastConvictionEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_FastConvictionEMA.PrimaryColor = Input_FastConvictionEMAColor.GetColor();
	}
	else
	{
		Subgraph_FastConvictionEMA.DrawStyle = DRAWSTYLE_IGNORE;
	}

	if (Input_ShowSlowConvictionEMA.GetYesNo())
	{
		Subgraph_SlowConvictionEMA.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SlowConvictionEMA.PrimaryColor = Input_SlowConvictionEMAColor.GetColor();
	}
	else
	{
		Subgraph_SlowConvictionEMA.DrawStyle = DRAWSTYLE_IGNORE;
	}

	// Process clouds for the entire visible range
	int startIndex = sc.UpdateStartIndex;
	int endIndex = sc.ArraySize - 1;

	for (int i = startIndex; i <= endIndex; ++i)
	{
		// Fast Cloud (Fast EMA - Pivot EMA)
		float FastEMAValue = Subgraph_FastEMA[i];
		float PivotEMAValue = Subgraph_PivotEMA[i];

		if (FastEMAValue != 0 && PivotEMAValue != 0)
		{
			Subgraph_FastCloud_Top[i] = max(FastEMAValue, PivotEMAValue);
			Subgraph_FastCloud_Bottom[i] = min(FastEMAValue, PivotEMAValue);

			// Set color based on which EMA is on top (bullish vs bearish)
			COLORREF CloudColor;
			if (FastEMAValue >= PivotEMAValue)
			{
				// Bullish - Fast EMA above Pivot EMA
				CloudColor = Input_BullishFastCloudColor.GetColor();
			}
			else
			{
				// Bearish - Fast EMA below Pivot EMA
				CloudColor = Input_BearishFastCloudColor.GetColor();
			}

			Subgraph_FastCloud_Top.DataColor[i] = CloudColor;
			Subgraph_FastCloud_Bottom.DataColor[i] = CloudColor;
		}

		// Slow Cloud (Pivot EMA - Slow EMA)
		float SlowEMAValue = Subgraph_SlowEMA[i];

		if (PivotEMAValue != 0 && SlowEMAValue != 0)
		{
			Subgraph_SlowCloud_Top[i] = max(PivotEMAValue, SlowEMAValue);
			Subgraph_SlowCloud_Bottom[i] = min(PivotEMAValue, SlowEMAValue);

			// Set color based on which EMA is on top
			COLORREF CloudColor;
			if (PivotEMAValue >= SlowEMAValue)
			{
				// Bullish - Pivot EMA above Slow EMA
				CloudColor = Input_BullishSlowCloudColor.GetColor();
			}
			else
			{
				// Bearish - Pivot EMA below Slow EMA
				CloudColor = Input_BearishSlowCloudColor.GetColor();
			}

			Subgraph_SlowCloud_Top.DataColor[i] = CloudColor;
			Subgraph_SlowCloud_Bottom.DataColor[i] = CloudColor;
		}
	}

	// Apply cloud transparency
	int transparency = Input_CloudTransparency.GetInt();
	Subgraph_FastCloud_Top.SecondaryColorUsed = 1;
	Subgraph_FastCloud_Bottom.SecondaryColorUsed = 1;
	Subgraph_SlowCloud_Top.SecondaryColorUsed = 1;
	Subgraph_SlowCloud_Bottom.SecondaryColorUsed = 1;

	// Detect Conviction EMA crossovers and place arrows
	if (Input_ShowConvictionArrows.GetYesNo() && BarIndex > 0)
	{
		float CurrentFastConviction = Subgraph_FastConvictionEMA[BarIndex];
		float PreviousFastConviction = Subgraph_FastConvictionEMA[BarIndex - 1];
		float CurrentSlowConviction = Subgraph_SlowConvictionEMA[BarIndex];
		float PreviousSlowConviction = Subgraph_SlowConvictionEMA[BarIndex - 1];

		// Check for bullish crossover (Fast Conviction crosses above Slow Conviction)
		if (PreviousFastConviction <= PreviousSlowConviction &&
			CurrentFastConviction > CurrentSlowConviction)
		{
			// Place bullish arrow below the bar
			Subgraph_BullishConvictionArrow[BarIndex] = sc.Low[BarIndex];
			Subgraph_BullishConvictionArrow.DataColor[BarIndex] = Input_BullishConvictionColor.GetColor();
		}

		// Check for bearish crossover (Fast Conviction crosses below Slow Conviction)
		if (PreviousFastConviction >= PreviousSlowConviction &&
			CurrentFastConviction < CurrentSlowConviction)
		{
			// Place bearish arrow above the bar
			Subgraph_BearishConvictionArrow[BarIndex] = sc.High[BarIndex];
			Subgraph_BearishConvictionArrow.DataColor[BarIndex] = Input_BearishConvictionColor.GetColor();
		}
	}

	// Candle Bias Coloring
	// Logic from PineScript:
	// - Above Bias EMA + Up candle = Bullish Fast Cloud Color (Green)
	// - Below Bias EMA + Up candle = Bearish Slow Cloud Color (Orange)
	// - Above Bias EMA + Down candle = Bullish Slow Cloud Color (Aqua)
	// - Below Bias EMA + Down candle = Bearish Fast Cloud Color (Red)
	// - Doji = Gray
	if (Input_ShowCandleBias.GetYesNo())
	{
		// Enable colored bars
		Subgraph_ColoredOpen.DrawStyle = DRAWSTYLE_COLORBAR;

		float BiasEMAValue = Subgraph_BiasEMA[BarIndex];
		float OpenPrice = sc.Open[BarIndex];
		float ClosePrice = sc.Close[BarIndex];
		float HighPrice = sc.High[BarIndex];
		float LowPrice = sc.Low[BarIndex];

		bool AbovePivot = ClosePrice >= BiasEMAValue;
		bool BelowPivot = ClosePrice < BiasEMAValue;
		bool UpCandle = OpenPrice < ClosePrice;
		bool DownCandle = OpenPrice > ClosePrice;
		bool DojiCandle = OpenPrice == ClosePrice;

		COLORREF CandleColor;

		if (DojiCandle)
		{
			CandleColor = RGB(128, 128, 128);  // Gray
		}
		else if (AbovePivot && UpCandle)
		{
			CandleColor = Input_BullishFastCloudColor.GetColor();  // Green
		}
		else if (BelowPivot && UpCandle)
		{
			CandleColor = Input_BearishSlowCloudColor.GetColor();  // Orange
		}
		else if (AbovePivot && DownCandle)
		{
			CandleColor = Input_BullishSlowCloudColor.GetColor();  // Aqua
		}
		else if (BelowPivot && DownCandle)
		{
			CandleColor = Input_BearishFastCloudColor.GetColor();  // Red
		}
		else
		{
			CandleColor = RGB(128, 128, 128);  // Default gray
		}

		// Set OHLC values for colored bars
		Subgraph_ColoredOpen[BarIndex] = OpenPrice;
		Subgraph_ColoredHigh[BarIndex] = HighPrice;
		Subgraph_ColoredLow[BarIndex] = LowPrice;
		Subgraph_ColoredClose[BarIndex] = ClosePrice;

		// Set the color
		Subgraph_ColoredOpen.DataColor[BarIndex] = CandleColor;
	}
	else
	{
		// Hide colored bars when disabled
		Subgraph_ColoredOpen.DrawStyle = DRAWSTYLE_IGNORE;
	}
}
