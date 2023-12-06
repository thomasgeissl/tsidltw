class Mark {
  float x,y;
  boolean moving = false;
  boolean neue = true;
  
  Mark(){
  }
  
  void reset(){
    neue = true;
  }
  
  void setFirst(float x_, float y_){
    x = x_;
    y = y_;
    neue = false;
  }
  
  boolean unset(){
    return neue;
  }
  
  boolean moves(){
    return moving;
  }
  
  void startMove(){
    moving = true;
  }
  
  void connect(float x_, float y_){
    
    stroke(255, 0, 255);
    blendMode(ADD);
    line(x * w, y * h, x_ * w, y_ * h);
    
    syphonOut.beginDraw();
    syphonOut.stroke(255, 0, 255);
    syphonOut.blendMode(ADD);
    syphonOut.line(x * syW, y * syH, x_ * syW, y_ * syH);
    syphonOut.endDraw();
    
    x = x_;
    y = y_;
    
    moving = false;
    
  }

}
