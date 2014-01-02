#include "ofApp.h"

int index = 0;

//--------------------------------------------------------------
void ofApp::setup(){
	gifloader.load("view.gif");
}

//--------------------------------------------------------------
void ofApp::update(){
	if (ofGetElapsedTimeMillis() % 3)
	{
		index++;
		if (index > gifloader.pages.size()-1) index = 0;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);

	gifloader.pages[index].draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 


}
