#include "sierrachart.h"

SCDLLName("Initial Balance Statistics")

SCSFExport scsf_InitialBalanceStatistics(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_IBHExt3  = sc.Subgraph[0];
	SCSubgraphRef Subgraph_IBHExt2  = sc.Subgraph[1];
	SCSubgraphRef Subgraph_IBHExt1  = sc.Subgraph[2];
	SCSubgraphRef Subgraph_IBHigh   = sc.Subgraph[3];
	SCSubgraphRef Subgraph_IBMid    = sc.Subgraph[4];
	SCSubgraphRef Subgraph_IBLow    = sc.Subgraph[5];
	SCSubgraphRef Subgraph_IBLExt1  = sc.Subgraph[6];
	SCSubgraphRef Subgraph_IBLExt2  = sc.Subgraph[7];
	SCSubgraphRef Subgraph_IBLExt3  = sc.Subgraph[8];

	SCInputRef Input_Instrument = sc.Input[0];
	SCInputRef Input_IBType      = sc.Input[1];
	SCInputRef Input_StartTime   = sc.Input[2];
	SCInputRef Input_EndTime     = sc.Input[3];
	SCInputRef Input_NumDays     = sc.Input[4];
	SCInputRef Input_RoundExt    = sc.Input[5];
	SCInputRef Input_NumberDaysToCalculate = sc.Input[6];
	SCInputRef Input_NumberOfMinutes      = sc.Input[7];
	SCInputRef Input_StartEndTimeMethod = sc.Input[8];
	SCInputRef Input_PeriodEndAsMinutesFromSessionStart = sc.Input[9];

	SCInputRef Input_Multiplier1 = sc.Input[10];
	SCInputRef Input_Multiplier2 = sc.Input[11];
	SCInputRef Input_Multiplier3 = sc.Input[12];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Initial Balance";
		sc.DrawZeros = 0;
		sc.GraphRegion = 0;
		sc.AutoLoop	= 1;

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		Subgraph_IBHExt3.Name = "IB High 100% Ext";
		Subgraph_IBHExt3.PrimaryColor = RGB(0, 255, 0);
		Subgraph_IBHExt3.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBHExt3.DrawZeros = false;

		Subgraph_IBHExt2.Name = "IB High 50% Ext";
		Subgraph_IBHExt2.PrimaryColor = RGB(0, 255, 0);
		Subgraph_IBHExt2.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBHExt2.DrawZeros = false;

		Subgraph_IBHExt1.Name = "IB High 25% Ext";
		Subgraph_IBHExt1.PrimaryColor = RGB(0, 255, 0);
		Subgraph_IBHExt1.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBHExt1.DrawZeros = false;

		Subgraph_IBHigh.Name = "IB High";
		Subgraph_IBHigh.PrimaryColor = RGB(128, 255, 128);
		Subgraph_IBHigh.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_IBHigh.DrawZeros = false;
		Subgraph_IBHigh.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;

		Subgraph_IBMid.Name = "IB Mid";
		Subgraph_IBMid.PrimaryColor = RGB(255, 255, 255);
		Subgraph_IBMid.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_IBMid.DrawZeros = false;
		Subgraph_IBMid.LineLabel = 	LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;

		Subgraph_IBLow.Name = "IB Low";
		Subgraph_IBLow.PrimaryColor = RGB(255, 128, 128);
		Subgraph_IBLow.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_IBLow.DrawZeros = false;
		Subgraph_IBLow.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;

		Subgraph_IBLExt1.Name = "IB Low 25% Ext";
		Subgraph_IBLExt1.PrimaryColor = RGB(255, 0, 0);
		Subgraph_IBLExt1.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt1.DrawZeros = false;

		Subgraph_IBLExt2.Name = "IB Low 50% Ext";
		Subgraph_IBLExt2.PrimaryColor = RGB(255, 0, 0);
		Subgraph_IBLExt2.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt2.DrawZeros = false;

		Subgraph_IBLExt3.Name = "IB Low 100% Ext";
		Subgraph_IBLExt3.PrimaryColor = RGB(255, 0, 0);
		Subgraph_IBLExt3.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt3.DrawZeros = false;

		// Inputs
		Input_Instrument.Name = "Instrument Selection";
		Input_Instrument.SetCustomInputStrings("NQ;ES");
		Input_Instrument.SetCustomInputIndex(0);

		Input_IBType.Name = "Initial Balance Type";
		Input_IBType.SetCustomInputStrings("Daily;Weekly;Weekly Include Sunday;Intraday");
		Input_IBType.SetCustomInputIndex(0);

		Input_StartTime.Name = "Start Time";
		Input_StartTime.SetTime(HMS_TIME(9, 30, 0));
		
		Input_EndTime.Name = "End Time";
		Input_EndTime.SetTime(HMS_TIME(10, 29, 59));

		Input_NumDays.Name = "Weekly: Number of Days";
		Input_NumDays.SetInt(2);
		Input_NumDays.SetIntLimits(1, 5);

		Input_NumberOfMinutes.Name = "Intraday: Number of Minutes";
		Input_NumberOfMinutes.SetInt(15);

		Input_RoundExt.Name = "Round Extensions to TickSize";
		Input_RoundExt.SetYesNo(1);

		Input_NumberDaysToCalculate.Name = "Number of Days to Calculate";
		Input_NumberDaysToCalculate.SetInt(100000);
		Input_NumberDaysToCalculate.SetIntLimits(1,INT_MAX);

		Input_StartEndTimeMethod.Name = "Start End Time Method";
		Input_StartEndTimeMethod.SetCustomInputStrings("Use Start/End Time;Use Session Start Time and Minutes From Start");
		Input_StartEndTimeMethod.SetCustomInputIndex(0);

		Input_PeriodEndAsMinutesFromSessionStart.Name = "Period End As Minutes from Session Start";
		Input_PeriodEndAsMinutesFromSessionStart.SetInt(30);

		Input_Multiplier1.Name = "Extension Multiplier 1";
		Input_Multiplier1.SetFloat(0.25f);
		Input_Multiplier2.Name = "Extension Multiplier 2";
		Input_Multiplier2.SetFloat(0.5f);
		Input_Multiplier3.Name = "Extension Multiplier 3";
		Input_Multiplier3.SetFloat(1.0f);

		return;
	}

	// Persist vars
	int& PeriodFirstIndex = sc.GetPersistentInt(1);
	int& NamesUpdated = sc.GetPersistentInt(2);
	int& IBHighBreached = sc.GetPersistentInt(3);
	int& IBLowBreached = sc.GetPersistentInt(4);
	int& ExtensionNamesUpdated = sc.GetPersistentInt(5);
	
	SCDateTime& PeriodStartDateTime = sc.GetPersistentSCDateTime(1);
	SCDateTime& PeriodEndDateTime   = sc.GetPersistentSCDateTime(2);

	float& PeriodHigh       = sc.GetPersistentFloat(1);
	float& PeriodLow        = sc.GetPersistentFloat(2);
	float& PeriodMid        = sc.GetPersistentFloat(3);
	float& PeriodHighExt1   = sc.GetPersistentFloat(4);
	float& PeriodHighExt2   = sc.GetPersistentFloat(5);
	float& PeriodHighExt3   = sc.GetPersistentFloat(6);
	float& PeriodHighExt4   = sc.GetPersistentFloat(7);
	float& PeriodHighExt5   = sc.GetPersistentFloat(8);
	float& PeriodHighExt6   = sc.GetPersistentFloat(9);
	float& PeriodLowExt1    = sc.GetPersistentFloat(10);
	float& PeriodLowExt2    = sc.GetPersistentFloat(11);
	float& PeriodLowExt3    = sc.GetPersistentFloat(12);
	float& PeriodLowExt4    = sc.GetPersistentFloat(13);
	float& PeriodLowExt5    = sc.GetPersistentFloat(14);
	float& PeriodLowExt6    = sc.GetPersistentFloat(15);

	// Reset persistent variables upon full calculation
	if (sc.Index == 0)
	{
		PeriodFirstIndex = -1;
		NamesUpdated = 0;
		IBHighBreached = 0;
		IBLowBreached = 0;
		ExtensionNamesUpdated = 0;
		PeriodStartDateTime = 0;
		PeriodEndDateTime   = 0;
		PeriodHigh = -FLT_MAX;
		PeriodLow  =  FLT_MAX;
	}

	SCDateTimeMS LastBarDateTime = sc.BaseDateTimeIn[sc.ArraySize-1];
	SCDateTimeMS FirstCalculationDate = LastBarDateTime.GetDate() - SCDateTime::DAYS(Input_NumberDaysToCalculate.GetInt() - 1);
	SCDateTimeMS CurrentBarDateTime = sc.BaseDateTimeIn[sc.Index];
	SCDateTimeMS PrevBarDateTime;

	if (sc.Index > 0)
		PrevBarDateTime = sc.BaseDateTimeIn[sc.Index-1];

	if (CurrentBarDateTime.GetDate() < FirstCalculationDate) // Limit calculation to specified number of days back
		return;

	bool Daily  = Input_IBType.GetIndex() == 0;
	bool Weekly = Input_IBType.GetIndex() == 1 || Input_IBType.GetIndex() == 2;
	bool Intraday = Input_IBType.GetIndex() == 3;
	bool IncludeSunday = Input_IBType.GetIndex() == 2;


	SCDateTimeMS StartDateTime = CurrentBarDateTime;

	if (Input_StartEndTimeMethod.GetIndex() == 0)
		StartDateTime.SetTime(Input_StartTime.GetTime());
	else
		StartDateTime.SetTime(sc.StartTimeOfDay);

	if (Weekly)
	{
		int PeriodStartDayOfWeek = IncludeSunday ? SUNDAY : MONDAY;

		int DayOfWeek = StartDateTime.GetDayOfWeek();

		if (DayOfWeek != PeriodStartDayOfWeek)
			StartDateTime.AddDays(7 - DayOfWeek + PeriodStartDayOfWeek);
	}
		
	if (PrevBarDateTime < StartDateTime && CurrentBarDateTime >= StartDateTime)
	{
		PeriodFirstIndex = sc.Index;
		PeriodHigh = -FLT_MAX;
		PeriodLow  = FLT_MAX;

		PeriodStartDateTime = StartDateTime;

		PeriodEndDateTime = PeriodStartDateTime;

		if (Input_StartEndTimeMethod.GetIndex() == 0)
			PeriodEndDateTime.SetTime(Input_EndTime.GetTime());
		else
		{
			PeriodEndDateTime.SetTime(static_cast<int>(sc.StartTimeOfDay + Input_PeriodEndAsMinutesFromSessionStart.GetInt() * SECONDS_PER_MINUTE - 1));
		}

		if (Daily || Intraday)
		{
			if (SCDateTimeMS(PeriodEndDateTime) <= PeriodStartDateTime)
				PeriodEndDateTime.AddDays(1);
		}
		else if (Weekly)
		{
			int PeriodEndDayOfWeek = IncludeSunday ? Input_NumDays.GetInt() - 1 : Input_NumDays.GetInt();

			int DayOfWeek = PeriodEndDateTime.GetDayOfWeek();

			if (DayOfWeek != PeriodEndDayOfWeek)
				PeriodEndDateTime.AddDays(PeriodEndDayOfWeek - DayOfWeek);
		}
	}

	// Check end of period
	if (PeriodFirstIndex >= 0)
	{
		// Check start of new intraday period
		if (Intraday)
		{
			SCDateTimeMS IntradayEndDateTime = PeriodStartDateTime + SCDateTime::MINUTES(Input_NumberOfMinutes.GetInt()) - SCDateTime::MICROSECONDS(1);

			if (PrevBarDateTime < IntradayEndDateTime && CurrentBarDateTime > IntradayEndDateTime)
			{
				PeriodFirstIndex = sc.Index;
				PeriodStartDateTime.AddMinutes(Input_NumberOfMinutes.GetInt());
				PeriodHigh = -FLT_MAX;
				PeriodLow  = FLT_MAX;
			}
		}

		if (CurrentBarDateTime > PeriodEndDateTime)
		{
			PeriodFirstIndex = -1;

			if (Intraday)
			{
				PeriodHigh = -FLT_MAX;
				PeriodLow  = FLT_MAX;
			}
		}
	}

	// Collecting data, back propagate if changed
	if (PeriodFirstIndex >= 0)
	{
		bool Changed = false;

		if (sc.High[sc.Index] > PeriodHigh)
		{
			PeriodHigh = sc.High[sc.Index];
			Changed = true;
		}

		if (sc.Low[sc.Index] < PeriodLow)
		{
			PeriodLow = sc.Low[sc.Index];
			Changed = true;
		}

		if (Changed)
		{
			PeriodMid = (PeriodHigh + PeriodLow) / 2.0f;

			float Range = PeriodHigh - PeriodLow;

			PeriodHighExt1 = PeriodHigh + Input_Multiplier1.GetFloat() * Range; 
			PeriodHighExt2 = PeriodHigh + Input_Multiplier2.GetFloat() * Range; 
			PeriodHighExt3 = PeriodHigh + Input_Multiplier3.GetFloat() * Range;  

			PeriodLowExt1 = PeriodLow - Input_Multiplier1.GetFloat() * Range; 
			PeriodLowExt2 = PeriodLow - Input_Multiplier2.GetFloat() * Range; 
			PeriodLowExt3 = PeriodLow - Input_Multiplier3.GetFloat() * Range; 

			if (Input_RoundExt.GetYesNo())
			{
				PeriodHighExt1 = sc.RoundToTickSize(PeriodHighExt1, sc.TickSize); 
				PeriodHighExt2 = sc.RoundToTickSize(PeriodHighExt2, sc.TickSize); 
				PeriodHighExt3 = sc.RoundToTickSize(PeriodHighExt3, sc.TickSize); 

				PeriodLowExt1 = sc.RoundToTickSize(PeriodLowExt1, sc.TickSize); 
				PeriodLowExt2 = sc.RoundToTickSize(PeriodLowExt2, sc.TickSize); 
				PeriodLowExt3 = sc.RoundToTickSize(PeriodLowExt3, sc.TickSize); 
			}

			for (int Index = PeriodFirstIndex; Index < sc.Index; Index++)
			{
				Subgraph_IBHigh[Index]  = PeriodHigh;
				Subgraph_IBLow[Index]   = PeriodLow;
				Subgraph_IBMid[Index]   = PeriodMid;
				Subgraph_IBHExt1[Index] = PeriodHighExt1;
				Subgraph_IBHExt2[Index] = PeriodHighExt2;
				Subgraph_IBHExt3[Index] = PeriodHighExt3;
				Subgraph_IBLExt1[Index] = PeriodLowExt1;
				Subgraph_IBLExt2[Index] = PeriodLowExt2;
				Subgraph_IBLExt3[Index] = PeriodLowExt3;
			}

			sc.EarliestUpdateSubgraphDataArrayIndex = PeriodFirstIndex;
		}
	}

	// Plot current values
	if (PeriodLow != FLT_MAX)
	{
		Subgraph_IBHigh[sc.Index]  = PeriodHigh;
		Subgraph_IBLow[sc.Index]   = PeriodLow;
		Subgraph_IBMid[sc.Index]   = PeriodMid;
		Subgraph_IBHExt1[sc.Index] = PeriodHighExt1;
		Subgraph_IBHExt2[sc.Index] = PeriodHighExt2;
		Subgraph_IBHExt3[sc.Index] = PeriodHighExt3;
		Subgraph_IBLExt1[sc.Index] = PeriodLowExt1;
		Subgraph_IBLExt2[sc.Index] = PeriodLowExt2;
		Subgraph_IBLExt3[sc.Index] = PeriodLowExt3;
	}

	// Check for 10:30 EST and update subgraph names based on close vs IBMid
	// This logic dynamically updates subgraph names with probability statistics
	// based on historical analysis of price action relative to Initial Balance
	if (!NamesUpdated && PeriodMid != 0.0f && PeriodHigh != -FLT_MAX && PeriodLow != FLT_MAX)
	{
		// Check if this is a 1-minute timeframe and if current time is 10:30 EST
		if (sc.SecondsPerBar == 60) // 1-minute bars
		{
			// Create target time for 10:30 EST
			// Note: Assuming chart time is already in EST/EDT (exchange time)
			// If your chart uses UTC, you may need to adjust for timezone offset
			SCDateTime TargetTime;
			TargetTime.SetDateTime(CurrentBarDateTime.GetDate(), HMS_TIME(10, 30, 0));
			
			// Check if we're at exactly 10:30 EST (within the 1-minute window)
			if (CurrentBarDateTime >= TargetTime && CurrentBarDateTime < TargetTime + SCDateTime::MINUTES(1))
			{
				// Get the close price of the current 1-minute candle
				float CandleClose = sc.Close[sc.Index];
				
				// Get instrument index (0 = NQ, 1 = ES)
				int InstrumentIndex = Input_Instrument.GetIndex(); // 0 = NQ, 1 = ES
				
				if (CandleClose < PeriodMid)
				{
					// Close below IBMid
					if (InstrumentIndex == 0) // NQ selected
					{
						Subgraph_IBHigh.Name = "IB High (40.4%)";
						Subgraph_IBLow.Name = "IB Low (78.2%)";
					}
					else // ES selected (index == 1)
					{
						Subgraph_IBHigh.Name = "IB High (47.1%)";
						Subgraph_IBLow.Name = "IB Low (94.9%)";
					}
				}
				else
				{
					// Close above IBMid
					if (InstrumentIndex == 0)
					{
						Subgraph_IBHigh.Name = "IB High (83.3%)";
						Subgraph_IBLow.Name = "IB Low (37.6%)";
					}
					else
					{
						Subgraph_IBHigh.Name = "IB High (83.5%)";
						Subgraph_IBLow.Name = "IB Low (54.0%)";
					}
				}
				
				// Mark names as updated to prevent repeated updates
				NamesUpdated = 1;
				
				// Force chart redraw to reflect name changes
				sc.UpdateAlways = 1;
			}
		}
	}

	// Check for IB High/Low breaches and update extension names
	if (!ExtensionNamesUpdated && PeriodHigh != -FLT_MAX && PeriodLow != FLT_MAX)
	{
		// Check if IB High has been breached
		if (!IBHighBreached && sc.High[sc.Index] > PeriodHigh)
		{
			IBHighBreached = 1;
			
			// Get instrument index (0 = NQ, 1 = ES)
			int InstrumentIndex = Input_Instrument.GetIndex(); // 0 = NQ, 1 = ES

			if (InstrumentIndex == 0) {
				// Update NQ IB extension names
				Subgraph_IBHExt1.Name = "IB High 25% Ext (75.2%)";
				Subgraph_IBHExt1.DrawStyle = DRAWSTYLE_DASH;
						Subgraph_IBHExt1.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBHExt2.Name = "IB High 50% Ext (53.2%)";
				Subgraph_IBHExt2.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBHExt2.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBHExt3.Name = "IB High 100% Ext (22.3%)";
				Subgraph_IBHExt3.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBHExt3.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;


				// Update IB Low percentage for both breached stat
				Subgraph_IBLow.Name = "IB Low (23.6%)";
			}
			else {
				// Update ES IB extension names
				Subgraph_IBHExt1.Name = "IB High 25% Ext (85.3%)";
				Subgraph_IBHExt1.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBHExt1.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBHExt2.Name = "IB High 50% Ext (69.5%)";
				Subgraph_IBHExt2.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBHExt2.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBHExt3.Name = "IB High 100% Ext (44.5%)";
				Subgraph_IBHExt3.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBHExt3.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;

				// Update IB Low percentage for both breached stat
				Subgraph_IBLow.Name = "IB Low (38.3%)";
			}
			
			// If both IB High and Low have been breached, mark as complete
			if (IBLowBreached)
			{
				ExtensionNamesUpdated = 1;
			}
			
			// Force chart redraw to reflect name changes
			sc.UpdateAlways = 1;
		}
		
		// Check if IB Low has been breached
		if (!IBLowBreached && sc.Low[sc.Index] < PeriodLow)
		{
			IBLowBreached = 1;
			
			// Get instrument index (0 = NQ, 1 = ES)
			int InstrumentIndex = Input_Instrument.GetIndex(); // 0 = NQ, 1 = ES
			
			if (InstrumentIndex == 0) {
				// Update NQ IB extension names
				Subgraph_IBLExt1.Name = "IB Low 25% Ext (75.5%)";
				Subgraph_IBLExt1.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt1.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBLExt2.Name = "IB Low 50% Ext (56.6%)";
				Subgraph_IBLExt2.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt2.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBLExt3.Name = "IB Low 100% Ext (29.8%)";
				Subgraph_IBLExt3.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt3.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;


				// Update IB High percentage for both breached stat
				Subgraph_IBHigh.Name = "IB High (23.4%)";
			}
			else {
				// Update ES IB extension names
				Subgraph_IBLExt1.Name = "IB Low 25% Ext (91.1%)";
				Subgraph_IBLExt1.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt1.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBLExt2.Name = "IB Low 50% Ext (82.6%)";
				Subgraph_IBLExt2.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt2.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
				Subgraph_IBLExt3.Name = "IB Low 100% Ext (67.7%)";
				Subgraph_IBLExt3.DrawStyle = DRAWSTYLE_DASH;
				Subgraph_IBLExt3.LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;

				// Update IB High percentage for both breached stat
				Subgraph_IBHigh.Name = "IB High (42.3%)";
			}
			
			// If both IB High and Low have been breached, mark as complete
			if (IBHighBreached)
			{
				ExtensionNamesUpdated = 1;
			}
			
			// Force chart redraw to reflect name changes
			sc.UpdateAlways = 1;
		}
	}

	// Reset flags and names at start of new trading day
	if (PeriodFirstIndex >= 0 && sc.Index == PeriodFirstIndex)
	{
		NamesUpdated = 0;
		IBHighBreached = 0;
		IBLowBreached = 0;
		ExtensionNamesUpdated = 0;
		// Reset to default names at start of new period
		Subgraph_IBHigh.Name = "IB High";
		Subgraph_IBLow.Name = "IB Low";
		Subgraph_IBHExt1.Name = "IB High 25% Ext";
		Subgraph_IBHExt1.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBHExt2.Name = "IB High 50% Ext";
		Subgraph_IBHExt2.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBHExt3.Name = "IB High 100% Ext";
		Subgraph_IBHExt3.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt1.Name = "IB Low 25% Ext";
		Subgraph_IBLExt1.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt2.Name = "IB Low 50% Ext";
		Subgraph_IBLExt2.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_IBLExt3.Name = "IB Low 100% Ext";
		Subgraph_IBLExt3.DrawStyle = DRAWSTYLE_IGNORE;
	}

	
}