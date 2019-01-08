
CC=g++

SRC := CalcHistogram.cpp glcm.cpp scene.cpp tools.cpp v-sfp.cpp input.cpp 
OBJ := $(subst .cpp,.o,$(SRC))

target := v-sfp

CFLAGS := -std=c++11 -O3 -I$(OPENCV2_INCLUDE) -L$(OPENCV2_LIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_contrib -lopencv_superres -lopencv_legacy -lopencv_stitching -lopencv_ocl -lopencv_photo -lopencv_ocl -lopencv_objdetect

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: $(target)


$(target): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

install:
	cp $(target) /usr/local/bin

uninstall:
	rm -f  /usr/local/bin/$(target)
	
clean:
	rm -f *.o
	rm -f $(target)

