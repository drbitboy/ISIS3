APPNAME = reduce

labels3.txt.IGNORELINES = ByteOrder StartByte Bytes TileSamples TileLines
labels6.txt.IGNORELINES = ByteOrder StartByte Bytes TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/peaks.cub \
	  to= $(OUTPUT)/reduceTruth3.cub \
	  algorithm=average \
	  mode=total \
	  ons=100 \
	  onl=100 > /dev/null;
	catlab from=$(OUTPUT)/reduceTruth3.cub  to=$(OUTPUT)/labels3.txt > /dev/null;
	$(APPNAME) from= $(OUTPUT)/reduceTruth3.cub \
          to= $(OUTPUT)/reduceTruth6.cub \
          algorithm=average \
	  mode=total \
	  ons=50 \
          onl=50 > /dev/null;
	catlab from=$(OUTPUT)/reduceTruth6.cub to=$(OUTPUT)/labels6.txt > /dev/null;
