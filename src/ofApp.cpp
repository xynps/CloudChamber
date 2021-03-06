#include "ofApp.h"

void ofApp::setup() {
	ofSetFullscreen(true);
	ofEnableLighting();
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
	ofSetBackgroundAuto(false);
	ofSetBackgroundColor(30, 30, 30);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	light.setup();
	light.setPointLight();
	light.setPosition(0, 0, 0);
	light.setAttenuation(1, (0.000001), (0.000001));
	mesh[0].init(-width / 4, -height / 4, -depth / 4, width / 4, height / 4, depth / 4);
	model[0].init("heart.obj", 0.5);
	light.disable();
	points[0].init(width,height,depth);
	for(int i = 0; i < 4; i++)
		attractor[i].init(width, height, depth);
	ofSoundStreamSetup(2, 2, this, sampleRate, bufferSize, 4); // initialise audio
}

double ofApp::wavetable(const int& sample, const int bufferSize) {
	if (sample >= 0 && sample < bufferSize * 0.25)
		return wta.wtx[0][sample];
	else if (sample >= bufferSize * 0.25 && sample < bufferSize * 0.5)
		return 1 - wta.wtx[1][sample];
	else if (sample >= bufferSize * 0.5 && sample < bufferSize * 0.75)
		return wta.wtx[2][sample-256] * -1;
	else if (sample >= bufferSize * 0.75 && sample < bufferSize)
		return 1 - wta.wtx[3][sample - 384] * - 1;
	else
		return 0;
}



void ofApp::audioOut(float * output, int bufferSize, int nChannels) {

	vector<double> levels = {0.2, 0.5, 0.2, 0.5}; // local mixer levels
	wta.update(width/2, height/2, depth/2, points[0].vertices); // update wavetable array
	fm.index = (points[0].velavrg()*1000)+1000;

	for (int sample = 0; sample < bufferSize; ++sample) {
		// map area of particles to filter bandwidth that filters fm/wavetable synth process
		bp.f1 = points[0].area();
		bp.f2 = points[0].area()+50;
		bp.q = 8;
		mixer.assign(1, bp.output(fm.output(wavetable(sample, 512))));
		mixer.setLevel(1, levels[0]);
		
		// trigger percussion sounds when particles are connected
		perc.trigger(points[0].state);
		mixer.assign(2, dist.fastAtanDist(perc.output(), 4));
		mixer.setLevel(2, levels[1]);

		// trigger certain kick drum sounds when model is being generated
		kick1.trigger(model[0].bang);
		mixer.assign(3, kick1.output());
		mixer.setLevel(3, levels[2]);

		// trigger other kick drum sounds when lines are rendered
		kick2.k.setPitch(ofRandom(2000, 2000));
		kick2.k.setRelease(50);
		kick2.filter = false;
		kick2.distortion = false;
		kick2.trigger(lines[0].bang);
		mixer.assign(4, kick2.output());
		mixer.setLevel(4, levels[3]);

		// summed mixer output is sent to audio output
		output[sample * nChannels] =	    mixer.output();
		output[sample * nChannels + 1] = 	output[sample * nChannels];
	}

	// reset states
	points[0].state = 0;
	model[0].bang = false;
	lines[0].bang = false;
}

void ofApp::structure() {
	int frame = ofGetFrameNum();
	int change = 6000;
	if (frame % change == 0) states.changeState(states.getCurrent() + 1);
	switch (states.getCurrent()) {
		case 1:
			bgalpha = 20;
			granprob = 0;
			light.disable();
			numattractors = 1;
			attractor[0].f = 2;
			attractor[0].pos.set(0,0,0);
			break;
		case 2:
			granprob = 1;
			break;
		case 3:
			attractor[0].f = -2;
			if (frame % 6000 == 0)
				points[0].stop();
			break;
		case 4:
			model[0].active = true;
			break;
		case 5:
			if (frame % 6000 == 0) {
				points[0].stop();
				numattractors = 3;
				attractor[0].f = 2;
				attractor[1].f = -2;
				attractor[2].f = 4;
            }
			break;
		case 6:
			granprob = 0;
			if (model[0].vertexcounter == 0) model[0].active = false;
			break;
		case 7:
			lines[0].active = true;
			if (model[0].vertexcounter == 0) model[0].active = false;
			break;
		case 8:
			if (frame % 6000 == 0) {
				numattractors = 2;
				attractor[0].f = 4;
				attractor[1].f = 2;
				points[0].stop();
			}
			break;
		case 9:
			numattractors = 0;
			break;
		case 10:
			bgalpha = 20;
			lines[0].active = false;
			granprob = 0;
			numattractors = 0;
			break;
		case 11:
			states.changeState(1);
			break;
		default:
			states.changeState(1);
			break;
		}
}

void ofApp::update() {
	structure(); // call the function that determines state changes over time
	for (int i = 0; i < 4; ++i)
		attractor[i].light.disable();
	if (timing.getFrame() % 800 == 0)
		lines[0].clear(width, height, depth, 1);
	lines[0].update(width, height, depth, 4);
	points[0].update(width, height, depth);
	model[0].render(0, -height / 16, 0, 2, 250, 5);
	for(int i = 0; i < numattractors; ++i) {
		attractor[i].limit(width, height, depth);
		attractor[i].lighton();
		attractor[i].update(25);
		points[0].attracted(attractor[i].pos, attractor[i].f, numattractors);
		for(int j = 0; j <= numattractors; j++)
			if (j != i)
				attractor[j].attracted(attractor[i].pos, attractor[i].f, numattractors);
	}
}


void ofApp::draw(){
	space.cam.begin();
	space.drawBackground(0, bgalpha);
	space.drawWireframe(8, 25);
	points[0].draw(250, 250, granprob);
	for (int i = 0; i < numattractors; ++i)
		attractor[i].draw(10,5);
	lines[0].draw(0, 0, 255, 25);
	model[0].draw(250, 50, 180, 180, 0.01);
	space.cam.end();
	timing.displayData();
}

void ofApp::keyPressed(int key){
	space.movecam(key);
	switch (key) {
		case '1':
			space.framedraw = !space.framedraw;
			break;
		case '2':
			for(int i = 0; i < 4; i++)
				attractor[i].attract = !attractor[i].attract;
			break;
		case 'r':
			points[0].stop();
			for(int i = 0; i < numattractors; i++) {
				attractor[i].init(width, height, depth);
				if(i == 0)
					attractor[0].pos.set(0, 0, 0);
				ofBackground(0);
			}
			break;
		case ' ':
			numattractors += 1;
			numattractors = numattractors % 5;
			points[0].stop();
			for(int i = 0; i < numattractors; i++) {
				attractor[i].init(width, height, depth);
				if(i == 0) attractor[0].pos.set(0, 0, 0);
			}
			ofBackground(0);
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		default:
			break;
	}
	
}

void ofApp::keyReleased(int key){

}

void ofApp::mouseMoved(int x, int y){

}

void ofApp::mouseDragged(int x, int y, int button){

}

void ofApp::mousePressed(int x, int y, int button){

}

void ofApp::mouseReleased(int x, int y, int button){

}

void ofApp::mouseEntered(int x, int y){

}

void ofApp::mouseExited(int x, int y){

}

void ofApp::windowResized(int w, int h){

}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){

}
