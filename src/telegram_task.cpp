#include "telegram_task.h"
#include "config.h"
#include <WiFi.h>
#include <UniversalTelegramBot.h>

// --- WIFI & TELEGRAM CONFIG ---
const char *WIFI_SSID = "Your_WiFi_SSID";
const char *WIFI_PASSWORD = "Your_WiFi_Password";
const char *GAS_BOT_TOKEN = "Your_Bot_Token";
const char *GAS_CHAT_ID = "Your_Chat_ID";

UniversalTelegramBot gasBot(GAS_BOT_TOKEN, gasTelegramClient);

bool gasLastIsWarning = false;
const unsigned long GAS_TELEGRAM_POLL_MS = 2000;
const unsigned long GAS_WARNING_CHECK_MS = 1000;
unsigned long lastTelegramPoll = 0;
unsigned long lastGasCheck = 0;

void handleGasBotMessages(int numNewMessages)
{
    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = gasBot.messages[i].chat_id;
        String text = gasBot.messages[i].text;

        if (chat_id != String(GAS_CHAT_ID))
        {
            continue;
        }

        Serial.println("[GAS BOT] Command: " + text);

        if (text == "/start" || text == "/start_gas")
        {
            String msg = "Hi, I am Gas Warning Bot on ESP32.\n\n";
            msg += "Commands:\n";
            msg += "  /start_gas   - Show help\n";
            msg += "  /status_gas  - Show status\n";
            msg += "  /set_warn <ppm> - Set Warning Level\n";
            msg += "  /set_crit <ppm> - Set Critical Level\n";
            gasBot.sendMessage(chat_id, msg, "Markdown");
        }
        else if (text == "/status_gas")
        {
            String msg = "GAS STATUS:\n";
            msg += "  - Gas: " + String(gasPPM) + " ppm\n";
            msg += "  - Warn Thresh: " + String(warnThreshold) + " ppm\n";
            msg += "  - Crit Thresh: " + String(criticalThreshold) + " ppm\n";

            if (gasPPM >= criticalThreshold)
                msg += "  - STATE: CRITICAL ❗";
            else if (gasPPM >= warnThreshold)
                msg += "  - STATE: WARNING ⚠";
            else
                msg += "  - STATE: SAFE ✅";

            gasBot.sendMessage(chat_id, msg, "");
        }
        else if (text.startsWith("/set_warn"))
        {
            float val = text.substring(9).toFloat();
            if (val > 0)
            {
                warnThreshold = (int)val;

                preferences.begin("gas-config", false);
                preferences.putInt("warn", warnThreshold);
                preferences.end();

                gasBot.sendMessage(chat_id, "✅ Set warn threshold to " + String(warnThreshold) + " ppm", "");
            }
            else
            {
                gasBot.sendMessage(chat_id, "❌ Invalid value. Usage: /set_warn 5000", "");
            }
        }
        else if (text.startsWith("/set_crit"))
        {
            float val = text.substring(9).toFloat();
            if (val > 0)
            {
                criticalThreshold = (int)val;

                preferences.begin("gas-config", false);
                preferences.putInt("crit", criticalThreshold);
                preferences.end();

                gasBot.sendMessage(chat_id, "✅ Set critical threshold to " + String(criticalThreshold) + " ppm", "");
            }
            else
            {
                gasBot.sendMessage(chat_id, "❌ Invalid value. Usage: /set_crit 10000", "");
            }
        }
        else
        {
            gasBot.sendMessage(chat_id, "Unknown command. Try /start_gas", "");
        }
    }
}

void taskTelegram(void *parameter)
{
    Serial.println("Telegram Task Started on Core 0");

    if (WiFi.status() == WL_CONNECTED)
    {
        gasBot.sendMessage(GAS_CHAT_ID, "System Online via FreeRTOS!", "");
    }

    for (;;)
    {
        unsigned long now = millis();

        // 1. Wifi checking
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi Lost, Reconnecting...");
            WiFi.reconnect();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        // 2. Send warning
        if (now - lastGasCheck >= GAS_WARNING_CHECK_MS)
        {
            lastGasCheck = now;
            bool isWarning = (gasPPM >= warnThreshold);
            bool isCritical = (gasPPM >= criticalThreshold);

            if (isWarning && !gasLastIsWarning)
            {
                String msg = "⚠ *GAS WARNING!*\n PPM: " + String(gasPPM);
                if (isCritical)
                    msg += "\nStatus: CRITICAL ❗";
                gasBot.sendMessage(GAS_CHAT_ID, msg, "Markdown");
            }
            else if (!isWarning && gasLastIsWarning)
            {
                gasBot.sendMessage(GAS_CHAT_ID, "✅ Gas Safe. PPM: " + String(gasPPM), "");
            }

            gasLastIsWarning = isWarning;
        }

        // 3. Receive command from Telegram
        if (now - lastTelegramPoll >= GAS_TELEGRAM_POLL_MS)
        {
            int numNewMessages = gasBot.getUpdates(gasBot.last_message_received + 1);

            if (numNewMessages > 0)
            {
                handleGasBotMessages(numNewMessages);
            }

            lastTelegramPoll = now;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}