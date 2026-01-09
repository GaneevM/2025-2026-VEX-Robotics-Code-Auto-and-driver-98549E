/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	//pros::MotorGroup left_mg({1, -2, 3});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	//pros::MotorGroup right_mg({-4, 5, -6});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6
	pros::lcd::print(5, "Vert: %f", vertical_tracking.get_position());
	pros::lcd::print(6, "Horiz: %f", horizontal_tracking.get_position());
	// toggle for odom pneumatics
	bool odom_state = false;
	bool prevLeft = false;


	

	// intake toggle state: 0 = off, 1 = X-direction, 2 = Y-direction
	int intake_state = 0;
	bool prevX = false;
	bool prevY = false;

	// scoring toggle state: 0 = off, 1 = B-direction, 2 = A-direction
	int scoring_state = 0;
	bool prevB = false;
	bool prevA = false;

	// piston toggle state: false = retracted, true = deployed
	bool piston_state = false;
	bool prevUp = false;

	// descore toggle state: false = off, true = on
	bool descore_state = false;
	bool prevRight = false;

	//stopper toggle state: false = off, true = on
	bool stopper_state = false;
	bool prevDown = false;

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		int dir = master.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int turn = master.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		leftmotors.move(dir + turn);                      // Sets left motor voltage
		rightmotors.move(dir - turn);                     // Sets right motor voltage
		pros::delay(20);  
		
		// Run for 20 ms then update

				// Intake toggle control (press B or A to toggle intake directions) - swapped
		bool curB_btn = master.get_digital(pros::E_CONTROLLER_DIGITAL_B);
		bool curA_btn = master.get_digital(pros::E_CONTROLLER_DIGITAL_A);

		// Rising-edge detection for B (now controls intake)
		if (curB_btn && !prevB) {
			if (intake_state == 1) intake_state = 0; // toggle off
			else intake_state = 1; // set B-direction (maps to previous X behavior)
		}

		// Rising-edge detection for A (now controls intake)
		if (curA_btn && !prevA) {
			if (intake_state == 2) intake_state = 0; // toggle off
			else intake_state = 2; // set A-direction (maps to previous Y behavior)
		}

		// Apply motor outputs based on intake_state
		if (intake_state == 1) {
			Intake.move(-127);
			
		} else if (intake_state == 2) {
			Intake.move(127);	
		} else {
			Intake.move(0);
		}

		// save previous button states for intake (B/A)
		prevB = curB_btn;
		prevA = curA_btn;

		// Scoring toggle control (press X or Y to toggle scoring directions) - swapped
		bool curX_btn = master.get_digital(pros::E_CONTROLLER_DIGITAL_X);
		bool curY_btn = master.get_digital(pros::E_CONTROLLER_DIGITAL_Y);

		// Rising-edge detection for X (now controls scoring)
		if (curX_btn && !prevX) {
			if (scoring_state == 1) scoring_state = 0; // toggle off
			else scoring_state = 1; // set X-direction (maps to previous B behavior)
		}

		// Rising-edge detection for Y (now controls scoring)
		if (curY_btn && !prevY) {
			if (scoring_state == 2) scoring_state = 0; // toggle off
			else scoring_state = 2; // set Y-direction (maps to previous A behavior)
		}

		// Apply motor outputs based on scoring_state
		if (scoring_state == 1) {
			Scoring.move(127);
		} else if (scoring_state == 2) {
			Scoring.move(-127);
		} else {
			Scoring.move(0);
		}

		// save previous button states for scoring (X/Y)
		prevX = curX_btn;
		prevY = curY_btn;

		// piston toggle: press UP to toggle both pistons deploy/retract
		bool curUp = master.get_digital(pros::E_CONTROLLER_DIGITAL_UP);
		if (curUp && !prevUp) {
			piston_state = !piston_state;
			Piston1.set_value(piston_state);
			Piston2.set_value(piston_state);
		}
		prevUp = curUp;

		// Stopper toggle: press DOWN to toggle deploy/retract
		bool curDown = master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN);
		if (curDown && !prevDown) {
			stopper_state = !stopper_state;
			Stopper.set_value(stopper_state);
		}
		prevDown = curDown;

		// Descore toggle: press RIGHT to toggle Descore on/off
		bool curRight = master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT);
		if (curRight && !prevRight) {
			descore_state = !descore_state;
			Descore.set_value(descore_state);
		}
		prevRight = curRight;

			// give CPU time to other tasks
			pros::delay(10);
		}
}
