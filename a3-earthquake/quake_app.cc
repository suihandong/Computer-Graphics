/** CSci-4611 Assignment 3:  Earthquake
 */

#include "quake_app.h"
#include "config.h"

#include <iostream>
#include <sstream>

// Number of seconds in 1 year (approx.)
const int PLAYBACK_WINDOW = 12 * 28 * 24 * 60 * 60;

using namespace std;

QuakeApp::QuakeApp() : GraphicsApp(1280,720, "Earthquake"),
    playback_scale_(15000000.0), debug_mode_(false), globe_mode_(false)
{
    // Define a search path for finding data files (images and earthquake db)
    search_path_.push_back(".");
    search_path_.push_back("./data");
    search_path_.push_back(DATA_DIR_INSTALL);
    search_path_.push_back(DATA_DIR_BUILD);
    
    quake_db_ = EarthquakeDatabase(Platform::FindFile("earthquakes.txt", search_path_));
    current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();

 }


QuakeApp::~QuakeApp() {
}


void QuakeApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window *window = new nanogui::Window(screen(), "Earthquake Controls");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(400,200));
    window->setLayout(new nanogui::GroupLayout());
    
    date_label_ = new nanogui::Label(window, "Current Date: MM/DD/YYYY", "sans-bold");
    
    globe_btn_ = new nanogui::Button(window, "Globe");
    globe_btn_->setCallback(std::bind(&QuakeApp::OnGlobeBtnPressed, this));
    globe_btn_->setTooltip("Toggle between map and globe.");
    
    new nanogui::Label(window, "Playback Speed", "sans-bold");
    
    nanogui::Widget *panel = new nanogui::Widget(window);
    panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                            nanogui::Alignment::Middle, 0, 20));
    
    nanogui::Slider *slider = new nanogui::Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(120);
    
    speed_box_ = new nanogui::TextBox(panel);
    speed_box_->setFixedSize(Eigen::Vector2i(60, 25));
    speed_box_->setValue("50");
    speed_box_->setUnits("%");
    slider->setCallback(std::bind(&QuakeApp::OnSliderUpdate, this, std::placeholders::_1));
    speed_box_->setFixedSize(Eigen::Vector2i(60,25));
    speed_box_->setFontSize(20);
    speed_box_->setAlignment(nanogui::TextBox::Alignment::Right);
    
    nanogui::Button* debug_btn = new nanogui::Button(window, "Toggle Debug Mode");
    debug_btn->setCallback(std::bind(&QuakeApp::OnDebugBtnPressed, this));
    debug_btn->setTooltip("Toggle displaying mesh triangles and normals (can be slow)");
    
    screen()->performLayout();
}

void QuakeApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    // Optional: In our demo, we adjust the tilt of the globe here when the
    // mouse is dragged up/down on the screen.
}


void QuakeApp::OnGlobeBtnPressed() {
    // TODO: This is where you can switch between flat earth mode and globe mode
    globe_mode_ = !globe_mode_;
}

void QuakeApp::OnDebugBtnPressed() {
    debug_mode_ = !debug_mode_;
}

void QuakeApp::OnSliderUpdate(float value) {
    speed_box_->setValue(std::to_string((int) (value * 100)));
    playback_scale_ = 30000000.0*value;
}


void QuakeApp::UpdateSimulation(double dt)  {
    // Advance the current time and loop back to the start if time is past the last earthquake
    current_time_ += playback_scale_ * dt;
    if (current_time_ > quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
    }
    if (current_time_ < quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds();
    }
    
    Date d(current_time_);
    stringstream s;
    s << "Current date: " << d.month()
        << "/" << d.day()
        << "/" << d.year();
    date_label_->setCaption(s.str());
    
    if (globe_mode_) {
        rotation += 0.5;
        if (alpha < 1) {
            alpha += 0.05;
        }
    } else {
        if (alpha > 0) {
            alpha -= 0.05;
        }
        rotation = 0;
    }
    
    earth_.Globe(alpha);
    
    // TODO: Any animation, morphing, rotation of the earth, or other things that should
    // be updated once each frame would go here.
    // update the earthquake information
    // decide the size and the color of the earthquake
    for (int i = 0; i < PLAYBACK_WINDOW; i += 24 * 60 * 60) {
        Date cur_time(current_time_ + i);
        int idx = quake_db_.FindMostRecentQuake(cur_time);
        double eq_lat = quake_db_.earthquake(idx).latitude();
        double eq_lon = quake_db_.earthquake(idx).longitude();
        Point3 p = earth_.LatLongToPlane(eq_lat, eq_lon);
        plane_position_list.push_back(p);
        Point3 p1 = earth_.LatLongToSphere(eq_lat, eq_lon);
        sphere_position_list.push_back(p1);

        Color col = color(quake_db_.earthquake(idx).magnitude());
        float r = radious(quake_db_.earthquake(idx).magnitude());
        color_list.push_back(col);
        radious_list.push_back(r);
    }
}

Color QuakeApp::color(double mag) {
    double max_mag = quake_db_.max_magnitude();
    double min_mag = quake_db_.min_magnitude();
    double step = (max_mag - min_mag)/10;
    Color c;
    for (int i = 0; i < 10; i++) {
        if (mag >= min_mag + step*i && mag < min_mag + step *(i+1)) {
            c = Color(1, 1-(float)i/10, 0);
        }
    }
    return c;
}

float QuakeApp::radious(double mag) {
    double max_mag = quake_db_.max_magnitude();
    double min_mag = quake_db_.min_magnitude();
    double step = (max_mag - min_mag)/10;
    float r;
    for (int i = 0; i < 10; i++) {
        if (mag >= min_mag + step*i && mag < min_mag + step *(i+1)) {
            r = 0.012 + (float)i/400;
        }
    }
    return r;
}


void QuakeApp::InitOpenGL() {
    // Set up the camera in a good position to see the entire earth in either mode
    proj_matrix_ = Matrix4::Perspective(60, aspect_ratio(), 0.1, 50);
    view_matrix_ = Matrix4::LookAt(Point3(0,0,3.5), Point3(0,0,0), Vector3(0,1,0));
    glClearColor(0.0, 0.0, 0.0, 1);
    
    // Initialize the earth object
    earth_.Init(search_path_);
    // Initialize the texture used for the background image
    stars_tex_.InitFromFile(Platform::FindFile("iss006e40544.png", search_path_));
}


void QuakeApp::DrawUsingOpenGL() {
    quick_shapes_.DrawFullscreenTexture(Color(1,1,1), stars_tex_);
    
    // You can leave this as the identity matrix and we will have a fine view of
    // the earth.  If you want to add any rotation or other animation of the
    // earth, the model_matrix is where you would apply that.
    Matrix4 R = Matrix4::RotationZ(GfxMath::ToRadians(-23.5));
    Matrix4 model_matrix = Matrix4::RotationY(GfxMath::ToRadians(rotation));
    if (globe_mode_) {
        model_matrix = model_matrix * R;
    }
    // Draw the earth
    earth_.Draw(model_matrix, view_matrix_, proj_matrix_);
    if (debug_mode_) {
        earth_.DrawDebugInfo(model_matrix, view_matrix_, proj_matrix_);
    }

    // TODO: You'll also need to draw the earthquakes.  It's up to you exactly
    Point3 p;
    float r;
    Color col;
    for (int i = 0; i < sphere_position_list.size(); i++) {
        col = color_list[i];
        r = radious_list[i];
        Point3 start = plane_position_list[i];
        Point3 end = sphere_position_list[i];
        Point3 pos = mingfx::Point3::Lerp(start, end, alpha);
        if (globe_mode_) {
            p = model_matrix * R * pos;
        }
        p = model_matrix * pos;
        Matrix4 Mquake = Matrix4::Translation(p - Point3(0,0,0)) * Matrix4::Scale(Vector3(r,r,r));
        quick_shapes_.DrawSphere(Mquake, view_matrix_, proj_matrix_, col);
    }
    color_list.clear();
    radious_list.clear();
    sphere_position_list.clear();
    plane_position_list.clear();
    
}






