#pragma once

#include "ofMain.h"
#include "ofSerial.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;

		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;

		
    
    void playNextVideo();
    void processSerialInput();
    

        vector<ofVideoPlayer> vVec;
        vector<int> videoOrder;
        int currentVideoIndex;
        bool isVideoPlaying;
        bool isVideoDone;

        float videoWidth;
        float videoHeight;
        float aspectRatio;

        ofSerial serial;

        int pointerIndex;
        bool videoSelected[3];
        bool playButtonVisible;
        ofRectangle playButtonRect;

        // New variables
        ofImage backgroundImages[6];
        ofImage objectImages[6];
        int selectedObject;
        bool objectCued[6];
};