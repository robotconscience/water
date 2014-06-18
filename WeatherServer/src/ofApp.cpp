#include "ofApp.h"

using namespace lab;

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);
    
    width = 1024;
    height = 768;
    
    float scale = .75;
    
    // GLOBE
    sphere = rc::sphereNP2(ofGetWidth()/4.0, width * scale, height * scale, 30);
    
    light.enable();
    
    // FLUIDS
    
    // Initial Allocation
    //
    fluid.allocate(width, height, scale);
    
    // Seting the gravity set up & injecting the background image
    //
    fluid.dissipation = 0.99;
    fluid.velocityDissipation = 0.99;
    
    fluid.setGravity(ofVec2f(0.0,0.0));
    //    fluid.setGravity(ofVec2f(0.0,0.0098));
    
    //  Set obstacle
    //
    fluid.begin();
    ofClear(0,0);
    fluid.end();
    fluid.setUseObstacles(false);
    
    // Adding constant forces
    //
    fluid.addConstantForce(ofPoint(0.0,0.0), ofPoint(-.5,0), ofFloatColor(0.5,0.5,0.5), 100.f, 10.0, .02);
    fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(.5,-.1), ofFloatColor(0.5,0.5,0.5), 100.f, 10.0, .02);
    fluid.addConstantForce(ofPoint(width,height), ofPoint(.5,0), ofFloatColor(0.5,0.5,0.5), 100.f, 10.0, .02);
    
    ofSetWindowShape(width, height);
    bNeedToRead = true;
    
    // debug particle system...
    for ( int i=0; i<500; i++){
        Particle * particle = new Particle();
        particle->set(ofRandom(0,width*scale), ofRandom(0,height*scale));
        particle->radius = ofRandom(2,10);
        particle->damping = ofMap(particle->radius, 2, 10, .99, .8);
        files.addParticle(particle);
    }
    
    fileFBO.allocate(width * scale, height * scale);
    fileFBO.begin(); ofClear(0,0); fileFBO.end();
    
    files.setupSquare( ofVec2f(fileFBO.getWidth(), fileFBO.getHeight()) );
    files.setOption(ParticleSystem::HORIZONTAL_WRAP, true);
    files.setOption(ParticleSystem::VERTICAL_WRAP, true);
    files.setOption(ParticleSystem::DETECT_COLLISIONS, true);
    
    screenFBO.allocate(ofGetWidth(), ofGetHeight());
    
    // setup weather API
    weatherManager.setup();
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    
    // settings?
    rc::TextSettings serverSettings("serversettings.txt");
    options.port = ofToInt(serverSettings.getSetting("9000"));
    // anything else?
    cout << options.port << endl;
    
    websocketServer.setup(options);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // oscillate forces: reset
    fluid.getConstantForce(0).vel.set(.5,.1);
    fluid.getConstantForce(1).vel.set(.5,-.1);
    fluid.getConstantForce(2).vel.set(-.5,-.1);
    
    // oscillate forces: shift
    fluid.getConstantForce(0).vel += ofVec2f(ofSignedNoise(ofGetElapsedTimeMillis() * .0001) * 5.0,
                                             ofSignedNoise(ofGetElapsedTimeMillis() * .0002) * 2.0);
    fluid.getConstantForce(1).vel += ofVec2f(ofSignedNoise(ofGetElapsedTimeMillis() * .001) * 2.0,
                                             ofSignedNoise(ofGetElapsedTimeMillis() * .00002) * 2.0);
    fluid.getConstantForce(2).vel += ofVec2f(ofSignedNoise(ofGetElapsedTimeMillis() * .0005) * 5.0,
                                             ofSignedNoise(ofGetElapsedTimeMillis() * .00005) * 2.0);
    
    // Adding temporal Force
    //
    ofPoint m = ofPoint(mouseX,mouseY);
    ofPoint d = (m - oldM)*10.0;
    oldM = m;
    ofPoint c = ofPoint(640*0.5, 480*0.5) - m;
    c.normalize();
    fluid.addTemporalForce(m, d, ofFloatColor(0.5,0.5,0.5)*sin(ofGetElapsedTimef()),3.0f);
    
    //  Update
    //
    
    fluid.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    bNeedToRead = true;
    
    ParticleSystem::const_Iterator it = files.getParticles()->begin();
    
    for ( it; it != files.getParticles()->end(); ++it){
        it->second->acceleration += ofVec2f(getVelocity(it->second->x, it->second->y)) * .1;
    }
    files.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    ofDisableDepthTest();
    fluid.drawVelocity();
    
    fileFBO.begin();
    ofClear(0,0);
    ofDisableDepthTest();
    files.draw(ofRectangle(0,0,width,height));
    fileFBO.end();
    
    float rot = ofGetElapsedTimeMillis() * .01;
    
    screenFBO.begin();
        ofClear(0,0);
        ofSetColor(255);
        ofPushMatrix();
            ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
            ofRotateY(rot);
        
            ofEnableDepthTest();
            fluid.getTextureReference().bind();
            sphere.draw();
            fluid.getTextureReference().unbind();
        ofPopMatrix();
    
    screenFBO.end();
    
//    ofDisableDepthTest();
    ofEnableAlphaBlending();
    ofDisableDepthTest();
    ofSetColor(255);
    screenFBO.draw(0,0);
    
    screenFBO.begin();
    ofClear(0,0);
    ofSetColor(255);
    ofPushMatrix();
    
        ofEnableDepthTest();
        ofSetColor(255, 0);
        ofRect(0,0,width,height);
        ofSetColor(255);
        ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
        ofRotateY(rot);
    
        fileFBO.getTextureReference().bind();
        sphere.draw();
        fileFBO.getTextureReference().unbind();
    ofPopMatrix();
    screenFBO.end();
    
    
    ofDisableDepthTest();
    screenFBO.draw(0,0);
//    files.draw(ofRectangle(0,0,width,height));
}
//--------------------------------------------------------------
ofVec2f ofApp::getVelocityLatLong( float lat, float lon ){
    if ( bNeedToRead ){
        bNeedToRead = false;
        fluid.getVelocityBuffer()->readToPixels(velPix);
    }
    
    ofColor c = velPix.getColor(ofMap(lon, -180, 180, 0, velPix.getWidth()),
                                ofMap(lat, -90, 90, 0, velPix.getHeight()));
    return ofVec2f(ofMap(c.r,0,255,-1,1),
                   ofMap(c.g,0,255,-1,1));
}

//--------------------------------------------------------------
ofVec2f ofApp::getVelocity( float x, float y ){
    if ( bNeedToRead ){
        bNeedToRead = false;
        fluid.getVelocityBuffer()->readToPixels(velPix);
    }
    
    x = floor(ofWrap(x, 0, velPix.getWidth() ));
    y = floor(ofWrap(y, 0, velPix.getHeight() ));
    
    ofColor c = velPix.getColor(x,y);
    return ofVec2f(ofMap(c.r,0,255,-1,1, true),
                   ofMap(c.g,0,255,-1,1, true));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}