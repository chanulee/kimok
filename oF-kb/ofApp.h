#pragma once

#include "ofMain.h"

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
        void skipToNextVideo();
        bool allObjectsCued();
        void resetState();
        void processSerialInput();
        void handleInput(const string& input);

        vector<ofVideoPlayer> vVec;
        vector<int> videoOrder;
        int currentVideoIndex;
        bool isVideoPlaying;
        bool isVideoDone;

        float videoWidth;
        float videoHeight;
        float aspectRatio;

        int pointerIndex;
        bool videoSelected[3];
        bool playButtonVisible;
        ofRectangle playButtonRect;

        // New variables
        ofImage backgroundImages[6];
        ofImage objectImages[6];
        int selectedObject;
        bool objectCued[6];

        // BGM player
        ofSoundPlayer bgmPlayer;

        // Serial communication
        ofSerial serial;
};