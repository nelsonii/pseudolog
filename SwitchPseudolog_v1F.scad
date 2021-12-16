include <BOSL/constants.scad>
use <BOSL/shapes.scad>

fileVersion = "1F";

caseWidthX = 50;
caseDepthY = 66;
caseHeightZ = 24;
wallThickness = 3;
overlap = 1; // overlap ensures that subtractions go beyond the edges

includeComponents = false; // optional component models, usually not included in print
  
mountingScrewDiameter = 2.0; // M2 Screw
mountingScrewHeadDiameter = 3.4; // M2 Phillips Head Screw
mountingScrewHeadHeight = 1.6; // M2 Phillips Head Screw

//joyType = "JST6";
//joyType = "DB9";
joyType = "JAC";

jackDiameter = 6.35;

emboss = true;
  
$fn=60; //circle smoothness
  
difference(){
  union() {
    difference(){
      //base
      //rounded cuboid is from the BOSL library
      //I'm using p1 setting to zero bottom (z). X/Y are centered on 0,0,0
      color("steelblue")
      cuboid([caseWidthX,caseDepthY,caseHeightZ], fillet=4, 
       p1=[-(caseWidthX/2), -(caseDepthY/2), 0]);
      //subtract out the inner cuboid
      cuboid([caseWidthX-wallThickness,caseDepthY-wallThickness,caseHeightZ-wallThickness], fillet=4, 
       p1=[-((caseWidthX-wallThickness)/2), -((caseDepthY-wallThickness)/2), (wallThickness/2)]);
      if (emboss) {
        //emboss version on inside case
        color("whitesmoke")
         translate([-5, -12, 1])
          linear_extrude(2)
           text(fileVersion, size=5);
        color("whitesmoke")
         translate([-4,-18, 1])
          linear_extrude(2)
           text(joyType, size=3);
        /*
        //emboss port info on outside of case
        color("whitesmoke")
         translate([-7, 25, caseHeightZ - 1]) 
          linear_extrude(2)
           text("USB", size=5);
        color("whitesmoke")
         translate([-20, -28, caseHeightZ - 1]) 
          linear_extrude(2)
           text("U    D    L    R", size=5);
        */
      }
    }//difference (base)

    //riser for micro pro board
    color("purple")
     translate([-5,-2,0])
      cube([10, 30, 5]);
   
    if (joyType=="JST6") {
    //riser for joystick connector (additional support)
    color("darkmagenta")
    translate([-((caseWidthX-wallThickness)/2), 5, 1]) 
        cube([7, 17.5, 4]);
    }
    
    if (joyType=="JAC") {
    //riser for slide pot (additional support)
    color("hotpink")
    translate([-18, -12.5, caseHeightZ-wallThickness-4]) 
        cube([36, 8.25, 6]);
    }
    
    //bumper for micro pro board -- endstop
    color("violet")
     translate([-4.5, -4, 0])  
      cube([9, 2, 7]);
    //bumpers for micro pro board -- lower side
    color("violet")
     translate([-11.25, -2, 0]) 
      cube([2,5,7]);
    color("violet")
     translate([9.25, -2, 0]) 
      cube([2,5,7]);
    //bumpers for micro pro board -- upper side
    color("violet")
     translate([-11.25, 27, 1.5]) 
      cube([2,5,5.5]);
    color("violet")
     translate([9.25, 27, 1.5]) 
      cube([2,5,5.5]);
    
    //case screw mounts
    color("gold")
     translate([-16-2.5, -2.5, 0])
      cube([5,5,caseHeightZ]);
    color("gold")
     translate([+16-2.5, -2.5, 0])
      cube([5,5,caseHeightZ]);

    if (includeComponents) {
      if (joyType=="JST6") {
        //model of connector to joystick
        color("tan")
         translate([-10, -5, 6]) 
          cube([6,18,10]);
      }        
    }//includeComponents
    
  }//union
  
  // Start of Difference stuff ************************************************************

  //USB Micro cable outlet -- chunky cord is 11mm wide and 7mm high.
  color("coral")
   translate([-5.5,30,5.75])
    cube([11, (wallThickness*2), 5]);


  if (joyType=="JST6") {
    //joystick connector
    color("darkorange")
    translate([-((caseWidthX+wallThickness)/2), 5, 5.1]) 
          cube([wallThickness*2, 17.5, 7.0]); 
  }
  if (joyType=="DB9") {
    //joystick connector
    color("darkorange")
    translate([-19.2/2, -((caseDepthY+wallThickness)/2), 5.1]) 
          cube([19.2, wallThickness*2, 10.7]); 
  }


  if (joyType=="JAC") {
    //four switch joystick connection
    color("darkorange")
     translate([-15, -((caseDepthY+wallThickness)/2), 9]) 
      rotate([-90,0,0])
          cylinder(h=wallThickness*2, d=jackDiameter); 
    color("darkorange")
     translate([+15, -((caseDepthY+wallThickness)/2), 9]) 
      rotate([-90,0,0])
          cylinder(h=wallThickness*2, d=jackDiameter); 
    color("darkorange")
     translate([-5, -((caseDepthY+wallThickness)/2), 9]) 
      rotate([-90,0,0])
          cylinder(h=wallThickness*2, d=jackDiameter); 
    color("darkorange")
     translate([+5, -((caseDepthY+wallThickness)/2), 9]) 
      rotate([-90,0,0])
          cylinder(h=wallThickness*2, d=jackDiameter); 
  }      

if (joyType=="JAC") {
  //slide pot cutout
  color("pink")
   translate([-13, -10, caseHeightZ-wallThickness-5])
    cube([26, 3, wallThickness+overlap+5]);
  //mounting screws
  color("deeppink")
   translate([-15.565, -8.5, caseHeightZ-wallThickness-5])
    cylinder(h=wallThickness+overlap+5, d=2);
  color("deeppink")
   translate([+15.565, -8.5, caseHeightZ-wallThickness-5])
    cylinder(h=wallThickness+overlap+5, d=2);
  //countersinks
  color("hotpink")
   translate([-15.565, -8.5, caseHeightZ-1.5])
    cylinder(h=2+overlap, d=3.4);
  color("hotpink")
   translate([+15.565, -8.5, caseHeightZ-1.5])
    cylinder(h=2+overlap, d=3.4);
}


  //screw hole for case (goes through top of case)
  color("tomato")
   translate([-16, 0, wallThickness])
    cylinder(h=caseHeightZ+overlap, d=mountingScrewDiameter);
  color("tomato")
   translate([+16, -0, wallThickness])
    cylinder(h=caseHeightZ+overlap, d=mountingScrewDiameter);
  
  //countersink
  color("darkorange")
   translate([-16, 0, caseHeightZ-(mountingScrewHeadHeight/2)])
    cylinder(h=mountingScrewHeadHeight+overlap, d=mountingScrewHeadDiameter);
  color("darkorange")
   translate([+16, -0, caseHeightZ-(mountingScrewHeadHeight/2)])  
    cylinder(h=mountingScrewHeadHeight+overlap, d=mountingScrewHeadDiameter);
 
  //-------------------------------------------------------------------------------------------------------

  splitAtZ=15; //actual top of usb opening is 10.75
  
  //remove the top for split
  color("crimson")  translate([-((caseWidthX+overlap)/2),-((caseDepthY+overlap)/2),splitAtZ])  cube([caseWidthX+overlap, caseDepthY+overlap, caseHeightZ+overlap]);
  // ******* OR ******
  //Remove everything but the top
  //color("crimson") translate([-((caseWidthX+overlap)/2),-((caseDepthY+overlap)/2),splitAtZ-caseHeightZ])  cube([caseWidthX+overlap, caseDepthY+overlap, caseHeightZ+overlap]);

  //-------------------------------------------------------------------------------------------------------
  
  
}//difference
  