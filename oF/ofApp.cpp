#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1920, 1080);
    
    // Load the videos
    ofVideoPlayer v1, v2, v3;
    v1.load("video1.mp4");
    v2.load("video2.mp4");
    v3.load("video3.mp4");

    vVec.push_back(v1);
    vVec.push_back(v2);
    vVec.push_back(v3);

    currentVideoIndex = 0;
    isVideoPlaying = false;
    
    // SERIAL
    serial.setup("/dev/tty.usbserial", 9600);
    ofLogNotice() << "Serial Setup";

    // Start playing the first video
//    playNextVideo();
//    isVideoPlaying = true;

    // Calculate the aspect ratio based on the first video's dimensions
//    aspectRatio = vVec[currentVideoIndex].getWidth() / vVec[currentVideoIndex].getHeight();
//    videoWidth = ofGetWidth();
//    videoHeight = videoWidth / aspectRatio;
//    ofSetWindowShape(videoWidth, videoHeight);
    
    // Initialize pointers and selection states
    pointerIndex = 0;
    std::fill(std::begin(videoSelected), std::end(videoSelected), false);
    playButtonVisible = false;
    playButtonRect.set(840, 900, 240, 60); // Position and size of the play button
    
    ofLogNotice() << "Setup completed. Playing first video.";

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
    if (!isVideoPlaying) {
        // Draw thumbnails
        float thumbnailWidth = 320;
        float thumbnailHeight = 320;
        for (int i = 0; i < vVec.size(); ++i) {
            float x = i * (thumbnailWidth + 40);
            float y = 200;
            vVec[i].draw(x, y, thumbnailWidth, thumbnailHeight);

            if (videoSelected[i]) {
                ofSetColor(255, 165, 0, 100); // Orange tint for selected thumbnails
                ofDrawRectangle(x, y, thumbnailWidth, thumbnailHeight);
                ofSetColor(255); // Reset color
            }

            if (i == pointerIndex) {
                ofNoFill();
                ofSetColor(255, 165, 0);
                ofSetLineWidth(5);
                ofDrawRectangle(x, y, thumbnailWidth, thumbnailHeight);
                ofFill();
                ofSetColor(255); // Reset color
            }
        }

        if (playButtonVisible) {
            ofSetColor(255, 165, 0);
            ofDrawRectangle(playButtonRect);
            ofSetColor(0);
            ofDrawBitmapString("PLAY", playButtonRect.x + 80, playButtonRect.y + 40);
            ofSetColor(255); // Reset color

            if (pointerIndex == 3) {
                ofNoFill();
                ofSetColor(255, 165, 0);
                ofSetLineWidth(5);
                ofDrawRectangle(playButtonRect);
                ofFill();
                ofSetColor(255); // Reset color
            }
        }
    } else {
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

        if (serialData == "4") {
            movePointerRight();
        } else if (serialData == "5") {
            movePointerLeft();
        } else if (serialData == "Button Pressed") {
            selectItem();
        }
    }
}

//--------------------------------------------------------------
void ofApp::movePointerRight() {
    if (playButtonVisible) {
        pointerIndex = (pointerIndex + 1) % 4;
    } else {
        pointerIndex = (pointerIndex + 1) % 3;
    }
    ofLogNotice() << "Pointer moved to index: " << pointerIndex;
}

//--------------------------------------------------------------
void ofApp::movePointerLeft() {
    if (playButtonVisible) {
        pointerIndex = (pointerIndex - 1 + 4) % 4;
    } else {
        pointerIndex = (pointerIndex - 1 + 3) % 3;
    }
    ofLogNotice() << "Pointer moved to index: " << pointerIndex;
}

//--------------------------------------------------------------
void ofApp::selectItem() {
    if (pointerIndex < 3) {
        videoSelected[pointerIndex] = true;
        videoOrder.push_back(pointerIndex + 1); // Store video order based on selection
        ofLogNotice() << "Video " << pointerIndex + 1 << " selected";

        // Check if all videos are selected
        bool allSelected = true;
        for (int i = 0; i < 3; ++i) {
            if (!videoSelected[i]) {
                allSelected = false;
                break;
            }
        }

        if (allSelected) {
            playButtonVisible = true;
        }
    } else if (pointerIndex == 3 && playButtonVisible) {
        currentVideoIndex = videoOrder[0] - 1;
        playNextVideo();
        isVideoPlaying = true;
        playButtonVisible = false;
        ofLogNotice() << "Play button pressed. Starting video sequence.";
    }
}
