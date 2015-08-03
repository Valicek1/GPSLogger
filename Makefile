INOFILE=GPSLogger.ino
BASENAME=$(basename $(INOFILE))
BOARD=arduino:avr:uno

PARTNO=atmega328p

build: 
	[ -d tmp ] || mkdir tmp
	arduino --verify --board $(BOARD) --pref build.path=tmp/ $(INOFILE)
	cp tmp/$(BASENAME).cpp.eep $(BASENAME).eep
	cp tmp/$(BASENAME).cpp.hex $(BASENAME).hex

all: clean isp-eep

isp: build
	avrdude -c usbasp -p $(PARTNO) -P USB -U flash:w:$(BASENAME).hex

load: build	
	avrdude -c arduino -p $(PARTNO) -P /dev/ttyUSB* -b 115200 -U flash:w:$(BASENAME).hex

isp-full: build
	avrdude -c usbasp -p $(PARTNO) -P USB -e -U flash:w:$(BASENAME).hex -U eeprom:w:$(BASENAME).eep

c: load console

console:
	screen /dev/ttyUSB* 115200

	    
clean:
	rm -Rf tmp
	rm -Rf $(BASENAME).hex
	rm -Rf $(BASENAME).eep

