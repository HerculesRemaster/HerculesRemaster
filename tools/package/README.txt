Hercules Remaster {version}
===========================

A display add-on for your own installed copy of Disney's Hercules (1997, Windows). It contains no
game files. The game runs unchanged; the add-on replaces how it is drawn:

- fullscreen borderless on modern Windows (any resolution, integer scaling, 4:3 with side bars)
- a GPU renderer that redraws the game at the screen's resolution (9x at 3840x2160)
- optional 4K replacement art (art\ folder), drawn at the original size, position and timing

Install
  PowerShell in this folder:   .\install.ps1 -GameDir "C:\path\to\Hercules"
  The installer checks that the game files are the expected edition and backs up anything it
  replaces to <GameDir>\remaster-backup\.

Uninstall
  <GameDir>\remaster-backup\uninstall.ps1 -GameDir "C:\path\to\Hercules"

Settings (hercules_display.ini next to HERCULES.EXE)
  mode = borderless | windowed     renderer = gpu | gdi (original 8-bit picture, scaled)
  scale = integer | fit | stretch  art_families = sprite,level,ui,screen (which art to use)
  dev = on shows a debug overlay.  In game, F11 switches the replacement art on and off.

Saves and settings
  The game keeps its options in the registry (HKLM\Software\Disney Interactive\Hercules) and its
  progress as before; the add-on reads and writes neither. It writes only hercules_display.log
  when log = on.
