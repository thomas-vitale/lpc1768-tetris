# Attribution and third-party material

The Keil project layout is kept exactly as the toolchain expects it, so the repository
builds as-is. That means my code and the board support package share the `Source/` tree;
this file is what separates them.

## Mine

- `Source/Tetris/` — game logic (`tetris_engine.c`), graphics and screens
  (`tetris_gfx.c`), tetromino tables (`asset.c`), and the two assembly routines
  (`LFSR.S`, `copy_tetronominos.s`).
- `Source/RIT/IRQ_RIT.c` — the interrupt handler that schedules the whole game.
- `Source/button_EXINT/`, `Source/joystick/` — input handling.

## Third party, redistributed with headers intact

| Path | Origin | Licence |
|---|---|---|
| `Source/CMSIS_core/` | ARM CMSIS | Apache 2.0 |
| `Source/GLCD/` (`GLCD.c`, `AsciiLib.c`, `HzLib.c` and headers) | powermcu.com, "AVRman", 2010–2012 | Not stated by the authors |
| `Source/TouchPanel/` | same board support package | Not stated by the authors |
| `Source/startup_LPC17xx.s`, `Source/system_LPC17xx.c` | NXP / CMSIS device files | Apache 2.0 |
| `Source/sample.c` | skeleton from powermcu.com, modified by P. Bernardi for the course; the `main()` body is mine | Not stated by the authors |

The GLCD, touch panel and board skeleton files come from the LandTiger board support
package distributed with the course. Their original copyright headers have not been
altered. If you are a rights holder for any of them and want a file removed, open an issue.

`LPC17xx.h` and the rest of the LPC1700 device pack are **not** included: install them
through the Keil pack manager.

## Licence

My own code is released under the MIT licence — see [LICENSE](LICENSE). The MIT grant
does not extend to the third-party files listed above, which keep their own terms.
