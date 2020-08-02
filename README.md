# storyphone
Wählscheibentelefon MP3 Player

Code und Hardware für ein Arduino Shield. Funktionen und Details:
- Speisung 24VDC (Ein 6W Netzteil reicht)
- Schaltregler um 5V für das MP3 Modul und 9V für den Arduino VIN zu generieren (getrennt wegen Störungen des MP3 Moduls)
- Ausgabe Leitungston über DIO2
- Detektion der Gabelzustände und Wählimpulse über Optokoppler und DIO4
- USB Stick (FAT32) mit Dateien im Rootfolder:
  - 0001.mp3 für Zahl 1
  - 0002.mp3 für Zahl 2 
  -  etc 
  - 0009.mp3 für Zahl 9
  - 0010.mp3 für Zahl 0
- Volume der Audiowiedergabe kann am Drehknopf eingestellt werden

!!! Funktioniert bei mir nur mit einem Original Arduino (mit Klon bei Speisung über VIN erst nach manuellem Drücken des Reset Buttons).

![](https://github.com/sxwid/storyphone/blob/master/M%C3%A4rchentelefon.svg.png)
