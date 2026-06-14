// AltAzMount by MegaSaturnv

/////////////////////////////
// Customizable Parameters //
/////////////////////////////
/* [Basic] */
AZ_GEAR_DIAMETER = 90;
ALT_GEAR_DIAMETER = 90;

GEAR_KEY_TOOTH_WIDTH = 6;
GEAR_KEY_TOOTH_LENGTH = 3;
GEAR_KEY_DEPTH = 10;

WORM_KEY_TOOTH_WIDTH = 3.2;
WORM_KEY_TOOTH_LENGTH = 1.2;
WORM_KEY_DEPTH = 22;

STEPPER_SHAFT_KEY_DEPTH = 1.6;
STEPPER_SHAFT_KEY_LENGTH = 10;

SHOW_SPUR = true;
SHOW_WORM_KEYED = true;
SHOW_WORM_X_AXIS = false; //Not currently used
SHOW_WORM_Y_AXIS = false; //Not currently used

SPLIT_WORM = false; //Optional. May help some with printing



/* [Advanced] */
//Use $fn = 24 if it's a preview. $fn = 96 for the render. Increase 96 to produce a smoother curve.
$fn = $preview ? 24 : 96;

//////////////
// Includes //
//////////////
use <gears.scad>



/////////////
// Modules //
/////////////
module gear_key(ArgGearKeyToothWidth=GEAR_KEY_TOOTH_WIDTH, ArgGearToothLength=GEAR_KEY_TOOTH_LENGTH, ArgGearKeyDepth=GEAR_KEY_DEPTH) {
	union() {
		translate([-ArgGearToothLength - ArgGearKeyToothWidth/2, -ArgGearKeyToothWidth/2, 0]) cube([2*ArgGearToothLength + ArgGearKeyToothWidth, ArgGearKeyToothWidth, ArgGearKeyDepth + 0.02]);
		translate([-ArgGearKeyToothWidth/2, -ArgGearToothLength - ArgGearKeyToothWidth/2, 0]) cube([ArgGearKeyToothWidth, 2*ArgGearToothLength + ArgGearKeyToothWidth, ArgGearKeyDepth + 0.02]);
	}
}

module worm_key(ArgGearKeyToothWidth=WORM_KEY_TOOTH_WIDTH, ArgGearToothLength=WORM_KEY_TOOTH_LENGTH, ArgGearKeyDepth=WORM_KEY_DEPTH) {
	union() {
		translate([-ArgGearToothLength - ArgGearKeyToothWidth/2, -ArgGearKeyToothWidth/2, 0]) cube([2*ArgGearToothLength + ArgGearKeyToothWidth, ArgGearKeyToothWidth, ArgGearKeyDepth + 0.02]);
		translate([-ArgGearKeyToothWidth/2, -ArgGearToothLength - ArgGearKeyToothWidth/2, 0]) cube([ArgGearKeyToothWidth, 2*ArgGearToothLength + ArgGearKeyToothWidth, ArgGearKeyDepth + 0.02]);
	}
}

module Main_Gears_Worm_x_axis() {
	worm_gear(modul=1, tooth_number=AZ_GEAR_DIAMETER, thread_starts=2, width=10, length=20, worm_bore=0, gear_bore=4, pressure_angle=20, lead_angle=9.5, optimized=true, together_built=true, show_spur=false, show_worm=true);

	translate([6, 23.5 + 11.15, 0]) difference() {
		rotate([90, 0, 0]) cylinder(d=6.4, h=23.5 + 1);
		translate([-6.4/2, -STEPPER_SHAFT_KEY_LENGTH, 6.4/2 - STEPPER_SHAFT_KEY_DEPTH]) cube([6.4, STEPPER_SHAFT_KEY_LENGTH, 6.4]);
	}

	translate([6, -8.8, 0]) rotate([90, 0, 0]) cylinder(d=10, h=13);
	translate([6, -8.8 + 1, 0]) rotate([90, 0, 0]) cylinder(d=8, h=13 + 8 + 1);
}

module Main_Gears_Worm_y_axis() {
	worm_gear(modul=1, tooth_number=ALT_GEAR_DIAMETER, thread_starts=2, width=10, length=20, worm_bore=0, gear_bore=4, pressure_angle=20, lead_angle=9.5, optimized=true, together_built=true, show_spur=false, show_worm=true);

	translate([6, 26 + 11.15, 0]) difference() {
		rotate([90, 0, 0]) cylinder(d=6.4, h=26 + 1);
		translate([-6.4/2, -STEPPER_SHAFT_KEY_LENGTH, 6.4/2 - STEPPER_SHAFT_KEY_DEPTH]) cube([6.4, STEPPER_SHAFT_KEY_LENGTH, 6.4]);
	}

	translate([6, -8.8, 0]) rotate([90, 0, 0]) cylinder(d=10, h=26);
	translate([6, -8.8 + 1, 0]) rotate([90, 0, 0]) cylinder(d=8, h=26 + 8 + 1);
}

module Main_Gears_Worm_keyed() {
	difference() {
		worm_gear(modul=1, tooth_number=ALT_GEAR_DIAMETER, thread_starts=2, width=10, length=20, worm_bore=0, gear_bore=4, pressure_angle=20, lead_angle=9.5, optimized=true, together_built=true, show_spur=false, show_worm=true);
		translate([6, -8.8 + 21, 0]) rotate([90, 0, 0]) worm_key();
	}
}


////////////////
// Main Model //
////////////////
//Spur
if (SHOW_SPUR) {
	difference() {
		union() {
			worm_gear(modul=1, tooth_number=AZ_GEAR_DIAMETER, thread_starts=2, width=10, length=20, worm_bore=4, gear_bore=4, pressure_angle=20, lead_angle=10, optimized=true, together_built=true, show_spur=true, show_worm=false);
			translate([-AZ_GEAR_DIAMETER/2, 0, -GEAR_KEY_DEPTH/2]) cylinder(d=24, h=GEAR_KEY_DEPTH);
		}
		translate([-AZ_GEAR_DIAMETER/2, 0, -5.01]) gear_key();
	}
}

//Worm X axis
if (SHOW_WORM_X_AXIS) {
	if (SPLIT_WORM) {
		//Half 1
		translate([-15, 0, 0]) difference() {
			Main_Gears_Worm_x_axis();
			translate([-200, -200, -100]) cube([400,400,100]);
		}
		//Half 2
		translate([15, 0, 0]) difference() {
			rotate([0, 180, 0]) Main_Gears_Worm_x_axis();
			translate([-200, -200, -100]) cube([400,400,100]);
		}

	} else {
		rotate([0, 0, 0]) Main_Gears_Worm_x_axis();
	}
}

//Worm Y axis
if (SHOW_WORM_Y_AXIS) {
	if (SPLIT_WORM) {
		//Half 1
		translate([-15, 0, 0]) difference() {
			Main_Gears_Worm_y_axis();
			translate([-200, -200, -100]) cube([400,400,100]);
		}
		//Half 2
		translate([15, 0, 0]) difference() {
			rotate([0, 180, 0]) Main_Gears_Worm_y_axis();
			translate([-200, -200, -100]) cube([400,400,100]);
		}

	} else {
		rotate([0, 0, 0]) Main_Gears_Worm_y_axis();
	}
}

//Worm keyed version
if (SHOW_WORM_KEYED) {
	if (SPLIT_WORM) {
		//Half 1
		translate([-15, 0, 0]) difference() {
			Main_Gears_Worm_keyed();
			translate([-200, -200, -100]) cube([400,400,100]);
		}
		//Half 2
		translate([15, 0, 0]) difference() {
			rotate([0, 180, 0]) Main_Gears_Worm_keyed();
			translate([-200, -200, -100]) cube([400,400,100]);
		}

	} else {
		rotate([0, 0, 0]) Main_Gears_Worm_keyed();
	}
}
