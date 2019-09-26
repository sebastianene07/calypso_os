#include <board.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <../drivers/display/ssd_1331.h>

extern const unsigned char simons_cat_bin[];

static Color get_color_from_name(const char *color_name)
{
  if (!strcmp("BLACK", color_name))
    return COLOR_BLACK;
  else if (!strcmp("GREY", color_name))
    return COLOR_GREY;
  else if (!strcmp("WHITE", color_name))
    return COLOR_WHITE;
  else if (!strcmp("RED", color_name))
    return COLOR_RED;
  else if (!strcmp("PINK", color_name))
    return COLOR_PINK;
  else if (!strcmp("YELLOW", color_name))
    return COLOR_YELLOW;
  else if (!strcmp("GOLDEN", color_name))
    return COLOR_GOLDEN;
  else if (!strcmp("BROWN", color_name))
    return COLOR_BROWN;
  else if (!strcmp("BLUE", color_name))
    return COLOR_BLUE;
  else if (!strcmp("CYAN", color_name))
    return COLOR_CYAN;
  else if (!strcmp("GREEN", color_name))
    return COLOR_GREEN;
  else
    return COLOR_PURPLE;
}

static DisplayPower get_power_from_name(const char *power_name)
{
  if (!strcmp("DIM", power_name))
    return DimMode;
  else if (!strcmp("SLEEP", power_name))
    return SleepMode;
  else
    return NormalMode;

}

static DisplayMode get_mode_from_name(const char *mode_name)
{
  if (!strcmp("NORMAL", mode_name))
    return NormalDisplay;
  else if (!strcmp("ON", mode_name))
    return DisplayOn;
  else if (!strcmp("OFF", mode_name))
    return DisplayOff;
  else
    return InverseDisplay;
}

/*
 * display - Test the SSD1331 display functionality
 *
 */
int display(int argc, const char *argv[])
{
  if (argc >= 2)
  {
    if (strcmp(argv[1], "frame") == 0)
    {
      if (argc == 4)
      {
        Color frame_color = get_color_from_name(argv[2]);
        Color fill_color  = get_color_from_name(argv[3]);

        ssd1331_display_drawFrame(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT, frame_color, fill_color);
      }
      else
      {
        printf("Wrong command: display frame <frame_color> <fill_color>\n"
               "{BLACK | GREY | WHITE | RED | PINK | YELLOW | GOLDEN"
               "| BROWN | BLUE | CYAN | GREEN | PURPLE}\n\n");
      }
    }
    else if (strcmp(argv[1], "clear") == 0)
    {
      ssd1331_display_clearWindow(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT);
    }
    else if (strcmp(argv[1], "dim") == 0)
    {
      ssd1331_display_dimWindow(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT);
    }
    else if (strcmp(argv[1], "mode") == 0)
    {
      if (argc == 3)
      {
        ssd1331_display_setDisplayMode(get_mode_from_name(argv[2]));
      }
      else
      {
        printf("Wrong command: display mode <mode_name>\n"
               "{ NORMAL | ON | OFF | INVERT }\n\n");
      }
    }
    else if (strcmp(argv[1], "power") == 0)
    {
      if (argc == 3)
      {
        ssd1331_display_setDisplayPower(get_power_from_name(argv[2]));
      }
      else
      {
        printf("Wrong command: display power <power_mode>\n"
               "{ DIM | SLEEP | NORMAL }\n\n");
      }
    }
    else if (strcmp(argv[1], "draw") == 0)
    {
      ssd1331_display_drawBuffer(0, 0, (uint8_t *)simons_cat_bin, 96 * 64 * 2);
    }
  }
  else
  {
    printf("Supported operations are:\n""display frame <frame_color> <fill_color>\n"
        "display clear\n"
        "display dim\n"
        "display mode <mode_name>\n"
        "display power <power_mode>\n"
        "display draw\n\n");
  }

  return 0;
}
