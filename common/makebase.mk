COMMONDIR = ../../common/
SHADERDIR = shaders/

CC=g++
CFLAGS=-std=c++2a -Wall -Wextra -pedantic -Werror -O3 -ffunction-sections -fdata-sections
INCLUDES=-I$(COMMONDIR)
LDFLAGS=-Wl,--gc-sections
LIBS=-lglfw -lGL

CFILES=$(wildcard *.cpp) $(wildcard $(COMMONDIR)*.cpp)

SHADERGENERATE=../../tools/generate-shader.py
SHADERS=$(wildcard $(SHADERDIR)*.cfg)
SHADERS_H=$(patsubst $(SHADERDIR)%.cfg,$(SHADERDIR)Program%.h,$(SHADERS))
SHADERS_CPP=$(patsubst $(SHADERDIR)%.cfg,$(SHADERDIR)Program%.cpp,$(SHADERS))
SHADERS_O=$(patsubst $(SHADERDIR)%.cfg,$(SHADERDIR)Program%.o,$(SHADERS))

OFILES=$(patsubst %.cpp,%.o,$(CFILES)) $(SHADERS_O)

%.o: %.cpp $(SHADERS_H)
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

$(BIN): $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(SHADERS_CPP) $(SHADERS_H): $(SHADERGENERATE) $(SHADERS)
	python3 $^

clean:
	rm -f $(BIN) $(OFILES) $(SHADERS_H) $(SHADERS_CPP)
