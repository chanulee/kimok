#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1920, 1080); // Set fixed window size
    
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

    // Load the BGM
    bgmPlayer.load("bgm.mp3");
    bgmPlayer.setLoop(true); // Loop the BGM

    // Setup serial communication
    serial.setup("/dev/tty.usbserial", 9600); // Adjust the port name as needed

    resetState(); // Initialize the state

    ofLogNotice() << "Setup completed. Initial state.";
}

//--------------------------------------------------------------
void ofApp::update(){
    if (isVideoPlaying) {
        vVec[currentVideoIndex].update();
        if (vVec[currentVideoIndex].getPosition() >= 0.99) {  // Check if video is almost done
            ofLogNotice() << "Video " << currentVideoIndex + 1 << " done.";
            skipToNextVideo();
        }
    }

    // Process serial input
    processSerialInput();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // Calculate the x offset to center the images
    float xOffset = (1920 - 1620) / 2;

    // Draw the selected background image
    backgroundImages[selectedObject].draw(xOffset, 0, 1620, 1080);

    // Draw object images with appropriate opacity
    for (int i = 0; i < 6; ++i) {
        if (objectCued[i]) {
            continue; // Skip drawing cued objects
        }
        if (i == selectedObject) {
            ofSetColor(255, 255); // Full opacity
        } else {
            ofSetColor(255, 77); // 30% opacity
        }
        objectImages[i].draw(xOffset, 0, 1620, 1080);
    }

    ofSetColor(255); // Reset color

    if (isVideoPlaying) {
        // Draw the current video
        vVec[currentVideoIndex].draw(0, 0, 1920, 1080);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_RIGHT) {
        do {
            selectedObject = (selectedObject + 1) % 6;
        } while (objectCued[selectedObject] && !allObjectsCued());
    } else if (key == OF_KEY_LEFT) {
        do {
            selectedObject = (selectedObject - 1 + 6) % 6;
        } while (objectCued[selectedObject] && !allObjectsCued());
    } else if (key == ' ') {
        handleInput("0");
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    // Do nothing to keep the window size fixed
}

//--------------------------------------------------------------
void ofApp::playNextVideo(){
    vVec[currentVideoIndex].setLoopState(OF_LOOP_NONE); // Ensure looping is disabled
    vVec[currentVideoIndex].setPosition(0); // Start from the beginning
    vVec[currentVideoIndex].play();
    isVideoPlaying = true; // Set isVideoPlaying to true

    ofLogNotice() << "Playing video " << currentVideoIndex + 1; // Debugging print
}

//--------------------------------------------------------------
void ofApp::skipToNextVideo() {
    currentVideoIndex++; // Move to the next video
    if (currentVideoIndex < videoOrder.size()) {
        playNextVideo();
    } else { // If it's the last video
        ofLogNotice() << "All videos played. Resetting.";
        isVideoPlaying = false;
        bgmPlayer.stop(); // Stop the BGM
        resetState(); // Reset to the initial state
    }
}

//--------------------------------------------------------------
bool ofApp::allObjectsCued() {
    for (int i = 0; i < 6; ++i) {
        if (!objectCued[i]) {
            return false;
        }
    }
    return true;
}

//--------------------------------------------------------------
void ofApp::resetState() {
    currentVideoIndex = 0;
    isVideoPlaying = false;
    videoOrder.clear();
    selectedObject = 0;

    for (int i = 0; i < 6; ++i) {
        objectCued[i] = false;
        backgroundImages[i].load("s" + ofToString(i + 1) + ".png");
        objectImages[i].load("t" + ofToString(i + 1) + ".png");
    }

    // Reset video positions
    for (auto& video : vVec) {
        video.setPosition(0);
    }

    ofLogNotice() << "State reset to initial.";
}

//--------------------------------------------------------------
void ofApp::processSerialInput() {
    if (serial.available() > 0) {
        char received = serial.readByte();
        if (received == '0') {
            handleInput("0");
        }
    }
}

//--------------------------------------------------------------
void ofApp::handleInput(const string& input) {
    if (input == "0") {
        if (isVideoPlaying) {
            skipToNextVideo();
        } else {
            if (objectCued[selectedObject]) {
                // Remove from queue
                objectCued[selectedObject] = false;
                videoOrder.erase(std::remove(videoOrder.begin(), videoOrder.end(), selectedObject), videoOrder.end());
            } else {
                // Add to queue
                objectCued[selectedObject] = true;
                videoOrder.push_back(selectedObject);

                // Move to the next un-cued object
                if (!allObjectsCued()) {
                    do {
                        selectedObject = (selectedObject + 1) % 6;
                    } while (objectCued[selectedObject]);
                }
            }
        }

        // Check if all objects are cued
        if (allObjectsCued() && !isVideoPlaying) {
            currentVideoIndex = videoOrder[0];
            playNextVideo();
            bgmPlayer.play(); // Start playing the BGM
        }
    }
}
