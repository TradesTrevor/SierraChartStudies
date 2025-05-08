#include "sierrachart.h"

SCDLLName("DiscordAlerts")

void SendDiscordAlert(SCStudyInterfaceRef sc, const char* message) {
    SCString webhookUrl = sc.GetPersistentSCString(0);
    if (webhookUrl.GetLength() == 0) {
        sc.AddMessageToLog("Error: Discord webhook URL not set.", 1);
        return;
    }

    // Format message for Discord - replace newlines with \\n for JSON
    SCString formattedMessage;
    for (int i = 0; message[i] != '\0'; ++i) {
        if (message[i] == '\n') {
            formattedMessage += "\\n";
        } else {
            formattedMessage += message[i];
        }
    }

    // Prepare JSON payload - properly escape all special characters
    SCString jsonPayload;
    jsonPayload.Format("{\"content\": \"%s\"}", formattedMessage.GetChars());
    
    // Minimize logging to reduce overhead
    sc.AddMessageToLog("Sending Discord alert...", 0);

    n_ACSIL::s_HTTPHeader header;
    header.Name = "Content-Type";
    header.Value = "application/json";
    
    int requestId = sc.MakeHTTPPOSTRequest(
        webhookUrl,
        jsonPayload,
        &header,
        1 
    );

    if (requestId < 0) {
        SCString errorMessage;
        errorMessage.Format("Error sending Discord alert. Request ID: %d", requestId);
        sc.AddMessageToLog(errorMessage, 1);
    }
}

SCSFExport scsf_DiscordTradeAlert(SCStudyInterfaceRef sc) {
    SCFloatArrayRef LastPositionQuantity = sc.Subgraph[0];
    SCString webhookUrl;

    SCInputRef Input_WebhookUrl = sc.Input[0];

    if (sc.SetDefaults) {
        sc.GraphName = "Discord Trade Alert";
        sc.AutoLoop = 1;
        sc.FreeDLL = 0;
        sc.GraphRegion = 0;

        Input_WebhookUrl.Name = "Discord Webhook URL";
        Input_WebhookUrl.SetString("");

        sc.Subgraph[0].Name = "Last Position Quantity";
        sc.Subgraph[0].DrawStyle = DRAWSTYLE_IGNORE;

        return;
    }

    // Update webhook URL from input
    SCString inputUrl = Input_WebhookUrl.GetString();
    if (inputUrl.GetLength() > 0) {
        webhookUrl = inputUrl;
        sc.SetPersistentSCString(0, webhookUrl);
    } else {
        webhookUrl = sc.GetPersistentSCString(0);
    }

    // Get current position data
    s_SCPositionData position;
    sc.GetTradePosition(position);
    float currentQuantity = position.PositionQuantity;

    int& PriorOrderFillEntrySize = sc.GetPersistentInt(1);
    int& LastPositionDirection = sc.GetPersistentInt(2);

    int CurrentOrderFillEntrySize = sc.GetOrderFillArraySize();

    // Determine current position direction
    int currentPositionDirection = 0; // flat
    if (currentQuantity > 0) {
        currentPositionDirection = 1; // long
    } else if (currentQuantity < 0) {
        currentPositionDirection = -1; // short
    }

    if (CurrentOrderFillEntrySize != PriorOrderFillEntrySize)
    {
      PriorOrderFillEntrySize = CurrentOrderFillEntrySize;
      if (CurrentOrderFillEntrySize > 0)
      {
        s_SCOrderFillData OrderFillData;
        sc.GetOrderFillEntry(CurrentOrderFillEntrySize - 1, OrderFillData);
        
        // Determine if this order is establishing a new position or flattening
        bool isNewPosition = false;
        
        // If we were flat and now we're not, it's a new position
        if (LastPositionDirection == 0 && currentPositionDirection != 0) {
            isNewPosition = true;
        } 
        // If direction changed from long to short or vice versa, it's a new position
        else if ((LastPositionDirection == 1 && currentPositionDirection == -1) ||
                 (LastPositionDirection == -1 && currentPositionDirection == 1)) {
            isNewPosition = true;
        }
        // If going from a position to flat, it's not a new position (it's flattening)
        else if (LastPositionDirection != 0 && currentPositionDirection == 0) {
            isNewPosition = false;
        }
        
        SCString positionStatusMsg;
        positionStatusMsg.Format("Position Status: Previous=%d, Current=%d, IsNewPosition=%d", 
                                LastPositionDirection, currentPositionDirection, isNewPosition);
        sc.AddMessageToLog(positionStatusMsg, 0);
        
        // Only process alerts for new positions
        if (isNewPosition) {
            s_SCTradeOrder TradeOrder;
            int TradeOrderResult = sc.GetOrderByOrderID(OrderFillData.InternalOrderID, TradeOrder);

            if (TradeOrderResult != SCTRADING_ORDER_ERROR)
            {
              // Get Stop Order details if available
              s_SCTradeOrder OrderDetails;
              int StopOrderResult = sc.GetNearestStopWorkingAttachedOrder(OrderDetails);
              float stopPrice = 0.0;
              if (StopOrderResult != SCTRADING_ORDER_ERROR) {
                stopPrice = OrderDetails.Price1;
              }

              // Get Target Order details if available
              s_SCTradeOrder TargetOrderDetails;
              int TargetOrderResult = sc.GetNearestTargetWorkingAttachedOrder(TargetOrderDetails);
              float targetPrice = 0.0;
              if (TargetOrderResult != SCTRADING_ORDER_ERROR) {
                targetPrice = TargetOrderDetails.Price1;
              }
              
              const char* positionType = (OrderFillData.BuySell == 1) ? "Long" : "Short";

              SCString positionTypeString;
              positionTypeString.Format("Position Type: %i", OrderFillData.BuySell);
              sc.AddMessageToLog(positionTypeString, 0);
              
              // Format Discord alert message
              SCString alertMessage;
              alertMessage.Format("%s: %.2f\nInitial Target: %.2f\nStop Loss: %.2f", 
                                 positionType, 
                                 OrderFillData.FillPrice, 
                                 targetPrice, 
                                 stopPrice);

              SendDiscordAlert(sc, alertMessage);
            }
        } else {
            sc.AddMessageToLog("Order detected as flattening or adjusting position - not sending alert", 0);
        }
      }
    }
    
    LastPositionDirection = currentPositionDirection;

    // Update last position quantity
    LastPositionQuantity[sc.Index] = currentQuantity;
}
