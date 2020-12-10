/** CSci-4611 Assignment 3:  Earthquake
 */

#include "earth.h"
#include "config.h"

#include <vector>
#include <list>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>
#define PI 3.1415926


Earth::Earth() {
}

Earth::~Earth() {
}

void Earth::Init(const std::vector<std::string> &search_path) {
    // init shader program
    shader_.Init();
    
    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // init geometry
    const int nslices = 30;
    const int nstacks = 30;

    // TODO: This is where you need to set the vertices and indiceds for earth_mesh_.

    // As a demo, we'll add a square with 2 triangles.
    
    // put all points into the vertices
    float x_min = (-1)*PI;
    float y_min = (-0.5)*PI;
    float x_step = (2*PI)/30;
    float y_step = (PI)/30;
    
    for (int i = 0; i <= nstacks; i++) {
        for (int j = 0; j <= nslices; j++) {
            float x = x_min + x_step*i;
            float y = y_min + y_step*j;
            plane_vertices.push_back(Point3(x, y, 0));
            plane_norm.push_back(Vector3(0,0,1));

            float normalized_x = (x-x_min)/(2*PI);
            float normalized_y = (y-y_min)/PI;
            Point3 p = Point3(normalized_x, normalized_y, 0);
            plane_tex_coords.push_back(Point2(p.x(), 1- p.y()));
            
            Point3 sphere_point = LatLongToSphere(-90 + (double) j/nstacks * 180, (double) i/nslices * 360 - 180);
            sphere_vertices.push_back(sphere_point);
            Point3 pos = Point3((float) i/nstacks, (float) j/nstacks, 0);
            sphere_tex_coords.push_back(Point2(pos.x(), 1-pos.y()));
            sphere_norm.push_back((sphere_point - Point3(0,0,0)).ToUnit());
        }
    }

    // draw the triangles for the points
    int i = 0;
    int idx = 0;
    while(i < 30) {
        for( int k = idx; k <idx + 30; k++) {
            indices.push_back(k);
            indices.push_back(k+31);
            indices.push_back(k+1);
        }
        for (int k = idx; k < idx + 30; k++) {
            indices.push_back(k+31);
            indices.push_back(k+32);
            indices.push_back(k+1);
        }
        idx =  idx+31;
        i++;
    }
    earth_mesh_.SetVertices(plane_vertices);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetTexCoords(0, plane_tex_coords);
    earth_mesh_.SetNormals(plane_norm);
    earth_mesh_.UpdateGPUMemory();
    
}

void Earth::Globe(float alpha) {
    std::vector<Point3> transform_vertices;
    std::vector<Vector3> transform_norms;
    for (int i = 0; i < sphere_vertices.size(); i++) {
        start = plane_vertices[i];
        end = sphere_vertices[i];
        Point3 p = mingfx::Point3::Lerp(start, end, alpha);
        transform_norms.push_back(p - Point3(0,0,0));
        transform_vertices.push_back(p);
    }
    
    earth_mesh_.SetVertices(transform_vertices);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetTexCoords(0, sphere_tex_coords);
    earth_mesh_.SetNormals(transform_norms);
    earth_mesh_.UpdateGPUMemory();
}
void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}


Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> sphere calculations in one place.
    float lat_radious = GfxMath::ToRadians((float) latitude);
    float long_radious = GfxMath::ToRadians((float) longitude);
    float x = cos(lat_radious) * sin(long_radious);
    float y = sin(lat_radious);
    float z = cos(lat_radious) * cos(long_radious);
    return Point3(x, y, z);
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> plane calculations in one place.
    float x = (float) longitude * (PI/180.0);
    float y = (float) latitude * (PI/180.0);
    return Point3(x, y, 0);
}



void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_vertices(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertex(indices[0]));
        loop.push_back(earth_mesh_.vertex(indices[1]));
        loop.push_back(earth_mesh_.vertex(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1,1,0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }
}
