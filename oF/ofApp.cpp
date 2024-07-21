#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1620, 1080);
    
    // Load the videos
    ofVideoPlayer v1, v2, v3, v4, v5, v6;
    v1.load("video1.mp4");
    v2.load("video2.mp4");
    v3.load("video3.mp4");
    v4.load("video4.mp4");
    v5.load("video5.mp4");
    v6.load("video6.mp4");

    vVec.push_back(v1);
    vVec.push_back(v2);
    vVec.push_back(v3);
    vVec.push_back(v4);
    vVec.push_back(v5);
    vVec.push_back(v6);

    currentVideoIndex = 0;
    isVideoPlaying = false;
    
    // SERIAL
    serial.setup("/dev/tty.usbserial", 9600);
    ofLogNotice() << "Serial Setup";

    // Load background and object images
    for (int i = 0; i < 6; ++i) {
        backgroundImages[i].load("s" + ofToString(i + 1) + ".png");
        objectImages[i].load("t" + ofToString(i + 1) + ".png");
        objectCued[i] = false;
    }

    selectedObject = 0; // Default selection is 1 (index 0)

    ofLogNotice() << "Setup completed. Initial state.";
}

//--------------------------------------------------------------
void ofApp::update(){
    if (serial.available() > 0) {
        processSerialInput();
    }
    
    if (isVideoPlaying) {
        vVec[currentVideoIndex].update();
        if (vVec[currentVideoIndex].getPosition() >= 0.99) {  // Check if video is almost done
            ofLogNotice() << "Video " << currentVideoIndex + 1 << " done.";

            currentVideoIndex++; // Move to the next video
            if (currentVideoIndex < vVec.size()) {
                playNextVideo();
                isVideoPlaying = true;

                // Update the aspect ratio and size for the next video
                aspectRatio = vVec[currentVideoIndex].getWidth() / vVec[currentVideoIndex].getHeight();
                videoWidth = ofGetWidth();
                videoHeight = videoWidth / aspectRatio;
                ofSetWindowShape(videoWidth, videoHeight);

                // Debugging print
                ofLogNotice() << "Playing video " << currentVideoIndex + 1;
                
            } else { // If it's the last video
                ofLogNotice() << "All videos played. Resetting.";
                isVideoPlaying = false;
                currentVideoIndex = 0; // Reset for the next cycle
                // Optional: Automatically start over
//                playNextVideo();
//                isVideoPlaying = true;
                
                videoOrder.clear();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    // Draw the selected background image
    backgroundImages[selectedObject].draw(0, 0);

    // Draw object images with appropriate opacity
    for (int i = 0; i < 6; ++i) {
        if (i == selectedObject) {
            ofSetColor(255, 255); // Full opacity
        } else if (objectCued[i]) {
            ofSetColor(255, 128); // 50% opacity
        } else {
            ofSetColor(255, 77); // 30% opacity
        }
        objectImages[i].draw(0, 0);
    }

    ofSetColor(255); // Reset color

    if (isVideoPlaying) {
        // Draw the current video
        vVec[currentVideoIndex].draw(0, 0, videoWidth, videoHeight);
    }
}


//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    videoWidth = w;
    videoHeight = videoWidth / aspectRatio;
}

//--------------------------------------------------------------
void ofApp::playNextVideo(){
    vVec[currentVideoIndex].setLoopState(OF_LOOP_NONE); // Ensure looping is disabled
    vVec[currentVideoIndex].play();
    isVideoPlaying = true; // Set isVideoPlaying to true
    ofLogNotice() << "Playing video " << currentVideoIndex + 1; // Debugging print
}

//--------------------------------------------------------------
void ofApp::processSerialInput() {
    string serialData = "";
    while (serial.available() > 0) {
        char incomingByte = serial.readByte();
        if (incomingByte == '\n') {
            break;
        }
        serialData += incomingByte;
    }

    if (!serialData.empty()) {
        ofLogNotice() << "Received serial input: " << serialData;

        if (serialData == "1") {
            selectedObject = (selectedObject + 1) % 6;
        } else if (serialData == "2") {
            selectedObject = (selectedObject - 1 + 6) % 6;
        } else if (serialData == "0") {
            if (objectCued[selectedObject]) {
                // Remove from queue
                objectCued[selectedObject] = false;
                videoOrder.erase(std::remove(videoOrder.begin(), videoOrder.end(), selectedObject), videoOrder.end());
            } else {
                // Add to queue
                objectCued[selectedObject] = true;
                videoOrder.push_back(selectedObject);
            }
        }

        // Check if all objects are cued
        bool allCued = true;
        for (int i = 0; i < 6; ++i) {
            if (!objectCued[i]) {
                allCued = false;
                break;
            }
        }

        if (allCued && !isVideoPlaying) {
            currentVideoIndex = videoOrder[0];
            playNextVideo();
        }
    }
}