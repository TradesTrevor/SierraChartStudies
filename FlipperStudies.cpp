<<<<<<< HEAD
// The top of every source code file must include this line
#include "sierrachart.h"

SCDLLName("FlipperStudies")

SCSFExport scsf_TheFlipper(SCStudyInterfaceRef sc)
{
    // Persistent variables
    float& PrevDelta = sc.GetPersistentFloat(0);
    int& PotentialBearishIndex = sc.GetPersistentInt(1);
    int& PotentialBullishIndex = sc.GetPersistentInt(2);
    int& LastProcessedIndex = sc.GetPersistentInt(3);
    int& IsStrongFlip = sc.GetPersistentInt(4);
    
    // Input parameters
    SCInputRef LookBackPeriod = sc.Input[0];
    SCInputRef DeltaFlipThreshold = sc.Input[1];
    SCInputRef StrongDeltaFlipFactor = sc.Input[2]; 
    SCInputRef RequireNextBarConfirmation = sc.Input[3];
    SCInputRef ConfirmationPointThreshold = sc.Input[4];
    
    // Subgraphs for final signals
    SCSubgraphRef BearishMarker = sc.Subgraph[0];
    SCSubgraphRef BullishMarker = sc.Subgraph[1];
    SCSubgraphRef StrongBearishMarker = sc.Subgraph[2];
    SCSubgraphRef StrongBullishMarker = sc.Subgraph[3];
    
    if (sc.SetDefaults)
    {
        sc.GraphName = "The Flipper";
        sc.StudyDescription = "Marks flips with strength detection and optional next bar confirmation.";
        sc.AutoLoop = 1;
        sc.UpdateAlways = 1;
        sc.GraphRegion = 0;
        
        // Input settings
        LookBackPeriod.Name = "LookBack Period";
        LookBackPeriod.SetInt(15);
        
        DeltaFlipThreshold.Name = "Flip Threshold";
        DeltaFlipThreshold.SetFloat(100.0f);
        
        StrongDeltaFlipFactor.Name = "Strong Flip Multiplier";
        StrongDeltaFlipFactor.SetFloat(2.5f);
        StrongDeltaFlipFactor.SetDescription("Multiple of threshold to consider as strong flip (0 = disabled)");
        
        RequireNextBarConfirmation.Name = "Require Next Bar Confirmation";
        RequireNextBarConfirmation.SetYesNo(0);
        
        ConfirmationPointThreshold.Name = "Confirmation Points Required";
        ConfirmationPointThreshold.SetFloat(0.0f);
        
        BearishMarker.Name = "Bearish Marker";
        BearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        BearishMarker.PrimaryColor = RGB(255, 0, 0);
        BearishMarker.LineWidth = 5;
        BearishMarker.DrawZeros = false;
        
        BullishMarker.Name = "Bullish Marker";
        BullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        BullishMarker.PrimaryColor = RGB(0, 255, 0);
        BullishMarker.LineWidth = 5;
        BullishMarker.DrawZeros = false;
        
        StrongBearishMarker.Name = "Strong Bearish Marker";
        StrongBearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        StrongBearishMarker.PrimaryColor = RGB(255, 128, 0); // Orange
        StrongBearishMarker.LineWidth = 6;
        StrongBearishMarker.DrawZeros = false;
        
        StrongBullishMarker.Name = "Strong Bullish Marker";
        StrongBullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        StrongBullishMarker.PrimaryColor = RGB(0, 128, 255); // Cyan-blue
        StrongBullishMarker.LineWidth = 6;
        StrongBullishMarker.DrawZeros = false;
        
        // Initialize persistent variables
        PrevDelta = 0.0f;
        PotentialBearishIndex = -1;
        PotentialBullishIndex = -1;
        LastProcessedIndex = -1;
        IsStrongFlip = 0;
        
        return;
    }
    
    // Calculate the current bar's delta (ask volume minus bid volume).
    float currentDelta = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];
    float deltaChange = currentDelta - PrevDelta;
    
    // On the first bar, initialize PrevDelta and exit.
    if (sc.Index == 0)
    {
        PrevDelta = currentDelta;
        LastProcessedIndex = 0;
        return;
    }
    
    // Get bar status
    int barStatus = sc.GetBarHasClosedStatus(sc.Index);
    bool isBarClosed = (barStatus == BHCS_BAR_HAS_CLOSED);
    
    // Skip processing if bar hasn't closed
    if (!isBarClosed) {
        return;
    }
    
    // Get confirmation parameters
    float pointThreshold = ConfirmationPointThreshold.GetFloat();
    
    /* ---------- CONFIRMATION LOGIC ---------- */
    if (RequireNextBarConfirmation.GetYesNo() && sc.Index > LastProcessedIndex && isBarClosed)
    {
        // Check for bearish signal confirmation
        if (PotentialBearishIndex >= 0 && PotentialBearishIndex < sc.Index)
        {
            // Confirm if current bar's close is lower than the signal bar's close
            if (sc.Close[sc.Index] < (sc.Close[PotentialBearishIndex] - pointThreshold))
            {
                // Confirmed - display the proper marker
                if (IsStrongFlip == 1)
                {
                    StrongBearishMarker[PotentialBearishIndex] = sc.High[PotentialBearishIndex];
                }
                else
                {
                    BearishMarker[PotentialBearishIndex] = sc.High[PotentialBearishIndex];
                }
            }
            // Reset potential signal
            PotentialBearishIndex = -1;
            IsStrongFlip = 0;
        }
        
        // Check for bullish signal confirmation
        if (PotentialBullishIndex >= 0 && PotentialBullishIndex < sc.Index)
        {
            // Confirm if current bar's close is higher than the signal bar's close
            if (sc.Close[sc.Index] > (sc.Close[PotentialBullishIndex] + pointThreshold))
            {
                // Confirmed - display the proper marker
                if (IsStrongFlip == 1)
                {
                    StrongBullishMarker[PotentialBullishIndex] = sc.Low[PotentialBullishIndex];
                }
                else
                {
                    BullishMarker[PotentialBullishIndex] = sc.Low[PotentialBullishIndex];
                }             
            }
            // Reset potential signal
            PotentialBullishIndex = -1;
            IsStrongFlip = 0;
        }
        
        LastProcessedIndex = sc.Index;
    }
    /* ---------- END CONFIRMATION LOGIC ---------- */
        
    /* FLIP LOGIC */
    float flipThreshold = DeltaFlipThreshold.GetFloat();
    
    // Calculate strong flip threshold
    float strongFlipThreshold = flipThreshold * StrongDeltaFlipFactor.GetFloat();
    bool isStrongFlip = (fabs(deltaChange) >= strongFlipThreshold && StrongDeltaFlipFactor.GetFloat() > 0);
    
    // Only proceed if the change in delta exceeds the flip threshold.
    if (fabs(deltaChange) < flipThreshold)
    {
        PrevDelta = currentDelta;
        return;
    }
    /* END FLIP LOGIC */
    
    // Get the highest high and lowest low over the LookBackPeriod.
    int lookBack = LookBackPeriod.GetInt();
    float highestValue = sc.GetHighest(sc.BaseDataIn[SC_HIGH], lookBack);
    float lowestValue = sc.GetLowest(sc.BaseDataIn[SC_LOW], lookBack);
    bool isExtremeHigh = (sc.High[sc.Index] >= highestValue);
    bool isExtremeLow = (sc.Low[sc.Index] <= lowestValue);
    
    // Check for a bearish or bullish delta flip.
    if (PrevDelta > 0 && currentDelta < 0 && isExtremeHigh)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            // Store potential signal info
            PotentialBearishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
        }
        else
        {
            // No confirmation required
            if (isStrongFlip)
            {
                StrongBearishMarker[sc.Index] = sc.High[sc.Index];
            }
            else
            {
                BearishMarker[sc.Index] = sc.High[sc.Index];
            }
        }
    }
    else if (PrevDelta < 0 && currentDelta > 0 && isExtremeLow)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            // Store potential signal info
            PotentialBullishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
        }
        else
        {
            // No confirmation required
            if (isStrongFlip)
            {
                StrongBullishMarker[sc.Index] = sc.Low[sc.Index];
            }
            else
            {
                BullishMarker[sc.Index] = sc.Low[sc.Index];
            }
        }
    }
    
    // Update PrevDelta for the next bar.
    PrevDelta = currentDelta;
}

SCSFExport scsf_DeltaColoredCandles(SCStudyInterfaceRef sc)
{
    // Input parameters
    SCInputRef PositiveDeltaThreshold = sc.Input[0];
    SCInputRef NegativeDeltaThreshold = sc.Input[1];
    SCInputRef UseAbsoluteValue = sc.Input[2];
    SCInputRef CVDStudyID = sc.Input[3];
	SCInputRef RSIPeriod = sc.Input[4];
    
    // Main subgraphs for candle data
    SCSubgraphRef OpenGraph = sc.Subgraph[0];
    SCSubgraphRef HighGraph = sc.Subgraph[1];
    SCSubgraphRef LowGraph = sc.Subgraph[2];
    SCSubgraphRef CloseGraph = sc.Subgraph[3];
    SCSubgraphRef RSIGraph = sc.Subgraph[4];
    
    // Color subgraphs
    SCSubgraphRef BuyMaxColorGraph = sc.Subgraph[5];
    SCSubgraphRef BuyMedColorGraph = sc.Subgraph[6];
    SCSubgraphRef BuyMinColorGraph = sc.Subgraph[7];
    SCSubgraphRef SellMaxColorGraph = sc.Subgraph[8];
    SCSubgraphRef SellMedColorGraph = sc.Subgraph[9];
    SCSubgraphRef SellMinColorGraph = sc.Subgraph[10];
    SCSubgraphRef NeutralUpColorGraph = sc.Subgraph[11];
    SCSubgraphRef NeutralDownColorGraph = sc.Subgraph[12];
    SCSubgraphRef PositiveDeltaColorGraph = sc.Subgraph[13];
    SCSubgraphRef NegativeDeltaColorGraph = sc.Subgraph[14];
    
    if (sc.SetDefaults)
    {
        sc.GraphName = "Delta Candles";
        sc.StudyDescription = "Colors candles based on delta thresholds.";
        
        sc.AutoLoop = 1;
        sc.GraphRegion = 0;
        
        // Input settings
        PositiveDeltaThreshold.Name = "Positive Delta Threshold";
        PositiveDeltaThreshold.SetFloat(150.0f);
        PositiveDeltaThreshold.SetDescription("Bar changes color when delta exceeds this value");
        
        NegativeDeltaThreshold.Name = "Negative Delta Threshold";
        NegativeDeltaThreshold.SetFloat(-150.0f);
        NegativeDeltaThreshold.SetDescription("Bar changes color when delta is below this value");
        
        UseAbsoluteValue.Name = "Use Absolute Value for Both Thresholds";
        UseAbsoluteValue.SetYesNo(0);
        UseAbsoluteValue.SetDescription("If Yes, Negative Threshold becomes absolute value");
        
        CVDStudyID.Name = "CVD Study ID";
        CVDStudyID.SetStudyID(7);
        CVDStudyID.SetDescription("Set to the Study ID of the CVD study");
        
        // Setup color subgraphs for RSI
        BuyMaxColorGraph.Name = "Buy Max Color";
        BuyMaxColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMaxColorGraph.PrimaryColor = RGB(64, 224, 208);
        
        BuyMedColorGraph.Name = "Buy Med Color";
        BuyMedColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMedColorGraph.PrimaryColor = RGB(0, 128, 128);
        
        BuyMinColorGraph.Name = "Buy Min Color";
        BuyMinColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMinColorGraph.PrimaryColor = RGB(0, 64, 64); 
        
        SellMaxColorGraph.Name = "Sell Max Color";
        SellMaxColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMaxColorGraph.PrimaryColor = RGB(255, 181, 218);
        
        SellMedColorGraph.Name = "Sell Med Color";
        SellMedColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMedColorGraph.PrimaryColor = RGB(255, 0, 0);
        
        SellMinColorGraph.Name = "Sell Min Color";
        SellMinColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMinColorGraph.PrimaryColor = RGB(128, 0, 0); 
        
        NeutralUpColorGraph.Name = "Neutral Up Color";
        NeutralUpColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NeutralUpColorGraph.PrimaryColor = RGB(50, 54, 69);
        
        NeutralDownColorGraph.Name = "Neutral Down Color";
        NeutralDownColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NeutralDownColorGraph.PrimaryColor = RGB(67, 73, 84);
        
        PositiveDeltaColorGraph.Name = "Positive Delta Color";
        PositiveDeltaColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        PositiveDeltaColorGraph.PrimaryColor = RGB(120, 200, 50);
        
        NegativeDeltaColorGraph.Name = "Negative Delta Color";
        NegativeDeltaColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NegativeDeltaColorGraph.PrimaryColor = RGB(130, 50, 200);
		
		    RSIPeriod.Name = "RSI Period";
        RSIPeriod.SetInt(24);
        RSIPeriod.SetIntLimits(1, 100);
        RSIPeriod.SetDescription("Period for RSI calculation");
        
        // Set graph type to OHLC Bar
        sc.GraphDrawType = GDT_OHLCBAR;
        
        // Set to use data from main price graph
        sc.ValueFormat = VALUEFORMAT_INHERITED;
        
        return;
    }
    
    // Copy price data to our subgraphs
    OpenGraph[sc.Index] = sc.Open[sc.Index];
    HighGraph[sc.Index] = sc.High[sc.Index];
    LowGraph[sc.Index] = sc.Low[sc.Index];
    CloseGraph[sc.Index] = sc.Close[sc.Index];
    
    // Default color setup - use neutral colors based on bar direction
    bool isUpBar = sc.Close[sc.Index] >= sc.Open[sc.Index];
    COLORREF barColor;
    
    if (isUpBar)
    {
        barColor = NeutralUpColorGraph.PrimaryColor;
    }
    else
    {
        barColor = NeutralDownColorGraph.PrimaryColor;
    }
    
    // Get input values for delta thresholds
    float posThreshold = PositiveDeltaThreshold.GetFloat();
    float negThreshold = NegativeDeltaThreshold.GetFloat();
    bool useAbsValue = UseAbsoluteValue.GetYesNo();
    
    // If using absolute value, adjust the negative threshold
    if (useAbsValue)
    {
        negThreshold = -fabs(posThreshold);
    }
    
    // Calculate delta (ask volume minus bid volume)
    float currentDelta = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];
    
    // Flag to track if delta coloring should be applied
    bool applyDeltaColor = false;
    COLORREF deltaColor = barColor;
    
    // Check if delta exceeds thresholds
    if (currentDelta >= posThreshold)
    {
        // Positive delta above threshold
        applyDeltaColor = true;
        deltaColor = PositiveDeltaColorGraph.PrimaryColor;
    }
    else if (currentDelta <= negThreshold)
    {
        // Negative delta below threshold
        applyDeltaColor = true;
        deltaColor = NegativeDeltaColorGraph.PrimaryColor;
    }
    
    // Flag to track if RSI coloring should be applied
    bool applyRSIColor = false;
    COLORREF rsiColor = barColor;
    
    // Apply RSI-based coloring
    // Get CVD study ID
    int cvdStudyID = CVDStudyID.GetStudyID();
    if (cvdStudyID > 0)
    {
        // Get CVD values from referenced study
        SCFloatArray cvdArray;
        sc.GetStudyArrayUsingID(cvdStudyID, 0, cvdArray);
        
        if (cvdArray.GetArraySize() > sc.Index)
        {
            // Calculate RSI on CVD data
            sc.RSI(cvdArray, RSIGraph, MOVAVGTYPE_EXPONENTIAL, RSIPeriod.GetInt());
            float currentRSI = RSIGraph[sc.Index];
            
            // Define static thresholds
            const float BUY_MAX_THRESHOLD = 80.0f;
            const float BUY_MED_THRESHOLD = 75.0f;
            const float BUY_MIN_THRESHOLD = 70.0f;
            const float SELL_MIN_THRESHOLD = 30.0f;
            const float SELL_MED_THRESHOLD = 25.0f;
            const float SELL_MAX_THRESHOLD = 20.0f;
            
            // For buy conditions (RSI above thresholds)
            if (currentRSI >= BUY_MAX_THRESHOLD)
            {
                // Max buy signal (RSI >= 80)
                applyRSIColor = true;
                rsiColor = BuyMaxColorGraph.PrimaryColor;
            }
            else if (currentRSI >= BUY_MED_THRESHOLD)
            {
                // Medium buy signal (RSI >= 75)
                applyRSIColor = true;
                rsiColor = BuyMedColorGraph.PrimaryColor;
            }
            else if (currentRSI >= BUY_MIN_THRESHOLD)
            {
                // Minimum buy signal (RSI >= 70)
                applyRSIColor = true;
                rsiColor = BuyMinColorGraph.PrimaryColor;
            }
            // For sell conditions (RSI below thresholds)
            else if (currentRSI <= SELL_MAX_THRESHOLD)
            {
                // Max sell signal (RSI <= 20)
                applyRSIColor = true;
                rsiColor = SellMaxColorGraph.PrimaryColor;
            }
            else if (currentRSI <= SELL_MED_THRESHOLD)
            {
                // Medium sell signal (RSI <= 25)
                applyRSIColor = true;
                rsiColor = SellMedColorGraph.PrimaryColor;
            }
            else if (currentRSI <= SELL_MIN_THRESHOLD)
            {
                // Minimum sell signal (RSI <= 30)
                applyRSIColor = true;
                rsiColor = SellMinColorGraph.PrimaryColor;
            }
        }
        else if (sc.Index == 0)
        {
            sc.AddMessageToLog("CVD Study data not available", 1);
        }
    }
    else if (sc.Index == 0)
    {
        sc.AddMessageToLog("Please set a valid CVD Study ID for RSI coloring", 1);
    }
    
    // Determine final color - always prioritize delta coloring
    if (applyDeltaColor)
    {
        barColor = deltaColor;
    }
    else if (applyRSIColor)
    {
        barColor = rsiColor;
    }
    
    // Set the color for all data points
    OpenGraph.DataColor[sc.Index] = barColor;
    HighGraph.DataColor[sc.Index] = barColor;
    LowGraph.DataColor[sc.Index] = barColor;
    CloseGraph.DataColor[sc.Index] = barColor;
}

SCSFExport scsf_DynamicFlipper(SCStudyInterfaceRef sc)
{
    // Persistent variables
    float& PrevDelta = sc.GetPersistentFloat(0);
    int& PotentialBearishIndex = sc.GetPersistentInt(1);
    int& PotentialBullishIndex = sc.GetPersistentInt(2);
    int& LastProcessedIndex = sc.GetPersistentInt(3);
    int& IsStrongFlip = sc.GetPersistentInt(4);
    float& SignalBarRSI = sc.GetPersistentFloat(5);
    float& PrevFlipThreshold = sc.GetPersistentFloat(6);
	  int& StudyVersion = sc.GetPersistentInt(7);

    
    // Input parameters
    SCInputRef DeltaFlipThreshold = sc.Input[0];
    SCInputRef StrongDeltaFlipFactor = sc.Input[1]; 
    SCInputRef RequireNextBarConfirmation = sc.Input[3];
    SCInputRef ConfirmationPointThreshold = sc.Input[4];
    SCInputRef CVDStudyID = sc.Input[5];
    SCInputRef LookbackPeriod = sc.Input[6];
    SCInputRef ThresholdMultiplier = sc.Input[7];
	  SCInputRef RSIPeriod = sc.Input[8];
    
    // Subgraphs
    SCSubgraphRef BearishMarker = sc.Subgraph[0];
    SCSubgraphRef BullishMarker = sc.Subgraph[1];
    SCSubgraphRef StrongBearishMarker = sc.Subgraph[2];
    SCSubgraphRef StrongBullishMarker = sc.Subgraph[3];
    SCSubgraphRef RSISubgraph = sc.Subgraph[4];
    SCSubgraphRef RSIUpperLine = sc.Subgraph[5];
    SCSubgraphRef RSILowerLine = sc.Subgraph[6];
    SCSubgraphRef DeltaStorage = sc.Subgraph[7];
    
    if (sc.SetDefaults)
    {
        sc.GraphName = "Dynamic Flipper";
        sc.StudyDescription = "Marks flips with strength detection, optional next bar confirmation, and dynamic threshold.";
        sc.AutoLoop = 1;
        sc.UpdateAlways = 1;
        sc.GraphRegion = 0;
        
        DeltaFlipThreshold.Name = "Default Flip Threshold";
        DeltaFlipThreshold.SetFloat(100.0f);
        
        StrongDeltaFlipFactor.Name = "Strong Flip Multiplier";
        StrongDeltaFlipFactor.SetFloat(2.0f);
        
        RequireNextBarConfirmation.Name = "Require Next Bar Confirmation";
        RequireNextBarConfirmation.SetYesNo(1);
        
        ConfirmationPointThreshold.Name = "Confirmation Points Required";
        ConfirmationPointThreshold.SetFloat(2.0f);
        
        CVDStudyID.Name = "CVD Study ID";
        CVDStudyID.SetStudyID(7);
        
        LookbackPeriod.Name = "Lookback Period for Dynamic Threshold";
        LookbackPeriod.SetInt(100);
        LookbackPeriod.SetIntLimits(5, 100);
        
        ThresholdMultiplier.Name = "Dynamic Threshold Multiplier";
        ThresholdMultiplier.SetFloat(1.5f);
        ThresholdMultiplier.SetFloatLimits(0.5f, 10.0f);
        
        BearishMarker.Name = "Bearish Marker";
        BearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        BearishMarker.PrimaryColor = RGB(255, 0, 0);
        BearishMarker.LineWidth = 5;
        BearishMarker.DrawZeros = false;
        
        BullishMarker.Name = "Bullish Marker";
        BullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        BullishMarker.PrimaryColor = RGB(0, 255, 0);
        BullishMarker.LineWidth = 5;
        BullishMarker.DrawZeros = false;
        
        StrongBearishMarker.Name = "Strong Bearish Marker";
        StrongBearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        StrongBearishMarker.PrimaryColor = RGB(255, 128, 0);
        StrongBearishMarker.LineWidth = 6;
        StrongBearishMarker.DrawZeros = false;
        
        StrongBullishMarker.Name = "Strong Bullish Marker";
        StrongBullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        StrongBullishMarker.PrimaryColor = RGB(0, 128, 255);
        StrongBullishMarker.LineWidth = 6;
        StrongBullishMarker.DrawZeros = false;
        
        DeltaStorage.Name = "Delta Storage";
        DeltaStorage.DrawStyle = DRAWSTYLE_IGNORE;
        DeltaStorage.DrawZeros = false;
		
		    RSIPeriod.Name = "RSI Period";
        RSIPeriod.SetInt(24);
        RSIPeriod.SetIntLimits(1, 100);
        RSIPeriod.SetDescription("Period for RSI calculation");
        
        // Initialize persistent variables
        PrevDelta = 0.0f;
        PotentialBearishIndex = -1;
        PotentialBullishIndex = -1;
        LastProcessedIndex = -1;
        IsStrongFlip = 0;
        SignalBarRSI = 0.0f;
        PrevFlipThreshold = 0.0f;
		    StudyVersion = 1;

        
        return;
    }
    
    // Calculate and store the current bar's delta
    float currentDelta = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];
    DeltaStorage[sc.Index] = currentDelta;
    float deltaChange = currentDelta - PrevDelta;
    
    // --- Dynamic Flip Threshold ---
    float flipThreshold = DeltaFlipThreshold.GetFloat();
    int lookback = LookbackPeriod.GetInt();
    bool isDynamic = false;
    float stdDev = 0.0f;
    
    if (lookback > 0 && sc.Index >= lookback)
    {
        // Calculate mean of deltas
        float sum = 0.0f;
        for (int i = 0; i < lookback; ++i)
        {
            int arrayIndex = sc.Index - i;
            if (arrayIndex >= 0)
            {
                sum += DeltaStorage[arrayIndex];
            }
        }
        float mean = sum / lookback;
        
        // Calculate variance
        float sumSquaredDiff = 0.0f;
        for (int i = 0; i < lookback; ++i)
        {
            int arrayIndex = sc.Index - i;
            if (arrayIndex >= 0)
            {
                float diff = DeltaStorage[arrayIndex] - mean;
                sumSquaredDiff += diff * diff;
            }
        }
        float variance = sumSquaredDiff / lookback;
        
        // Calculate standard deviation
        stdDev = sqrtf(variance);
        
        // Set dynamic threshold
        if (stdDev > 0.0f)
        {
            flipThreshold = stdDev * ThresholdMultiplier.GetFloat();
            isDynamic = true;
            
            // Ensure threshold isn't too small
            if (flipThreshold < DeltaFlipThreshold.GetFloat() * 0.5f)
            {
                flipThreshold = DeltaFlipThreshold.GetFloat();
                isDynamic = false;
            }
        }
		
		    // Debug logging
    SCString debugMessage;
    debugMessage.Format("Index %d: FlipThreshold=%.2f, StdDev=%.2f, IsDynamic=%s",
                       sc.Index, flipThreshold, stdDev, isDynamic ? "Yes" : "No");
    sc.AddMessageToLog(debugMessage, 0);
    }
	
    PrevFlipThreshold = flipThreshold;
    
    // --- Rest of the code ---
    int cvdStudyID = CVDStudyID.GetStudyID();
    float currentRSI = 50.0f;
    float rsiUpper = 70.0f;
    float rsiLower = 30.0f;
    
    if (cvdStudyID != 0)
    {
        SCFloatArray cvdArray;
        sc.GetStudyArrayUsingID(cvdStudyID, 0, cvdArray);
        if (cvdArray.GetArraySize() > 0)
        {
            sc.RSI(cvdArray, RSISubgraph, MOVAVGTYPE_EXPONENTIAL, RSIPeriod.GetInt());
            currentRSI = RSISubgraph[sc.Index];
            RSIUpperLine[sc.Index] = rsiUpper;
            RSILowerLine[sc.Index] = rsiLower;
        }
    }
    
    if (sc.Index == 0)
    {
        PrevDelta = currentDelta;
        LastProcessedIndex = 0;
        return;
    }
    
    if (sc.GetBarHasClosedStatus(sc.Index) == BHCS_BAR_HAS_NOT_CLOSED) 
    {    
        return;
    }
    
    float pointThreshold = ConfirmationPointThreshold.GetFloat();
    
    // Confirmation logic
    if (RequireNextBarConfirmation.GetYesNo() && sc.Index > LastProcessedIndex)
    {
        if (PotentialBearishIndex >= 0 && PotentialBearishIndex < sc.Index)
        {
            bool rsiConditionMet = true;
            if (cvdStudyID != 0)
            {
                rsiConditionMet = (SignalBarRSI >= rsiUpper);
            }
            if (rsiConditionMet && sc.Close[sc.Index] < (sc.Close[PotentialBearishIndex] - pointThreshold))
            {
                if (IsStrongFlip == 1)
                {
                    StrongBearishMarker[sc.Index] = sc.High[sc.Index];
                }
                else
                {
                    BearishMarker[sc.Index] = sc.High[sc.Index];
                }
            }
            PotentialBearishIndex = -1;
            IsStrongFlip = 0;
            SignalBarRSI = 0.0f;
        }
        
        if (PotentialBullishIndex >= 0 && PotentialBullishIndex < sc.Index)
        {
            bool rsiConditionMet = true;
            if (cvdStudyID != 0)
            {
                rsiConditionMet = (SignalBarRSI <= rsiLower);
            }
            if (rsiConditionMet && sc.Close[sc.Index] > (sc.Close[PotentialBullishIndex] + pointThreshold))
            {
                if (IsStrongFlip == 1)
                {
                    StrongBullishMarker[sc.Index] = sc.Low[sc.Index];
                }
                else
                {
                    BullishMarker[sc.Index] = sc.Low[sc.Index];
                }             
            }
            PotentialBullishIndex = -1;
            IsStrongFlip = 0;
            SignalBarRSI = 0.0f;
        }
        
        LastProcessedIndex = sc.Index;
    }
    
    // FLIP LOGIC
    float strongFlipThreshold = flipThreshold * StrongDeltaFlipFactor.GetFloat();
    bool isStrongFlip = (fabs(deltaChange) >= strongFlipThreshold && StrongDeltaFlipFactor.GetFloat() > 0);
    
    if (fabs(deltaChange) < flipThreshold)
    {
        PrevDelta = currentDelta;
        return;
    }
    
    bool bearishRSICondition = true;
    bool bullishRSICondition = true;
    
    if (cvdStudyID != 0)
    {
        bearishRSICondition = (currentRSI >= rsiUpper);
        bullishRSICondition = (currentRSI <= rsiLower);
    }
    
    bool bearishDeltaCondition = (PrevDelta > 0 && currentDelta < 0);
    bool bullishDeltaCondition = (PrevDelta < 0 && currentDelta > 0);
    
    if (bearishDeltaCondition && bearishRSICondition)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            PotentialBearishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
            SignalBarRSI = currentRSI;
        }
        else
        {
            if (isStrongFlip)
            {
                StrongBearishMarker[sc.Index] = sc.High[sc.Index];
            }
            else
            {
                BearishMarker[sc.Index] = sc.High[sc.Index];
            }
        }
    }
    else if (bullishDeltaCondition && bullishRSICondition)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            PotentialBullishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
            SignalBarRSI = currentRSI;
        }
        else
        {
            if (isStrongFlip)
            {
                StrongBullishMarker[sc.Index] = sc.Low[sc.Index];
            }
            else
            {
                BullishMarker[sc.Index] = sc.Low[sc.Index];
            }
        }
    }
    
    PrevDelta = currentDelta;
}
=======
// The top of every source code file must include this line
#include "sierrachart.h"

SCDLLName("FlipperStudies")

SCSFExport scsf_TheFlipper(SCStudyInterfaceRef sc)
{
    // Persistent variables
    float& PrevDelta = sc.GetPersistentFloat(0);
    int& PotentialBearishIndex = sc.GetPersistentInt(1);
    int& PotentialBullishIndex = sc.GetPersistentInt(2);
    int& LastProcessedIndex = sc.GetPersistentInt(3);
    int& IsStrongFlip = sc.GetPersistentInt(4);
    
    // Input parameters
    SCInputRef LookBackPeriod = sc.Input[0];
    SCInputRef DeltaFlipThreshold = sc.Input[1];
    SCInputRef StrongDeltaFlipFactor = sc.Input[2]; 
    SCInputRef RequireNextBarConfirmation = sc.Input[3];
    SCInputRef ConfirmationPointThreshold = sc.Input[4];
    
    // Subgraphs for final signals
    SCSubgraphRef BearishMarker = sc.Subgraph[0];
    SCSubgraphRef BullishMarker = sc.Subgraph[1];
    SCSubgraphRef StrongBearishMarker = sc.Subgraph[2];
    SCSubgraphRef StrongBullishMarker = sc.Subgraph[3];
    
    if (sc.SetDefaults)
    {
        sc.GraphName = "The Flipper";
        sc.StudyDescription = "Marks flips with strength detection and optional next bar confirmation.";
        sc.AutoLoop = 1;
        sc.UpdateAlways = 1;
        sc.GraphRegion = 0;
        
        // Input settings
        LookBackPeriod.Name = "LookBack Period";
        LookBackPeriod.SetInt(15);
        
        DeltaFlipThreshold.Name = "Flip Threshold";
        DeltaFlipThreshold.SetFloat(100.0f);
        
        StrongDeltaFlipFactor.Name = "Strong Flip Multiplier";
        StrongDeltaFlipFactor.SetFloat(2.5f);
        StrongDeltaFlipFactor.SetDescription("Multiple of threshold to consider as strong flip (0 = disabled)");
        
        RequireNextBarConfirmation.Name = "Require Next Bar Confirmation";
        RequireNextBarConfirmation.SetYesNo(0);
        
        ConfirmationPointThreshold.Name = "Confirmation Points Required";
        ConfirmationPointThreshold.SetFloat(0.0f);
        
        BearishMarker.Name = "Bearish Marker";
        BearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        BearishMarker.PrimaryColor = RGB(255, 0, 0);
        BearishMarker.LineWidth = 5;
        BearishMarker.DrawZeros = false;
        
        BullishMarker.Name = "Bullish Marker";
        BullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        BullishMarker.PrimaryColor = RGB(0, 255, 0);
        BullishMarker.LineWidth = 5;
        BullishMarker.DrawZeros = false;
        
        StrongBearishMarker.Name = "Strong Bearish Marker";
        StrongBearishMarker.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        StrongBearishMarker.PrimaryColor = RGB(255, 128, 0); // Orange
        StrongBearishMarker.LineWidth = 6;
        StrongBearishMarker.DrawZeros = false;
        
        StrongBullishMarker.Name = "Strong Bullish Marker";
        StrongBullishMarker.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        StrongBullishMarker.PrimaryColor = RGB(0, 128, 255); // Cyan-blue
        StrongBullishMarker.LineWidth = 6;
        StrongBullishMarker.DrawZeros = false;
        
        // Initialize persistent variables
        PrevDelta = 0.0f;
        PotentialBearishIndex = -1;
        PotentialBullishIndex = -1;
        LastProcessedIndex = -1;
        IsStrongFlip = 0;
        
        return;
    }
    
    // Calculate the current bar's delta (ask volume minus bid volume).
    float currentDelta = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];
    float deltaChange = currentDelta - PrevDelta;
    
    // On the first bar, initialize PrevDelta and exit.
    if (sc.Index == 0)
    {
        PrevDelta = currentDelta;
        LastProcessedIndex = 0;
        return;
    }
    
    // Get bar status
    int barStatus = sc.GetBarHasClosedStatus(sc.Index);
    bool isBarClosed = (barStatus == BHCS_BAR_HAS_CLOSED);
    
    // Skip processing if bar hasn't closed
    if (!isBarClosed) {
        return;
    }
    
    // Get confirmation parameters
    float pointThreshold = ConfirmationPointThreshold.GetFloat();
    
    /* ---------- CONFIRMATION LOGIC ---------- */
    if (RequireNextBarConfirmation.GetYesNo() && sc.Index > LastProcessedIndex && isBarClosed)
    {
        // Check for bearish signal confirmation
        if (PotentialBearishIndex >= 0 && PotentialBearishIndex < sc.Index)
        {
            // Confirm if current bar's close is lower than the signal bar's close
            if (sc.Close[sc.Index] < (sc.Close[PotentialBearishIndex] - pointThreshold))
            {
                // Confirmed - display the proper marker
                if (IsStrongFlip == 1)
                {
                    StrongBearishMarker[PotentialBearishIndex] = sc.High[PotentialBearishIndex];
                }
                else
                {
                    BearishMarker[PotentialBearishIndex] = sc.High[PotentialBearishIndex];
                }
            }
            // Reset potential signal
            PotentialBearishIndex = -1;
            IsStrongFlip = 0;
        }
        
        // Check for bullish signal confirmation
        if (PotentialBullishIndex >= 0 && PotentialBullishIndex < sc.Index)
        {
            // Confirm if current bar's close is higher than the signal bar's close
            if (sc.Close[sc.Index] > (sc.Close[PotentialBullishIndex] + pointThreshold))
            {
                // Confirmed - display the proper marker
                if (IsStrongFlip == 1)
                {
                    StrongBullishMarker[PotentialBullishIndex] = sc.Low[PotentialBullishIndex];
                }
                else
                {
                    BullishMarker[PotentialBullishIndex] = sc.Low[PotentialBullishIndex];
                }             
            }
            // Reset potential signal
            PotentialBullishIndex = -1;
            IsStrongFlip = 0;
        }
        
        LastProcessedIndex = sc.Index;
    }
    /* ---------- END CONFIRMATION LOGIC ---------- */
        
    /* FLIP LOGIC */
    float flipThreshold = DeltaFlipThreshold.GetFloat();
    
    // Calculate strong flip threshold
    float strongFlipThreshold = flipThreshold * StrongDeltaFlipFactor.GetFloat();
    bool isStrongFlip = (fabs(deltaChange) >= strongFlipThreshold && StrongDeltaFlipFactor.GetFloat() > 0);
    
    // Only proceed if the change in delta exceeds the flip threshold.
    if (fabs(deltaChange) < flipThreshold)
    {
        PrevDelta = currentDelta;
        return;
    }
    /* END FLIP LOGIC */
    
    // Get the highest high and lowest low over the LookBackPeriod.
    int lookBack = LookBackPeriod.GetInt();
    float highestValue = sc.GetHighest(sc.BaseDataIn[SC_HIGH], lookBack);
    float lowestValue = sc.GetLowest(sc.BaseDataIn[SC_LOW], lookBack);
    bool isExtremeHigh = (sc.High[sc.Index] >= highestValue);
    bool isExtremeLow = (sc.Low[sc.Index] <= lowestValue);
    
    // Check for a bearish or bullish delta flip.
    if (PrevDelta > 0 && currentDelta < 0 && isExtremeHigh)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            // Store potential signal info
            PotentialBearishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
        }
        else
        {
            // No confirmation required
            if (isStrongFlip)
            {
                StrongBearishMarker[sc.Index] = sc.High[sc.Index];
            }
            else
            {
                BearishMarker[sc.Index] = sc.High[sc.Index];
            }
        }
    }
    else if (PrevDelta < 0 && currentDelta > 0 && isExtremeLow)
    {
        if (RequireNextBarConfirmation.GetYesNo())
        {
            // Store potential signal info
            PotentialBullishIndex = sc.Index;
            IsStrongFlip = isStrongFlip ? 1 : 0;
        }
        else
        {
            // No confirmation required
            if (isStrongFlip)
            {
                StrongBullishMarker[sc.Index] = sc.Low[sc.Index];
            }
            else
            {
                BullishMarker[sc.Index] = sc.Low[sc.Index];
            }
        }
    }
    
    // Update PrevDelta for the next bar.
    PrevDelta = currentDelta;
}

SCSFExport scsf_DeltaColoredCandles(SCStudyInterfaceRef sc)
{
    // Input parameters
    SCInputRef PositiveDeltaThreshold = sc.Input[0];
    SCInputRef NegativeDeltaThreshold = sc.Input[1];
    SCInputRef UseAbsoluteValue = sc.Input[2];
    SCInputRef CVDStudyID = sc.Input[3];
	SCInputRef RSIPeriod = sc.Input[4];
    
    // Main subgraphs for candle data
    SCSubgraphRef OpenGraph = sc.Subgraph[0];
    SCSubgraphRef HighGraph = sc.Subgraph[1];
    SCSubgraphRef LowGraph = sc.Subgraph[2];
    SCSubgraphRef CloseGraph = sc.Subgraph[3];
    SCSubgraphRef RSIGraph = sc.Subgraph[4];
    
    // Color subgraphs
    SCSubgraphRef BuyMaxColorGraph = sc.Subgraph[5];
    SCSubgraphRef BuyMedColorGraph = sc.Subgraph[6];
    SCSubgraphRef BuyMinColorGraph = sc.Subgraph[7];
    SCSubgraphRef SellMaxColorGraph = sc.Subgraph[8];
    SCSubgraphRef SellMedColorGraph = sc.Subgraph[9];
    SCSubgraphRef SellMinColorGraph = sc.Subgraph[10];
    SCSubgraphRef NeutralUpColorGraph = sc.Subgraph[11];
    SCSubgraphRef NeutralDownColorGraph = sc.Subgraph[12];
    SCSubgraphRef PositiveDeltaColorGraph = sc.Subgraph[13];
    SCSubgraphRef NegativeDeltaColorGraph = sc.Subgraph[14];
    
    if (sc.SetDefaults)
    {
        sc.GraphName = "Delta Candles";
        sc.StudyDescription = "Colors candles based on delta thresholds.";
        
        sc.AutoLoop = 1;
        sc.GraphRegion = 0;
        
        // Input settings
        PositiveDeltaThreshold.Name = "Positive Delta Threshold";
        PositiveDeltaThreshold.SetFloat(150.0f);
        PositiveDeltaThreshold.SetDescription("Bar changes color when delta exceeds this value");
        
        NegativeDeltaThreshold.Name = "Negative Delta Threshold";
        NegativeDeltaThreshold.SetFloat(-150.0f);
        NegativeDeltaThreshold.SetDescription("Bar changes color when delta is below this value");
        
        UseAbsoluteValue.Name = "Use Absolute Value for Both Thresholds";
        UseAbsoluteValue.SetYesNo(0);
        UseAbsoluteValue.SetDescription("If Yes, Negative Threshold becomes absolute value");
        
        CVDStudyID.Name = "CVD Study ID";
        CVDStudyID.SetStudyID(7);
        CVDStudyID.SetDescription("Set to the Study ID of the CVD study");
        
        // Setup color subgraphs for RSI
        BuyMaxColorGraph.Name = "Buy Max Color";
        BuyMaxColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMaxColorGraph.PrimaryColor = RGB(64, 224, 208);
        
        BuyMedColorGraph.Name = "Buy Med Color";
        BuyMedColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMedColorGraph.PrimaryColor = RGB(0, 128, 128);
        
        BuyMinColorGraph.Name = "Buy Min Color";
        BuyMinColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        BuyMinColorGraph.PrimaryColor = RGB(0, 64, 64); 
        
        SellMaxColorGraph.Name = "Sell Max Color";
        SellMaxColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMaxColorGraph.PrimaryColor = RGB(255, 181, 218);
        
        SellMedColorGraph.Name = "Sell Med Color";
        SellMedColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMedColorGraph.PrimaryColor = RGB(255, 0, 0);
        
        SellMinColorGraph.Name = "Sell Min Color";
        SellMinColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        SellMinColorGraph.PrimaryColor = RGB(128, 0, 0); 
        
        NeutralUpColorGraph.Name = "Neutral Up Color";
        NeutralUpColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NeutralUpColorGraph.PrimaryColor = RGB(50, 54, 69);
        
        NeutralDownColorGraph.Name = "Neutral Down Color";
        NeutralDownColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NeutralDownColorGraph.PrimaryColor = RGB(67, 73, 84);
        
        PositiveDeltaColorGraph.Name = "Positive Delta Color";
        PositiveDeltaColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        PositiveDeltaColorGraph.PrimaryColor = RGB(120, 200, 50);
        
        NegativeDeltaColorGraph.Name = "Negative Delta Color";
        NegativeDeltaColorGraph.DrawStyle = DRAWSTYLE_HIDDEN;
        NegativeDeltaColorGraph.PrimaryColor = RGB(130, 50, 200);
		
		    RSIPeriod.Name = "RSI Period";
        RSIPeriod.SetInt(24);
        RSIPeriod.SetIntLimits(1, 100);
        RSIPeriod.SetDescription("Period for RSI calculation");
        
        // Set graph type to OHLC Bar
        sc.GraphDrawType = GDT_OHLCBAR;
        
        // Set to use data from main price graph
        sc.ValueFormat = VALUEFORMAT_INHERITED;
        
        return;
    }
    
    // Copy price data to our subgraphs
    OpenGraph[sc.Index] = sc.Open[sc.Index];
    HighGraph[sc.Index] = sc.High[sc.Index];
    LowGraph[sc.Index] = sc.Low[sc.Index];
    CloseGraph[sc.Index] = sc.Close[sc.Index];
    
    // Default color setup - use neutral colors based on bar direction
    bool isUpBar = sc.Close[sc.Index] >= sc.Open[sc.Index];
    COLORREF barColor;
    
    if (isUpBar)
    {
        barColor = NeutralUpColorGraph.PrimaryColor;
    }
    else
    {
        barColor = NeutralDownColorGraph.PrimaryColor;
    }
    
    // Get input values for delta thresholds
    float posThreshold = PositiveDeltaThreshold.GetFloat();
    float negThreshold = NegativeDeltaThreshold.GetFloat();
    bool useAbsValue = UseAbsoluteValue.GetYesNo();
    
    // If using absolute value, adjust the negative threshold
    if (useAbsValue)
    {
        negThreshold = -fabs(posThreshold);
    }
    
    // Calculate delta (ask volume minus bid volume)
    float currentDelta = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];
    
    // Flag to track if delta coloring should be applied
    bool applyDeltaColor = false;
    COLORREF deltaColor = barColor;
    
    // Check if delta exceeds thresholds
    if (currentDelta >= posThreshold)
    {
        // Positive delta above threshold
        applyDeltaColor = true;
        deltaColor = PositiveDeltaColorGraph.PrimaryColor;
    }
    else if (currentDelta <= negThreshold)
    {
        // Negative delta below threshold
        applyDeltaColor = true;
        deltaColor = NegativeDeltaColorGraph.PrimaryColor;
    }
    
    // Flag to track if RSI coloring should be applied
    bool applyRSIColor = false;
    COLORREF rsiColor = barColor;
    
    // Apply RSI-based coloring
    // Get CVD study ID
    int cvdStudyID = CVDStudyID.GetStudyID();
    if (cvdStudyID > 0)
    {
        // Get CVD values from referenced study
        SCFloatArray cvdArray;
        sc.GetStudyArrayUsingID(cvdStudyID, 0, cvdArray);
        
        if (cvdArray.GetArraySize() > sc.Index)
        {
            // Calculate RSI on CVD data
            sc.RSI(cvdArray, RSIGraph, MOVAVGTYPE_EXPONENTIAL, RSIPeriod.GetInt());
            float currentRSI = RSIGraph[sc.Index];
            
            // Define static thresholds
            const float BUY_MAX_THRESHOLD = 80.0f;
            const float BUY_MED_THRESHOLD = 75.0f;
            const float BUY_MIN_THRESHOLD = 70.0f;
            const float SELL_MIN_THRESHOLD = 30.0f;
            const float SELL_MED_THRESHOLD = 25.0f;
            const float SELL_MAX_THRESHOLD = 20.0f;
            
            // For buy conditions (RSI above thresholds)
            if (currentRSI >= BUY_MAX_THRESHOLD)
            {
                // Max buy signal (RSI >= 80)
                applyRSIColor = true;
                rsiColor = BuyMaxColorGraph.PrimaryColor;
            }
            else if (currentRSI >= BUY_MED_THRESHOLD)
            {
                // Medium buy signal (RSI >= 75)
                applyRSIColor = true;
                rsiColor = BuyMedColorGraph.PrimaryColor;
            }
            else if (currentRSI >= BUY_MIN_THRESHOLD)
            {
                // Minimum buy signal (RSI >= 70)
                applyRSIColor = true;
                rsiColor = BuyMinColorGraph.PrimaryColor;
            }
            // For sell conditions (RSI below thresholds)
            else if (currentRSI <= SELL_MAX_THRESHOLD)
            {
                // Max sell signal (RSI <= 20)
                applyRSIColor = true;
                rsiColor = SellMaxColorGraph.PrimaryColor;
            }
            else if (currentRSI <= SELL_MED_THRESHOLD)
            {
                // Medium sell signal (RSI <= 25)
                applyRSIColor = true;
                rsiColor = SellMedColorGraph.PrimaryColor;
            }
            else if (currentRSI <= SELL_MIN_THRESHOLD)
            {
                // Minimum sell signal (RSI <= 30)
                applyRSIColor = true;
                rsiColor = SellMinColorGraph.PrimaryColor;
            }
        }
        else if (sc.Index == 0)
        {
            sc.AddMessageToLog("CVD Study data not available", 1);
        }
    }
    else if (sc.Index == 0)
    {
        sc.AddMessageToLog("Please set a valid CVD Study ID for RSI coloring", 1);
    }
    
    // Determine final color - always prioritize delta coloring
    if (applyDeltaColor)
    {
        barColor = deltaColor;
    }
    else if (applyRSIColor)
    {
        barColor = rsiColor;
    }
    
    // Set the color for all data points
    OpenGraph.DataColor[sc.Index] = barColor;
    HighGraph.DataColor[sc.Index] = barColor;
    LowGraph.DataColor[sc.Index] = barColor;
    CloseGraph.DataColor[sc.Index] = barColor;
}
>>>>>>> 60eac553924efc82c54861676ce3b365acc3bcc0
