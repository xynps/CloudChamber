#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMaxim.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"

#include "model.h"
#include "instruments.h"
#include "mixer.h"
#include "space.h"
#include "shapes.h"
#include "attractor.h"
#include "clifford.h"
#include "counter.h"
#include "random.h"
#include "timing.h"
#include "states.h"

class ofApp : public ofBaseApp {
    public:
        void structure();
        void setup();
        void update();
        void draw();
        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void gotMessage(ofMessage msg);
        void dragEvent(ofDragInfo dragInfo);



		int initialBufferSize;
		const int sampleRate = 44100;
		const int bufferSize = 512;
		void audioOut(float * output, int bufferSize, int nChannels);
		double outputs[2];

        int numattractors = 0;
        int bgreset = 0;
        int bgresetmax = 128;
        int granprob = 2;
        int divisionsize = 32;

		double mix;
		double f;

        ofLight light;
        Space space;
        Attractor attractor[4];
        Model model[2];
        Points points [1];
        Lines lines [1];
        Mesh mesh [4];
        Counter counter;
        Timing timing;
        ofxOscReceiver receiver;
        States states;
		Clifford clifford;

	    Mixer mixer;

		    Fm fm[512];

        int width = space.width;
        int height = space.height;
        int depth = space.depth;

        maxiOsc osc1;
        maxiOsc osc2;
        maxiDelayline dl1;
        maxiFilter f1;
        maxiDistortion m1;
        maxiKick k1;
};
