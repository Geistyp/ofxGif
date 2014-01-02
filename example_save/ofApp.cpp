#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	gifsaver.create("test.gif");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);

	ofNoFill();
	ofCircle(ofGetMouseX(), ofGetMouseY(), 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') gifsaver.save();
	if (key == 'a')
	{
		ofImage img;
		img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		gifsaver.append(img.getPixelsRef());
	}
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

	if (dragInfo.files.size() > 0)
	{
		for (int i = 0; i < dragInfo.files.size(); i++ )
		{
			gifsaver.append(dragInfo.files[i]);
		}
		
	}

}
