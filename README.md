# FlipperStudies

## **The Flipper**

- Detects delta flips (changes in buying/selling pressure) with customizable thresholds.
- Supports strong flip detection based on a multiplier of the delta threshold.
- Optional next-bar confirmation to reduce false signals.
- Visual markers for bearish and bullish flips on price highs/lows.

## **Delta Colored Candles**

- Colors candlesticks based on positive/negative delta thresholds.
- Integrates RSI calculations on Cumulative Volume Delta (CVD) for additional coloring logic.
- Supports absolute value thresholds for symmetric delta analysis.
- Customizable neutral, buy, and sell colors for clear visualization.

## **Discord Alerts**

- Sends trade alerts to Discord channels via webhooks when new positions are established.
- Automatically detects position entries (long/short) while filtering out adjustments and exits.
- Includes critical trade information: position type, fill price, target price, and stop loss.
- Configurable webhook URL for easy integration with any Discord server.
- Proper JSON formatting with character escaping for reliable message delivery.
