// ****************************************************************************
//  dmcp.cpp                                                      DB48X project
// ****************************************************************************
//
//   File Description:
//
//     A fake DMCP implementation with the functions we use in the simulator
//
//
//
//
//
//
//
//
// ****************************************************************************
//   (C) 2022 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the terms outlined in LICENSE.txt
// ****************************************************************************
//   This file is part of DB48X.
//
//   DB48X is free software: you can redistribute it and/or modify
//   it under the terms outlined in the LICENSE.txt file
//
//   DB48X is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// ****************************************************************************

#include "dmcp.h"

#include "dmcp_fonts.c"
#include "sim-rpl.h"
#include "sim-screen.h"
#include "sim-window.h"
#include "types.h"
#include "recorder.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <target.h>

#include <QFileDialog>
#include <QSettings>
#include <iostream>
#include <sys/stat.h>


#pragma GCC diagnostic ignored "-Wunused-parameter"

RECORDER(dmcp,          64, "DMCP system calls");
RECORDER(dmcp_error,    64, "DMCP errors");
RECORDER(dmcp_warning,  64, "DMCP warnings");
RECORDER(dmcp_notyet,   64, "DMCP features that are not yet implemented");
RECORDER(keys,          64, "DMCP key handling");
RECORDER(keys_empty,    64, "DMCP key_empty() call");
RECORDER(keys_warning,  64, "Warnings related to key handling");
RECORDER(lcd,           64, "DMCP lcd/display functions");
RECORDER(lcd_refresh,   64, "DMCP lcd/display refresh");
RECORDER(lcd_width,     64, "Width of strings and chars");
RECORDER(lcd_warning,   64, "Warnings from lcd/display functions");

#undef ppgm_fp

volatile int       lcd_needsupdate = 0;
int                lcd_buf_cleared = 0;
uint8_t            lcd_buffer[LCD_SCANLINE * LCD_H / 8];
bool               shiftHeld = false;
bool               altHeld   = false;

static disp_stat_t t20_ds = { .f = &lib_mono_10x17 };
static disp_stat_t t24_ds = { .f = &lib_mono_12x20 };
static disp_stat_t fReg_ds = { .f = &lib_mono_17x25 };
static FIL ppgm_fp_file;

sys_sdb_t sdb =
{
    .ppgm_fp = &ppgm_fp_file,
    .pds_t20 = &t20_ds,
    .pds_t24 = &t24_ds,
    .pds_fReg = &fReg_ds,
};


void LCD_power_off(int UNUSED clear)
{
    record(dmcp, "LCD_power_off");
}


void LCD_power_on()
{
    record(dmcp, "LCD_power_on");
}

uint32_t read_power_voltage()
{
    return BATTERY_VOFF + 25 + sys_current_ms() % (BATTERY_VMAX - BATTERY_VMIN);
}

int get_lowbat_state()
{
    return read_power_voltage() < 2300;
}

int usb_powered()
{
    return sys_current_ms() / 10000 % 3;
}

int create_screenshot(int report_error)
{
    record(dmcp_notyet,
           "create_screenshot(%d) not implemented", report_error);
    return 0;
}


void draw_power_off_image(int allow_errors)
{
    record(dmcp_notyet,
           "draw_power_off_image(%d) not implemented", allow_errors);
}


int handle_menu(const smenu_t * menu_id, int action, int cur_line)
{
    uint menu_line = 0;
    bool done = false;

    while (!done)
    {
        t24->xoffs = 0;
        lcd_writeClr(t24);
        lcd_writeClr(t20);
        lcd_clear_buf();
        lcd_putsR(t20, menu_id->name);

        char buf[80];
        uint count = 0;
        while (menu_id->items[count])
            count++;

        for (uint i = 0; i < count; i++)
        {
            uint8_t mid = menu_id->items[i];
            cstring label = menu_line_str_app(mid, buf, sizeof(buf));
            if (!label)
            {
                label = "Unimplemented DMCP menu";
                switch(mid)
                {
                case MI_MSC:            label = "Activate USB Disk"; break;
                case MI_PGM_LOAD:       label = "Load Program"; break;
                case MI_LOAD_QSPI:      label = "Load QSPI from FAT"; break;
                case MI_SYSTEM_ENTER:   label = "System >"; break;
                case MI_SET_TIME:       label = "Set Time >"; break;
                case MI_SET_DATE:       label = "Set Date >"; break;
                case MI_BEEP_MUTE:      label = "Beep Mute"; break;
                case MI_SLOW_AUTOREP:   label = "Slow Autorepeat"; break;
                case MI_DISK_INFO:      label = "Show Disk Info"; break;
                }
            }
            t24->inv = i == menu_line;
            lcd_printAt(t24, i+1, "%u. %s", i+1, label);
        }
        lcd_refresh();

        bool redraw = false;
        while (!redraw)
        {
            while (key_empty())
                sys_sleep();

            int key = key_pop();
            switch (key)
            {
            case KEY_UP:
                if (menu_line > 0)
                {
                    menu_line--;
                    redraw = true;
                }
                break;
            case KEY_DOWN:
                if (menu_line + 1 < count)
                {
                    menu_line++;
                    redraw = true;
                }
                break;
            case -1:
                // Signals that main application is exiting, leave all dialogs
            case KEY_EXIT:
                redraw = true;
                done = true;
                break;

            case KEY_ENTER:
                run_menu_item_app(menu_id->items[menu_line]);
                redraw = true;
                break;
            }
        }
    }

    return 0;
}

volatile int8_t  keys[4] = { 0 };
volatile uint    keyrd   = 0;
volatile uint    keywr   = 0;
enum {  nkeys = sizeof(keys) / sizeof(keys[0]) };

int key_empty()
{
    static bool empty = true;
    if ((keyrd == keywr) != empty)
    {
        record(keys_empty,
               "Key empty %u-%u = %+s",
               keyrd,
               keywr,
               keyrd == keywr ? "empty" : "full");
        empty = keyrd == keywr;
    }
    return keyrd == keywr;
}

int key_remaining()
{
    return nkeys - (keywr - keyrd);
}

int key_pop()
{
    if (keyrd != keywr)
    {
        record(keys, "Key %d (rd %u wr %u)", keys[keyrd % nkeys], keyrd, keywr);
        return keys[keyrd++ % nkeys];
    }
    return -1;
}

int key_tail()
{
    if (keyrd != keywr)
        return keys[(keyrd + nkeys - 1) % nkeys];
    return -1;
}

int key_pop_last()
{
    if (keywr - keyrd > 1)
        keyrd = keywr - 1;
    if (keyrd != keywr)
        return keys[keyrd++ % nkeys];
    return -1;
}

void key_pop_all()
{
    keyrd = 0;
    keywr = 0;
}
int key_push(int k)
{
    record(keys, "Push key %d (wr %u rd %u) shifts=%+s",
           k, keywr, keyrd,
           shiftHeld ? altHeld ? "Shift+Alt" : "Shift"
           : altHeld ? "Alt" : "None");
    MainWindow::theMainWindow()->pushKey(k);
    if (keywr - keyrd < nkeys)
        keys[keywr++ % nkeys] = k;
    else
        record(keys_warning, "Dropped key %d (wr %u rd %u)", k, keywr, keyrd);
    record(keys, "Pushed key %d (wr %u rd %u)", k, keywr, keyrd);
    return keywr - keyrd < nkeys;
}

int read_key(int *k1, int *k2)
{
    uint count = keywr - keyrd;
    if (shiftHeld || altHeld)
    {
        *k1 = keys[(keywr - 1) % nkeys];
        if (*k1)
        {
            *k2 = shiftHeld ? KEY_UP : KEY_DOWN;
            return 2;
        }
    }

    if (count > 1)
    {
        *k1 = keys[(keywr - 2) % nkeys];
        *k2 = keys[(keywr - 1) % nkeys];
        record(keys, "read_key has two keys %d and %d", *k1, *k2);
        return 2;
    }
    if (count > 0)
    {
        *k1 = keys[(keywr - 1) % nkeys];
        *k2 = 0;
        return 1;
    }
    *k1 = *k2 = 0;
    return 0;
}

int sys_last_key()
{
    return keys[(keywr - 1) % nkeys];
}

int runner_get_key(int *repeat)
{
    return repeat ? key_pop_last() :  key_pop();
}

void lcd_clear_buf()
{
    record(lcd, "Clearing buffer");
    for (unsigned i = 0; i < sizeof(lcd_buffer); i++)
        lcd_buffer[i] = 0xFF;
}

static uint32_t last_warning = 0;

inline void lcd_set_pixel(int x, int y)
{
    if (x < 0 || x > LCD_W || y < 0 || y > LCD_H)
    {
        uint now = sys_current_ms();
        if (now - last_warning > 1000)
        {
            record(lcd_warning, "Clearing pixel at (%d, %d)", x, y);
            last_warning = now;
        }
        return;
    }
    unsigned bo = y * LCD_SCANLINE + (LCD_W - x);
    if (bo/8 < sizeof(lcd_buffer))
        lcd_buffer[bo / 8] |= (1 << (bo % 8));
}

inline void lcd_clear_pixel(int x, int y)
{

    if (x < 0 || x > LCD_W || y < 0 || y > LCD_H)
    {
        uint now = sys_current_ms();
        if (now - last_warning > 1000)
        {
            record(lcd_warning, "Setting pixel at (%d, %d)", x, y);
            last_warning = now;
        }
        return;
    }
    unsigned bo = y * LCD_SCANLINE + (LCD_W - x);
    if (bo/8 < sizeof(lcd_buffer))
        lcd_buffer[bo / 8] &= ~(1 << (bo % 8));
}

inline void lcd_pixel(int x, int y, int val)
{
    if (!val)
        lcd_set_pixel(x, y);
    else
        lcd_clear_pixel(x, y);
}

void lcd_draw_menu_keys(const char *keys[])
{
    int my = LCD_H - t20->f->height - 4;
    int mh = t20->f->height + 2;
    int mw = (LCD_W - 10) / 6;
    int sp = (LCD_W - 5) - 6 * mw;

    t20->inv = 1;
    t20->lnfill = 0;
    t20->bgfill = 1;
    t20->newln = 0;
    t20->y = my + 1;

    record(lcd, "Menu [%s][%s][%s][%s][%s][%s]",
           keys[0], keys[1], keys[2], keys[3], keys[4], keys[5]);
    for (int m = 0; m < 6; m++)
    {
        int x = (2 * m + 1) * mw / 2 + (m * sp) / 5 + 2;
        lcd_fill_rect(x - mw/2+2, my,   mw-4,   mh, 1);
        lcd_fill_rect(x - mw/2+1, my+1, mw-2, mh-2, 1);
        lcd_fill_rect(x - mw/2,   my+2, mw,   mh-4, 1);

        // Truncate the menu to fit
        // Note that DMCP is NOT robust to overflow here and can die
        int size = 11;
        int w = 0;
        char buffer[12];
        do
        {
            snprintf(buffer, sizeof(buffer), "%.*s", size, keys[m]);
            w = lcd_textWidth(t20, buffer);
            size--;
        } while (w > mw);

        if (size < (int) strlen(keys[m]))
            record(lcd_warning,
                       "Menu entry %d [%s] is too long "
                       "(%d chars lost, shows as [%s])",
                       m, keys[m], (int) strlen(keys[m]) - size + 1, buffer);

        t20->x = x - w / 2;
        lcd_puts(t20, buffer);
    }
    t20->lnfill = 1;
    t20->inv = 0;
}

void lcd_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, int val)
{
    if (val)
        record(lcd, "Fill  rectangle (%u,%u) + (%u, %u)", x, y, w, h);
    else
        record(lcd, "Clear rectangle (%u,%u) + (%u, %u)", x, y, w, h);

    if (x + w > LCD_W)
    {
        record(lcd_warning,
               "Rectangle X is outside screen (%u, %u) + (%u, %u)",
               x, y, w, h);
        w = LCD_W - x;
        if (w > LCD_W)
            x = w = 0;
    }
    if (y +h > LCD_H)
    {
        record(lcd_warning,
               "Rectangle Y is outside screen (%u, %u) + (%u, %u)",
               x, y, w, h);
        h = LCD_W - y;
        if (h > LCD_W)
            y = h = 0;
    }

    for (uint r = y; r < y + h; r++)
        for (uint c = x; c < x + w; c++)
            lcd_pixel(c, r, val);
}

int lcd_fontWidth(disp_stat_t * ds)
{
    return ds->f->width;
}
int lcd_for_calc(int what)
{
    record(dmcp_notyet, "lcd_for_calc %d not implemented", what);
    return 0;
}
int lcd_get_buf_cleared()
{
    record(lcd, "get_buf_cleared returns %d", lcd_buf_cleared);
    return lcd_buf_cleared;
}
int lcd_lineHeight(disp_stat_t * ds)
{
    return ds->f->height;
}
uint8_t * lcd_line_addr(int y)
{
    if (y < 0 || y > LCD_H)
    {
        record(lcd_warning, "lcd_line_addr(%d), line is out of range", y);
        y = 0;
    }
    unsigned offset = y * LCD_SCANLINE / 8;
    return lcd_buffer + offset;
}
int lcd_toggleFontT(int nr)
{
    return nr;
}
int lcd_nextFontNr(int nr)
{
    if (nr < (int) dmcp_fonts_count - 1)
        nr++;
    else
        nr = dmcp_fonts_count - 1;
    return nr;
}
int lcd_prevFontNr(int nr)
{
    if (nr > 0)
        nr--;
    else
        nr = 0;
    return nr;
}
void lcd_prevLn(disp_stat_t * ds)
{
    ds->y -= lcd_lineHeight(ds);
    ds->x = ds->xoffs;
}
void lcd_print(disp_stat_t * ds, const char* fmt, ...)
{
    static char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    lcd_puts(ds, buffer);
}

void lcd_forced_refresh()
{
    record(lcd, "Forced refresh");
    lcd_needsupdate++;
}
void lcd_refresh()
{
    record(lcd_refresh, "Refresh %u", lcd_needsupdate);
    lcd_needsupdate++;
}
void lcd_refresh_dma()
{
    record(lcd_refresh, "Refresh DMA %u", lcd_needsupdate);
    lcd_needsupdate++;
}
void lcd_refresh_wait()
{
    record(lcd_refresh, "Refresh wait %u", lcd_needsupdate);
    lcd_needsupdate++;
}
void lcd_refresh_lines(int ln, int cnt)
{
    record(lcd_refresh, "Refresh lines %u (%d-%d) count %d",
           lcd_needsupdate, ln, ln+cnt-1, cnt);
    lcd_needsupdate += (ln >= 0 && cnt > 0);
}
void lcd_setLine(disp_stat_t * ds, int ln_nr)
{
    ds->x = ds->xoffs;;
    ds->y = ln_nr * lcd_lineHeight(ds);
    record(lcd, "set line %u coord (%d, %d)", ln_nr, ds->x, ds->y);
}

void lcd_setXY(disp_stat_t * ds, int x, int y)
{
    record(lcd, "set XY (%d, %d)", x, y);
    ds->x = x;
    ds->y = y;
}
void lcd_set_buf_cleared(int val)
{
    record(lcd, "Set buffer cleared %d", val);
    lcd_buf_cleared = val;
}
void lcd_switchFont(disp_stat_t * ds, int nr)
{
    record(lcd, "Selected font %d", nr);
    if (nr >= 0 && nr <= (int) dmcp_fonts_count)
        ds->f = dmcp_fonts[nr];
}

int lcd_charWidth(disp_stat_t * ds, int c)
{
    int                width = 0;
    const line_font_t *f     = ds->f;
    byte               first = f->first_char;
    byte               count = f->char_cnt;
    const uint16_t    *offs  = f->offs;
    const uint8_t     *data  = f->data;
    uint               xspc  = ds->xspc;

    c -= first;
    if (c >= 0 && c < count)
    {
        uint off = offs[c];
        width += data[off + 0] + data[off + 2] + xspc;
        record(lcd_width,
               "Character width of %c (%d=0x%x) is %d",
               c + first, c + first, c + first, width);
    }
    else
    {
        record(lcd_width, "Character width of nonexistent %d is %d", c, width);
    }

    return width;
}

int lcd_textWidth(disp_stat_t * ds, const char* text)
{
    int                width = 0;
    byte               c;
    const line_font_t *f     = ds->f;
    byte               first = f->first_char;
    byte               count = f->char_cnt;
    const uint16_t    *offs  = f->offs;
    const uint8_t     *data  = f->data;
    uint               xspc  = ds->xspc;
    const byte         *p    = (const byte *) text;

    while ((c = *p++))
    {
        c -= first;
        if (c >= 0 && c < count)
        {
            uint off = offs[c];
            width += data[off + 0] + data[off + 2] + xspc;
        }
        else
        {
            record(lcd_width,
                   "Nonexistent character %d at offset %d in [%s]",
                   c + first, p - (const byte *) text, text);
        }
    }
    return width;
}

void lcd_writeClr(disp_stat_t *ds)
{
    record(lcd, "Clearing display state"); // Not sure this is what it does
    ds->x      = 0; // ds->xoffs;
    ds->y      = 0;
    ds->inv    = 0;
    ds->bgfill = 1;
    ds->lnfill = 1;
    ds->newln  = 1;
    ds->xspc   = 1;
}

void lcd_writeNl(disp_stat_t *ds)
{
    ds->x = ds->xoffs;
    ds->y += lcd_lineHeight(ds);
    record(lcd, "New line, now at (%d, %d)", ds->x, ds->y);
}

inline void lcd_writeTextInternal(disp_stat_t *ds, const char *text, int write)
{
    uint               c;
    const line_font_t *f        = ds->f;
    uint               first    = f->first_char;
    uint               count    = f->char_cnt;
    uint               height   = f->height;
    const uint8_t     *data     = f->data;
    const uint16_t    *offs     = f->offs;
    int                xspc     = ds->xspc;
    int                x        = ds->x + xspc;
    int                y        = ds->y + ds->ln_offs;
    int                inv      = ds->inv != 0;
    const byte        *p        = (const byte *) text;

    if (write)
        record(lcd, "Write text [%s] at (%d, %d)", text, x, y);
    else
        record(lcd, "Skip text [%s] at (%d, %d)", text, x, y);

    if (ds->lnfill)
        lcd_fill_rect(ds->xoffs, y, LCD_W, height, inv);

    while ((c = *p++))
    {
        c -= first;
        if (c < count)
        {
            int            off  = offs[c];
            const uint8_t *dp   = data + off;
            int            cx   = *dp++;
            int            cy   = *dp++;
            int            cols = *dp++;
            int            rows = *dp++;

            if (!write)
            {
                x += cx + cols;
                continue;
            }

            for (int r = 0; r < cy; r++)
                for (int c = 0; c < cx + cols; c++)
                    lcd_pixel(x+c, y+r, inv);

            for (int r = 0; r < rows; r++)
            {
                int data = 0;
                for (int c = 0; c < cols; c += 8)
                    data |= *dp++ << c;

                for (int c = 0; c < cx; c++)
                    lcd_pixel(x+c, y+r, inv);

                for (int c = 0; c < cols; c++)
                {
                    int val = (data >> (cols - c - 1)) & 1;
                    if (val || ds->bgfill)
                        lcd_pixel(x + c + cx, y + r + cy, val != inv);
                }
            }

            for (uint r = cy + rows; r < height; r++)
                for (int c = 0; c < cx + cols; c++)
                    lcd_pixel(x+c, y+r, inv);


            x += cx + cols + xspc;
        }
        else
        {
            record(lcd_warning,
                   "Nonexistent character [%d] in [%s] at %d, max=%d",
                   c + first, text, p - (byte_p) text, count + first);
        }

    }
    ds->x = x;
    if (ds->newln)
    {
        ds->x = ds->xoffs;
        ds->y += height;
    }
}

void lcd_writeText(disp_stat_t * ds, const char* text)
{
    lcd_writeTextInternal(ds, text, 1);
}
void lcd_writeTextWidth(disp_stat_t * ds, const char* text)
{
    lcd_writeTextInternal(ds, text, 0);
}
void reset_auto_off()
{
    // No effect
}
void rtc_wakeup_delay()
{
    record(dmcp_notyet, "rtc_wakeup_delay not implemented");
}
void run_help_file(const char * help_file)
{
    record(dmcp_notyet, "run_help_file not implemented");
}
void run_help_file_style(const char * help_file, user_style_fn_t *user_style_fn)
{
    record(dmcp_notyet, "run_help_file_style not implemented");
}
void start_buzzer_freq(uint32_t freq)
{
    record(dmcp_error, "start_buzzer");
}
void stop_buzzer()
{
    record(dmcp_notyet, "stop_buzzer not implemented");
}

int sys_free_mem()
{
    // On the simulator, we have real memory
    return 1024 * 1024;
}

void sys_delay(uint32_t ms_delay)
{
    QThread::msleep(ms_delay);
}

static struct timer
{
    uint32_t deadline;
    bool     enabled;
} timers[4];

void sys_sleep()
{
    while (key_empty())
    {
        uint32_t now = sys_current_ms();
        for (int i = 0; i < 4; i++)
            if (timers[i].enabled && int(timers[i].deadline - now) < 0)
                return;
        QThread::msleep(20);
    }
    CLR_ST(STAT_SUSPENDED | STAT_OFF | STAT_PGM_END);
}

void sys_critical_start()
{
}

void sys_critical_end()
{
}

void sys_timer_disable(int timer_ix)
{
    timers[timer_ix].enabled = false;
}

void sys_timer_start(int timer_ix, uint32_t ms_value)
{
    uint32_t now = sys_current_ms();
    uint32_t then = now + ms_value;
    timers[timer_ix].deadline = then;
    timers[timer_ix].enabled = true;
}
int sys_timer_active(int timer_ix)
{
    return timers[timer_ix].enabled;
}

int sys_timer_timeout(int timer_ix)
{
    uint32_t now = sys_current_ms();
    if (timers[timer_ix].enabled)
        return int(timers[timer_ix].deadline - now) < 0;
    return false;
}

void wait_for_key_press()
{
    wait_for_key_release(-1);
    while (key_empty() || !key_pop())
        sys_sleep();
}
void wait_for_key_release(int tout)
{
    record(dmcp_notyet, "wait_for_key_release not implemented");
    while (!key_empty() && key_pop())
        sys_sleep();
}

int file_selection_screen(const char   *title,
                          const char   *base_dir,
                          const char   *ext,
                          file_sel_fn_t sel_fn,
                          int           disp_new,
                          int           overwrite_check,
                          void         *data)
{
    int ret = 0;

    // Make things relative to the working directory
    if (*base_dir == '/' || *base_dir == '\\')
        base_dir++;

    QString path;
    bool done = false;

    postToThread([&]{ // the functor captures parent and text by value
        path =
            disp_new
            ? QFileDialog::getSaveFileName(nullptr,
                                           title,
                                           base_dir,
                                           QString("*") + QString(ext),
                                           nullptr,
                                           overwrite_check
                                           ? QFileDialog::Options()
                                           : QFileDialog::DontConfirmOverwrite)
            : QFileDialog::getOpenFileName(nullptr,
                                           title,
                                           base_dir,
                                           QString("*") + QString(ext));
        std::cout << "Selected path: " << path.toStdString() << "\n";
        done = true;
    });

    while (!done)
        sys_sleep();

    std::cout << "Got path: " << path.toStdString() << "\n";
    QFileInfo fi(path);
    QString name = fi.fileName();
    ret = sel_fn(path.toStdString().c_str(),
                 name.toStdString().c_str(),
                 data);
    return ret;
}

int power_check_screen()
{
    record(dmcp, "file_selection_screen not imlemented");
    return 0;
}

int sys_disk_ok()
{
    return 1;
}

int sys_disk_write_enable(int val)
{
    return 0;
}

uint32_t sys_current_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000 + tv.tv_usec) / 1000;
}


FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode)
{
    record(dmcp_notyet, "f_open not implemented");
    return FR_NOT_ENABLED;
}
FRESULT f_close(FIL *fp)
{
    record(dmcp_notyet, "f_close not implemented");
    return FR_NOT_ENABLED;
}

FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br)
{
    record(dmcp_notyet, "f_read not implemented");
    return FR_NOT_ENABLED;
}

FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw)
{
    record(dmcp_notyet, "f_write not implemented");
    return FR_NOT_ENABLED;
}

FRESULT f_lseek(FIL *fp, FSIZE_t ofs)
{
    record(dmcp_notyet, "f_lseek not implemented");
    return FR_NOT_ENABLED;
}

FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new)
{
    record(dmcp_notyet, "f_rename not implemented");
    return FR_NOT_ENABLED;
}

FRESULT f_unlink(const TCHAR *path)
{
    record(dmcp_notyet, "f_unlink not implemented");
    return FR_NOT_ENABLED;
}


void disp_disk_info(const char *hdr)
{
}

void set_reset_state_file(const char * str)
{
    QSettings settings;
    settings.setValue("state", str);
    record(dmcp, "Setting saved state: %s", str);
}


char *get_reset_state_file()
{
    static char result[256];
    QSettings settings;
    QString file = settings.value("state").toString();
    result[0] = 0;
    if (!file.isNull())
        strncpy(result, file.toStdString().c_str(), sizeof(result));
    record(dmcp, "Saved state: %+s", result);
    return result;
}

uint32_t reset_magic = 0;
void set_reset_magic(uint32_t value)
{
    reset_magic = value;
}

void sys_reset()
{
}

int is_menu_auto_off()
{
    return false;
}


void rtc_read(tm_t * tm, dt_t *dt)
{
    time_t now;
    time(&now);

    struct tm utm;
    localtime_r(&now, &utm);

    dt->year = 1900 + utm.tm_year;
    dt->month = utm.tm_mon;
    dt->day = utm.tm_mday;

    tm->hour = utm.tm_hour;
    tm->min = utm.tm_min;
    tm->sec = utm.tm_sec;
    tm->csec = 0;
    tm->dow = utm.tm_wday;
}


cstring get_wday_shortcut(int day)
{
    static cstring dow[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    return dow[day];
}

cstring get_month_shortcut(int month)
{
    static cstring name[] =
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    return name[month];
}


int check_create_dir(const char * dir)
{
    return mkdir(dir, 0777);
}
