/* //<>// //<>//
 TUIO 2 OSC 2023 Kevin Blackistone
 Only broadcasts x, y, velocity for cursors
 modified from
 TUIO 1.1 Demo for Processing
 Copyright (c) 2005-2014 Martin Kaltenbrunner <martin@tuio.org>
 
 */

import oscP5.*;
import netP5.*;
import codeanticode.syphon.*;


OscP5 oscP5;
NetAddress myRemoteLocation;
NetAddress myLocalLocation;
NetAddress pdLocation;
String IP = "127.0.0.1";
int Port = 1234;
int Port2 = 12000;
int pdPort = 13579;

int w = 720;
int h = 405;
int syW = 3840;
int syH = 2160;

PGraphics syphonOut;
SyphonServer server;

// pImage frame;


// import the TUIO library
import TUIO.*;
// declare a TuioProcessing client
TuioProcessing tuioClient;

// these are some helper variables which are used
// to create scalable graphical feedback
float cursor_size = 15;
float object_size = 60;
float table_size = 760;
float scale_factor = 1;
PFont font;

boolean verbose = false; // print console debug messages
boolean callback = true; // updates only after callbacks


boolean whipeScreen;


void setup()
{
  // GUI setup
  noCursor();
  size(720, 405, P2D);
  
  //smooth(8);
  
  noStroke();
  fill(0);

  syphonOut = createGraphics(syW, syH, P2D);
  syphonOut.beginDraw();
  syphonOut.background(0);
  syphonOut.endDraw();

  server = new SyphonServer(this, "ProcessingLayer");


  // periodic updates
  if (!callback) {
    frameRate(60);
    loop();
  } else noLoop(); // or callback updates

  
  font = createFont("Arial", 18);
  scale_factor = height/table_size;

  // finally we create an instance of the TuioProcessing client
  // since we add "this" class as an argument the TuioProcessing class expects
  // an implementation of the TUIO callback methods in this class (see below)
  tuioClient  = new TuioProcessing(this);

  oscP5 = new OscP5(this, Port2);
  myRemoteLocation = new NetAddress(IP, Port);
  myLocalLocation = new NetAddress(IP, Port2);
  pdLocation = new NetAddress(IP, pdPort);

  background(0);
}


ArrayList<Mark> marks = new ArrayList<Mark>();

// within the draw method we retrieve an ArrayList of type <TuioObject>, <TuioCursor> or <TuioBlob>
// from the TuioProcessing client and then loops over all lists to draw the graphical feedback.
void draw()
{

  float cur_size = cursor_size*scale_factor;

  ArrayList<TuioCursor> tuioCursorList = tuioClient.getTuioCursorList();
  while (tuioCursorList.size() > marks.size()) {
    marks.add(new Mark());
  }
  while (tuioCursorList.size() < marks.size()) {
    marks.remove(marks.size()-1);
  }
  for (int i=0; i<tuioCursorList.size(); i++) {
    TuioCursor tcur = tuioCursorList.get(i);
    // Mark mark = marks.get(i);
    ArrayList<TuioPoint> pointList = tcur.getPath();

    String OSC_Address = "/tracker" + i;
    float Velocity = sqrt(sq(tcur.getXSpeed()) + sq(tcur.getYSpeed()));


    // println(Velocity);
    if (marks.get(i).moves()) {
      if (Velocity < 0.05) {
        if (marks.get(i).unset()) marks.get(i).setFirst(tcur.getX(), tcur.getY());
        else marks.get(i).connect(tcur.getX(), tcur.getY());
      }
    } else if (Velocity > 0.1) {
      marks.get(i).startMove();
    }

    OscMessage myMessage = new OscMessage(OSC_Address);
    myMessage.add(tcur.getX());
    myMessage.add(tcur.getY());
    myMessage.add(Velocity);
    oscP5.send(myMessage, myRemoteLocation);
    oscP5.send(myMessage, myLocalLocation);
    oscP5.send(myMessage, pdLocation);
  }
  println(whipeScreen);
  if (whipeScreen){
    println("FOO");
    background(0);
    //rect(0,0,w,h);
    syphonOut.beginDraw();
    syphonOut.background(0);
    //syphonOut.rect(0,0,syW,syH);
    syphonOut.endDraw();
    // for (int i = 0; i < marks.size(); i++) marks.get(i).reset();
    whipeScreen = false;
  }
  server.sendImage(syphonOut);
}

void keyPressed() {

  if (key == 'X') {
    whipeScreen = true;
    println("DEEZ");
  }
}

/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());
}
