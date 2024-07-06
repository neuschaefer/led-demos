LED_BASE_DIR ?= ../rpi-rgb-led-matrix

LED_LIB_DIR=${LED_BASE_DIR}/lib
LED_INCLUDE_DIR=${LED_BASE_DIR}/include

binaries = wanja marcel best rainbow mimoja ledflut fireworks

%: %.cc
	$(CXX) $< -std=c++11 -lpthread -L$(LED_LIB_DIR) -lrgbmatrix -I$(LED_INCLUDE_DIR) -I$(LED_INCLUDE_DIR)/rpi-rgb-led-matrix -o $@

all: $(binaries)

clean:
	$(RM) $(binaries)

.PHONY: all clean
