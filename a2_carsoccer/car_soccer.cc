/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"
#include <iostream>
#include <vector>
#include <math.h>

#define PI 3.14159265

CarSoccer::CarSoccer() : GraphicsApp(1024,768, "Car Soccer") {
    // Define a search path for finding data files (images and shaders)
    searchPath_.push_back(".");
    searchPath_.push_back("./data");
    searchPath_.push_back(DATA_DIR_INSTALL);
    searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer() {
}


Vector2 CarSoccer::joystick_direction() {
    Vector2 dir;
    if (IsKeyDown(GLFW_KEY_LEFT))
        dir[0]--;
    if (IsKeyDown(GLFW_KEY_RIGHT))
        dir[0]++;
    if (IsKeyDown(GLFW_KEY_UP))
        dir[1]++;
    if (IsKeyDown(GLFW_KEY_DOWN))
        dir[1]--;
    return dir;
}


void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    if (key == GLFW_KEY_SPACE) {
        // Here's where you could call some form of launch_ball();
        Vector3 v = Vector3(0, 0, 15);
        ball_.set_velocity(v);
    }
}



void CarSoccer::UpdateSimulation(double timeStep) {
    // Here's where you shound do your "simulation", updating the positions of the
    // car and ball as needed and checking for collisions.  Filling this routine
    // in is the main part of the assignment.
    
    //get the direction of the car
    Vector2 dir = joystick_direction();
    float turnRate = dir[0];
    
    //get the current car values
    Point3 car_pos = car_.position();
    float car_speed = car_.speed();
    float thrust = car_.get_thrust();
    float car_angle = car_.angle();
    float car_r = car_.collision_radius();
    
    //set the new car values for the car movement
    car_speed = car_speed + (thrust * dir[1] - 4 * car_speed) * timeStep;
    car_angle = car_angle + turnRate * car_speed * timeStep * 4;
    float turn_x = sin(car_angle) * car_speed;
    float turn_z = cos(car_angle) * car_speed;
    Vector3 v_car = Vector3(turn_x, 0, turn_z);
    car_pos = car_pos + v_car;
    
    //set the boundary for the car
    if (car_pos.x() + car_r > 40) {
        car_pos[0] = 40 - car_r;
    }
    if (car_pos.x() - car_r < -40) {
        car_pos[0] = -40 + car_r;
    }
    if (car_pos.z() + car_r > 50) {
        car_pos[2] = 50 - car_r;
    }
    if (car_pos.z() - car_r < -50) {
        car_pos[2] = -50 + car_r;
    }
    
    car_.set_position(car_pos);
    car_.set_angle(car_angle);
    car_.set_speed(car_speed);
    
    //ball movement
    Vector3 gravity(0, 30, 0);
    ball_.set_position(ball_.position() + ball_.velocity() * timeStep);
    if (ball_.position().y() > ball_.radius()) {
        ball_.set_velocity(ball_.velocity() - gravity * timeStep);
    }
    
    Point3 ball_pos = ball_.position();
    Vector3 v_ball = ball_.velocity();
    float ball_r = ball_.radius();
    
    
    float d = car_r + ball_r;
    Vector3 balltocar = ball_pos - car_.position();
    float real_d = balltocar.Length();
    
    // if the distance between the ball and the car is less than or equal to the
    // sum of the raidous of the car and the radious of the ball, update the ball's
    // position and the ball's velocity
    if (real_d <= d) {
        ball_pos = ball_pos + balltocar.ToUnit() * (ball_r + car_r - real_d);
        v_ball = v_ball * 0.8;
        Vector3 v_rel = v_ball- v_car; // the relaity velocity
        v_rel = v_rel - 2 * v_rel.Dot(balltocar.ToUnit()) * balltocar.ToUnit();
        v_ball = v_car + v_rel;
    }
    
    // let the ball bonce if it hit the boundary of the fild
    if (ball_pos.x() + ball_r > 40) {
        ball_pos[0] = 40 - ball_r;
        v_ball[0] = v_ball[0]*(-0.8);
    }
    if (ball_pos.x() - ball_r < -40) {
        ball_pos[0] = -40 + ball_r;
        v_ball[0] = v_ball[0]*(-0.8);
    }
    if (ball_pos.y() + ball_r > 35) {
        ball_pos[1] = 35 - ball_r;
        v_ball[1] = v_ball[1]*(-0.8);
    }
    if (ball_pos.y() - ball_r < 0) {
        ball_pos[1] = ball_r;
        v_ball[1] = v_ball[1]*(-0.8);
    }
    if (ball_pos.z() + ball_r > 50) {
        ball_pos[2] = 50 - ball_r;
        v_ball[2] = v_ball[2]*(-0.8);
    }
    if (ball_pos.z() - ball_r < -50) {
        ball_pos[2] = -50 + ball_r;
        v_ball[2] = v_ball[2]*(-0.8);
    }
    ball_.set_velocity(v_ball);
    ball_.set_position(ball_pos + v_ball * timeStep);
    
    // if the ball hit the goal on either side, reset the ball and the car
    if (ball_pos.z() + ball_r == 50 && ball_pos.x() >= -10 && ball_pos.x() <= 10 && ball_pos.y() >= 0 && ball_pos.y() <= 10){
        ball_.Reset();
        car_.Reset();
    }
    if (ball_pos.z() - ball_r == -50 && ball_pos.x() >= -10 && ball_pos.x() <= 10 && ball_pos.y() >= 0 && ball_pos.y() <= 10){
        ball_.Reset();
        car_.Reset();
    }
}


void CarSoccer::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
    viewMatrix_ = Matrix4::LookAt(Point3(0,60,70), Point3(0,0,10), Vector3(0,1,0));
 
    // Set a background color for the screen
    glClearColor(0.8,0.8,0.8, 1);
    
    // Load some image files we'll use
    fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
    crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}


void CarSoccer::DrawUsingOpenGL() {
    // Draw the crowd as a fullscreen background image
    quickShapes_.DrawFullscreenTexture(Color(1,1,1), crowdTex_);
    
    // Draw the field with the field texture on it.
    Color col(16.0/255.0, 46.0/255.0, 9.0/255.0);
    Matrix4 M = Matrix4::Translation(Vector3(0,-0.201,0)) * Matrix4::Scale(Vector3(50, 1, 60));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
    M = Matrix4::Translation(Vector3(0,-0.2,0)) * Matrix4::Scale(Vector3(40, 1, 50));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1,1,1), fieldTex_);
    
    // Draw the car
    Color carcol(0.8, 0.2, 0.2);
    Matrix4 Mcar =
        Matrix4::Translation(car_.position() - Point3(0,0,0)) *
        Matrix4::RotationY(car_.angle()) *
        Matrix4::Scale(car_.size()) *
        Matrix4::Scale(Vector3(0.5,0.5,0.5));
    quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);
    
    
    // Draw the ball
    Color ballcol(1,1,1);
    Matrix4 Mball =
        Matrix4::Translation(ball_.position() - Point3(0,0,0)) *
        Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
    quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);
    
    
    // Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
    // flattens the sphere into a pancake, which we then draw just a bit
    // above the ground plane.
    Color shadowcol(0.2,0.4,0.15);
    Matrix4 Mshadow =
        Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
        Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
        Matrix4::RotationX(90);
    quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);
    
    
    // You should add drawing the goals and the boundary of the playing area
    // using quickShapes_.DrawLines()
    Matrix4 Mline;
    // draw the bpundary of the playing area
    Color boundarycol(1, 1, 1);
    std::vector<Point3> boundary1;
    boundary1.push_back(Point3(-40, 0, -50));
    boundary1.push_back(Point3(-40, 35, -50));
    boundary1.push_back(Point3(40, 35, -50));
    boundary1.push_back(Point3(40, 0, -50));
    std::vector<Point3> boundary2;
    boundary2.push_back(Point3(40, 35, -50));
    boundary2.push_back(Point3(40, 35, 50));
    boundary2.push_back(Point3(40, 0, 50));
    std::vector<Point3> boundary3;
    boundary3.push_back(Point3(40, 35, 50));
    boundary3.push_back(Point3(-40, 35, 50));
    boundary3.push_back(Point3(-40, 0, 50));
    std::vector<Point3> boundary4;
    boundary4.push_back(Point3(-40, 35, 50));
    boundary4.push_back(Point3(-40, 35, -50));
    
    quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, boundarycol, boundary1, QuickShapes::LinesType::LINE_STRIP, 0.1);
    quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, boundarycol, boundary2, QuickShapes::LinesType::LINE_STRIP, 0.1);
    quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, boundarycol, boundary3, QuickShapes::LinesType::LINE_STRIP, 0.1);
    quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, boundarycol, boundary4, QuickShapes::LinesType::LINE_STRIP, 0.1);
    
    //draw the goals
    Color goalcol_1(1,0.8,0.2);
    std::vector<Point3> goal1;
    for (int i = 0; i <= 10; i++) {
        goal1.push_back(Point3(-10, i, 50));
        goal1.push_back(Point3(10, i, 50));
        quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, goalcol_1, goal1, QuickShapes::LinesType::LINES, 0.05);
        
    }
    for (int j = 0; j <= 20; j++) {
        goal1.push_back(Point3(-10+j, 0, 50));
        goal1.push_back(Point3(-10+j, 10, 50));
        quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, goalcol_1, goal1, QuickShapes::LinesType::LINES, 0.05);
    }
    
    Color goalcol_2(0.5, 0.3, 0.8);
    std::vector<Point3> goal2;
    for (int i = 0; i <= 10; i++) {
        goal2.push_back(Point3(-10, i, -50));
        goal2.push_back(Point3(10, i, -50));
        quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, goalcol_2, goal2, QuickShapes::LinesType::LINES, 0.05);
        
    }
    for (int j = 0; j <= 20; j++) {
        goal2.push_back(Point3(-10+j, 0, -50));
        goal2.push_back(Point3(-10+j, 10, -50));
        quickShapes_.DrawLines(modelMatrix_ * Mline, viewMatrix_, projMatrix_, goalcol_2, goal2, QuickShapes::LinesType::LINES, 0.05);
    }
}
