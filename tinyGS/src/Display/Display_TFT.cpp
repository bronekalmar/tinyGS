#include "Display_TFT.h"
#include <SPI.h>

#ifdef HELTEC_TRACKER_V1_1
#endif

#ifndef TFT_MESH
#define TFT_MESH COLOR565(0x67, 0xEA, 0x94)
#endif

class LGFX : public lgfx::LGFX_Device {

lgfx::Panel_ST7735S     _panel_instance;
lgfx::Bus_SPI        _bus_instance;
lgfx::Light_PWM     _light_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

        cfg.spi_host = ST7735_SPI_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = SPI_FREQUENCY;
        cfg.freq_read = SPI_READ_FREQUENCY;
        cfg.spi_3wire = false;
        cfg.use_lock = true;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_sclk = ST7735_SCK;
        cfg.pin_mosi = ST7735_SDA;
        cfg.pin_miso = ST7735_MISO;
        cfg.pin_dc = ST7735_RS;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = ST7735_CS;
      cfg.pin_rst = ST7735_RESET;
      cfg.pin_busy = ST7735_BUSY;
      cfg.panel_width = TFT_WIDTH;
      cfg.panel_height = TFT_HEIGHT;
      cfg.offset_x = TFT_OFFSET_X;
      cfg.offset_y = TFT_OFFSET_Y;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = TFT_INVERT;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      cfg.memory_width = TFT_WIDTH;
      cfg.memory_height = TFT_HEIGHT;
      _panel_instance.config(cfg);
    }


    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = ST7735_BL_V05;
      cfg.invert = false;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

static LGFX *tft = nullptr;


TFTDisplay::TFTDisplay(uint8_t address, int sda, int scl)
{
    Serial.println(" -- TFTDisplay!");
    setGeometry(GEOMETRY_RAWMODE, TFT_HEIGHT, TFT_WIDTH);
}

// Write the buffer to the display memory
void TFTDisplay::display(bool fromBlank)
{
    if (fromBlank)
        tft->fillScreen(TFT_BLACK);
    // tft->clear();

    uint16_t x, y;

    for (y = 0; y < displayHeight; y++) {
        for (x = 0; x < displayWidth; x++) {
            auto isset = buffer[x + (y / 8) * displayWidth] & (1 << (y & 7));
            if (!fromBlank) {
                // get src pixel in the page based ordering the OLED lib uses FIXME, super inefficent
                auto dblbuf_isset = buffer_back[x + (y / 8) * displayWidth] & (1 << (y & 7));
                if (isset != dblbuf_isset) {
                    tft->drawPixel(x, y, isset ? TFT_MESH : TFT_BLACK);
                }
            } else if (isset) {
                tft->drawPixel(x, y, TFT_MESH);
            }
        }
    }
    // Copy the Buffer to the Back Buffer
    for (y = 0; y < (displayHeight / 8); y++) {
        for (x = 0; x < displayWidth; x++) {
            uint16_t pos = x + y * displayWidth;
            buffer_back[pos] = buffer[pos];
        }
    }
}

void TFTDisplay::sendCommand(uint8_t com)
{
    // handle display on/off directly
    switch (com) {
    case DISPLAYON: {
        pinMode(ST7735_BL_V05, OUTPUT);
        digitalWrite(ST7735_BL_V05, TFT_BACKLIGHT_ON);

        pinMode(ST7735_VTFT_CTRL_Pin, OUTPUT);
        digitalWrite(VEXT_ENABLE_V05, HIGH);

        tft->setBrightness(172);
        break;
    }
    case DISPLAYOFF: {
        pinMode(ST7735_BL_V05, OUTPUT);
        digitalWrite(ST7735_BL_V05, !TFT_BACKLIGHT_ON);

        tft->setBrightness(0);
        break;
    }
    default:
        break;
    }

    // Drop all other commands to device (we just update the buffer)
}

void TFTDisplay::flipScreenVertically()
{
    return;
}

bool TFTDisplay::hasTouch(void)
{
    return false;
}

bool TFTDisplay::getTouch(int16_t *x, int16_t *y)
{
    return false;
}

void TFTDisplay::setDetected(uint8_t detected)
{
    (void)detected;
}

// Connect to the display
bool TFTDisplay::connect()
{
    Serial.println(" -- Doing TFT init");
    tft = new LGFX;

    pinMode(ST7735_VTFT_CTRL_Pin, OUTPUT);
    digitalWrite(VEXT_ENABLE_V05, HIGH);
    
    pinMode(ST7735_BL_V05, OUTPUT);
    digitalWrite(ST7735_BL_V05, TFT_BACKLIGHT_ON);

    tft->init();
    tft->setRotation(3); // Orient horizontal and wide underneath the silkscreen name label
    tft->fillScreen(TFT_BLACK);

    return true;
}