CXX = g++
CXXFLAGS = `root-config --cflags` -O2 -Wall -fopenmp
LDLIBS = `root-config --libs` -lSpectrum -fopenmp

TARGETS = jitter_extractor_errors

all: $(TARGETS)

jitter_extractor_errors: test_openmp_jitterextract_updated.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(TARGETS)
