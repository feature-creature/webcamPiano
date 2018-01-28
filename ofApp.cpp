#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // initialize app settings 
	ofBackground(125);
	video.setDeviceID(0);
    video.setDesiredFrameRate(60);
    video.initGrabber(640, 480);
    debug = true;

    // initialize particle system
    partSys.setupAsGrid(10,2,150);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // ?? differences?
    // ofxVideoGrabber
    // ofxCvColorImage
    // ofxCvGrayscaleImage
    // ofxCvFloatImage
    // ofImage


    //decode the new frame
	video.update();
    
	//if new frame was obtained
	if ( video.isFrameNew() ) {
        
		//if the last 'current' frame is still stored,
        //now store it as the 'previous' frame
		if ( grayImage.bAllocated ) {
			grayImagePrev = grayImage;
		}

		//store the new current frame
        //?? and convert to grayscale how
		image.setFromPixels( video.getPixelsRef() );
		image.mirror(false, true);
		grayImage = image; 

		//if the previous frame is stored
        //?? why not combine this with the first step
		if ( grayImagePrev.bAllocated ) {
            
			//Get absolute difference
            //between current frame and previous frame 
            //use absolute diff for only positive values
			diff.absDiff( grayImage, grayImagePrev );

            //to increase the motion's visibility
            //amplify our absolute difference by
            //converting it to a float image
            //and applying a multiplication factor to its pixel values
			diffFloat = diff;
			diffFloat *= 5.0;	//original = 5.0

			//Update the accumulation buffer
            //if the buffer (?) is not initialized
			if ( !bufferFloat.bAllocated ) {
				//set it equal to diffFloat
				bufferFloat = diffFloat;
			}
			else {
				//Slow damping the buffer to zero
                //?? damping
				bufferFloat *= 0.65; //original = 0.85

				//Add current difference image to the buffer
				bufferFloat += diffFloat;

                // decrease noise (unnecessary)
				bufferFloat.erode();
			}
		}
	}
    // update particle system
    partSys.update();

}


//--------------------------------------------------------------
void ofApp::draw(){
    // set default background color
	ofBackground( 255, 255, 255 );

    //so only draw if more than two video frames are stored 
	//you need 2 frames for calculating frame differencing
	if ( diffFloat.bAllocated ) {
		//Get image dimensions
		int w = grayImage.width;
		int h = grayImage.height;

        //Set color for images drawing
        ofSetColor( 255, 255, 255 );

        //if in debug mode,
        //Draw standrd image with additional images in a grid for anaylsis
        //grayImage
        //diffFloat
        //bufferFloat
        if (debug)
        {
            grayImage.draw( 0, 0, w/2, h/2 );
            diffFloat.draw( w/2, 0, w/2, h/2 );
            bufferFloat.draw( 0, h/2, w/2, h/2 );
        }
        else{
            image.draw( 0, 0, ofGetWidth(), ofGetHeight() );
        }

        //if in debug mode,
        //Draw the image motion areas
        //?? combine with previous if statement
        if (debug)
        {
            //Shift and scale the coordinate system
            ofPushMatrix();
            ofTranslate( w/2, h/2);
            ofScale( 0.5, 0.5 );
            //Draw bounding rectangle
            ofSetColor(0, 0, 0);
            ofNoFill();

            //?? no ofPopMatrix();
            //perhaps ends within scope
        }

        //locally store bufferFloat pixels
        //?? what does the * symbolize
        float *pixels = bufferFloat.getPixelsAsFloats();

        //Iterate through all pixels
        //columns by rows
        for (int y=0; y<h; y+=1)
        {
            for (int x=0; x<w; x+=1)
            {
                //Get the pixel value
                float value = pixels[ x + w * y ];
                //If value exceed threshold, then draw pixel
                //original threshold value of 0.9
                if ( value >= 1.4 ) { 
                    if(debug){
                        ofPushStyle();
                        ofNoFill();
                        ofDrawEllipse(x,y,15,15);
                        ofPopStyle();
                    }else{
                        //EDIT HERE: this is where you'll be activating
                        //the grid locations or generating particles
                        //the debug mode draws small black points in the bottom right square
                        partSys.activateParticle(x,y);


                    }
                }
            }
        }
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    debug = !debug;
}
