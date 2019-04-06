/***
 Interactive rendering of the Milky Way solar system using OpenGL
 With the exception of the files "mat-yjc-new.h", "Angel-yjc.h", "InitShader.cpp", "vec.h", and the shader files, all code in this program was written by Dennis Williams
 ***/

/***
 To do:
 [X] Planet class
 [X] Star class
 [X] Add physical shapes to planet objects (create sphere vertices to be stored in sphere object's data structure); will need to convert from vectors to arrays when sending to shaders
 [X] Add movement to planets (elliptical orbit and rotation on axis)
 [X] Add light point source at position of the sun
 [X] Calculate inherent colors and lighting param's of each Planet to be passed to shaders
 [X] init()
 [X] drawObj()
 [ ] display()
 [ ] Render planets with proper movements and inherent colors
 [ ] Add shadows appropriately
 [ ] Map planet textures onto appropriate planets
 [ ] Add menu and keboard functionality for interactiveness
 [ ] Add movement to moons (elliptical orbit around planet and rotation on axis)
 [ ] Add axial tilt to planets and moons
 ***/

#include <iostream>
#include "planet.h"
#include <string>
#include <vector>

//using namspace std;

int wireFlag = 0;  //0: Wire-frame rendering;  1: Filled rendering

GLuint program, model_view, projection;
GLfloat fovy = 45.0;
GLfloat aspect = 1.0;
GLfloat near = 0.1;     GLfloat far = 1000.0;
GLfloat angle = 0.0;
vec4 eye = vec4(-10.0, 3.0, -10.0, 1.0);  //Initial view position

vector<Planet*> planets; //Global list of all planets in the scene
vector<Moon*> moons;  //Global list of all moons in the scene
const double PI = 3.14159265358979;

//Global planet object declaractions:
Planet Mercury("Mercury", 0.382, 0.3871, 0.205630, 0.241, 0.0171, false);
Planet Venus("Venus", 0.949, 0.7232, 0.007, 0.616, 0.0086, false);
Planet Earth("Earth", 1.0, 1, 0.0167, 1, 1, true);
Planet Mars("Mars", 0.533, 1.5235, 0.0935, 1.882, 0.9747, true);
Planet Jupiter("Jupiter", 11.2, 5.2043, 0.0489, 11.870, 2.416, true);
Planet Saturn("Saturn", 9.5, 9.617, 0.0565, 29.477, 2.2432, true);
Planet Uranus("Uranus", 4.0, 19.288, 0.0464, 84.078, 1.3926, true);
Planet Neptune("Neptune", 3.9, 30.2, 0.0095, 164.882, 1.4908, true);

Moon TheMoon("Moon", 0.27, &Earth);
Moon Phobos("Phobos", 0.27, &Mars);
Moon Deimos("Deimos", 0.27, &Mars);
Moon jMoon("Jupiter's Moon", 0.27, &Jupiter);
Moon sMoon("Saturn's Moon", 0.27, &Saturn);
Moon uMoon("Uranus's Moon", 0.27, &Uranus);
Moon nMoon("Neptune's Moon", 0.27, &Neptune);

point4 *sunPoints, *mercPoints, *venPoints, *earthPoints, *marsPoints, *jupPoints, *satPoints, *urPoints, *nepPoints;  //Points for each vertex in each Planet/Sun
point4 *mercPath, *venPath, *earthPath, *marsPath, *jupPath, *satPath, *urPath, *nepPath;  //Coordinates of elliptical path of each Planet object
vec3 *sunNorms, *mercNorms, *venNorms, *earthNorms, *marsNorms, *jupNorms, *satNorms, *urNorms, *nepNorms;  //Normals for each Planet/Sun

Sun theSun;

GLuint mercBuf = Mercury.getBuf();      GLuint venBuf = Venus.getBuf();
GLuint earthBuf = Earth.getBuf();       GLuint marsBuf = Mars.getBuf();
GLuint jupBuf = Jupiter.getBuf();       GLuint satBuf = Saturn.getBuf();
GLuint urBuf = Uranus.getBuf();         GLuint nepBuf = Neptune.getBuf();
GLuint sunBuf = theSun.getBuf();

float const_att = 2.0;  //Constant attenuation
float linear_att = 0.01;  //Linear attenuation
float quad_att = 0.001;  //Exponential attenuation
float  material_shininess = 125.0;  //Shininess of material
color4 black_ambient(0.0, 0.0, 0.0, 1.0);
color4 black_diffuse(0.8, 0.8, 0.8, 1.0);
color4 black_specular(0.2, 0.2, 0.2, 1.0);
color4 white_ambient(1.0, 1.0, 1.0, 1.0);
color4 white_diffuse(1.0, 1.0, 1.0, 1.0);
color4 white_specular(1.0, 1.0, 1.0, 1.0);
color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );  //Ambient material parameter
color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );  //Diffuse material parameter
color4 material_specular( 1.0, 0.8, 0.0, 1.0 );  //Specular material parameter
point4 lightPos = theSun.getCenter();  //Position of sun's center = position of light source

//Light parameters passed to shaders for each type of lighting for each obj in scene
color4 black_ambient_product = black_ambient * /* lightPos * */ material_ambient;
color4 black_diffuse_product = black_diffuse * /* lightPos * */ material_diffuse;
color4 black_specular_product = black_specular * /* lightPos * */ material_specular;

//vector<Moon*> jMoonList, sMoonList, uMoonList, nMoonList; //Temporarily hold Moon objects for Planets with many moons – to be copied into class object data structure

void pairMoonPlanet() {  //Pair moons with their respective planets; also creates planet and moon lists
    ////This function seems to be causing a segmentation fault, most likely due to the large array sizes being generated in the lines below
    ////Most likely solution: Allocate new lists on heap (instead of stack) by using new()
    Earth.addMoon(&TheMoon);
    Mars.addMoon(&Phobos);  Mars.addMoon(&Deimos);

    for(int i = 0; i < 79; i++) { Jupiter.addMoon(&jMoon); }
    
    for(int i = 0; i < 62; i++) { Saturn.addMoon(&sMoon); }
    
    for(int i = 0; i < 27; i++) { Uranus.addMoon(&uMoon); }
    
    for(int i = 0; i < 14; i++) { Neptune.addMoon(&nMoon); }
    
    //Add planets to global vector 'planets':
    planets.push_back(&Mercury);    planets.push_back(&Venus);
    planets.push_back(&Earth);      planets.push_back(&Mars);
    planets.push_back(&Jupiter);    planets.push_back(&Saturn);
    planets.push_back(&Uranus);     planets.push_back(&Neptune);
    
    //Add moons to global vector 'moons':
    for(int i = 0; i < planets.size(); i++) {
        if(planets[i]->hasMoons()) {
            vector<Moon*> moonList = planets[i]->getMoons();
            for(int j = 0; j < moonList.size(); j++) { moons.push_back(moonList[j]); }
        }
    }
}

void createSphereObj(const vector<Planet*> planetList) {
    for(int i = 0; i < planetList.size(); i++) {
        vector<point4> temp;  //Temp vector to hold vertices during sphere object generation
        float r = planetList[i]->getRenderRadius();
        for(float phi = 0; phi < 2*PI; phi += PI/10) {
            for(float theta = 0; theta < PI; theta += PI/10) {
                point4 point;
                point4 center = {0.0, 0.0, 0.0, 1.0};  //All sphere objs generated w/ center at origin
                point.x = r * cos(phi) * sin(theta) + center.x;
                point.y = r * sin(phi) * sin(theta) + center.y;
                point.z = r            * cos(theta) + center.z;
                point.w = 1.0;
                temp.push_back(point);
            }
        }
        planetList[i]->setVertices(temp);  //Store sphere obj vertices in respective Planet class
        temp.clear();
    }
}

void createSphereObj(const vector<Moon*> moonList) {
    for(int i = 0; i < moonList.size(); i++) {
        vector<point4> temp;  //Temp vector to hold vertices during sphere object generation
        float r = moonList[i]->getRenderRadius();
        for(float phi = 0; phi < 2*PI; phi += PI/10) {
            for(float theta = 0; theta < PI; theta += PI/10) {
                point4 point;
                point4 center = moonList[i]->getOrbPlanet()->getCenter();  //All sphere objs generated w/ center at center of orbPlanet
                point.x = r * cos(phi) * sin(theta) + center.x;
                point.y = r * sin(phi) * sin(theta) + center.y;
                point.z = r            * cos(theta) + center.z;
                point.w = 1.0;
                temp.push_back(point);
            }
        }
        moonList[i]->setVertices(temp);  //Store sphere obj vertices in respective Planet class
        temp.clear();
    }
}

void createSunObj() {
    vector<point4> temp;  //Temp vector to gold vertices during Sun creation
    float r = theSun.getRenderRadius();
    for(double phi = 0; phi < 2*PI; phi += PI/10) {
        for(double theta = 0; theta < PI; theta += PI/10) {
            point4 point;
            point4 center = {0.0, 0.0, 0.0, 1.0};  //Sun will always be centered at origin
            point.x = r * cos(phi) * sin(theta) + center.x;
            point.y = r * sin(phi) * sin(theta) + center.y;
            point.z = r            * cos(theta) + center.z;
            point.w = 1.0;
            temp.push_back(point);
        }
    }
    theSun.setVertices(temp);
}

////Orbit data has NOT been calculated for moons
//Calculate semi-minor axis, orbit speed, and generate map of orbit points for each Planet:
void calcOrbit(const vector<Planet*> planetList) {
    for(int i = 0; i < planetList.size(); i++) {
        //Calc and set semi-minor axis of Planet's orbit
        float a = planetList[i]->getMajorAxis();
        float e = planetList[i]->getEccentricity();
        float b = a * sqrt(1 - (e*e));
        planetList[i]->setMinorAxis(b);
        
        //Calc and set orbit speed of Planet
        float h = ((a-b)*(a-b)) / ((a+b)*(a+b));
        float p = PI * (a+b) * ((1 + (1/4)*h + (1/64)*h*h + (1/256)*h*h*h + (1/1024)*h*h*h*h));
        planetList[i]->setPerim(p);
        float L = planetList[i]->getOrbPeriod();
        double s = p/L;
        planetList[i]->setOrbSpeed(s);
        
        vector<point4> orbMap;
        point4 point;
        //Calc and set coordinates of orbit for Planet
        for(int j = 0; j < 360; j++) {
            point.x += a * cos(j);
            point.z += b * sin(j);
            point.y = 0;    point.w = 1;
            orbMap.push_back(point);
        }
        planetList[i]->setOrbitMap(orbMap);
        orbMap.clear();
    }
    
    //Copy path coordinates to global array:
    mercPath = (point4 *) malloc(Mercury.getOrbitMap().size() * sizeof(point4));
    vector<point4> mercPathTemp = Mercury.getOrbitMap();
    for(int i = 0; i < mercPathTemp.size(); i++) { mercPath[i] = mercPathTemp[i]; }
    mercPathTemp.clear();
    
    venPath = (point4 *) malloc(Venus.getOrbitMap().size() * sizeof(point4));
    vector<point4> venPathTemp = Venus.getOrbitMap();
    for(int i = 0; i < venPathTemp.size(); i++) { venPath[i] = venPathTemp[i]; }
    venPathTemp.clear();
    
    earthPath = (point4 *) malloc(Earth.getOrbitMap().size() * sizeof(point4));
    vector<point4> earthPathTemp = Earth.getOrbitMap();
    for(int i = 0; i < earthPathTemp.size(); i++) { earthPath[i] = earthPathTemp[i]; }
    earthPathTemp.clear();
    
    marsPath = (point4 *) malloc(Mars.getOrbitMap().size() * sizeof(point4));
    vector<point4> marsPathTemp = Mars.getOrbitMap();
    for(int i = 0; i < marsPathTemp.size(); i++) { marsPath[i] = marsPathTemp[i]; }
    marsPathTemp.clear();
    
    jupPath = (point4 *) malloc(Jupiter.getOrbitMap().size() * sizeof(point4));
    vector<point4> jupPathTemp = Jupiter.getOrbitMap();
    for(int i = 0; i < jupPathTemp.size(); i++) { jupPath[i] = jupPathTemp[i]; }
    jupPathTemp.clear();
    
    satPath = (point4 *) malloc(Saturn.getOrbitMap().size() * sizeof(point4));
    vector<point4> satPathTemp = Saturn.getOrbitMap();
    for(int i = 0; i < satPathTemp.size(); i++) { satPath[i] = satPathTemp[i]; }
    satPathTemp.clear();
    
    urPath = (point4 *) malloc(Uranus.getOrbitMap().size() * sizeof(point4));
    vector<point4> urPathTemp = Uranus.getOrbitMap();
    for(int i = 0; i < urPathTemp.size(); i++) { satPath[i] = satPathTemp[i]; }
    urPathTemp.clear();
    
    nepPath = (point4 *) malloc(Neptune.getOrbitMap().size() * sizeof(point4));
    vector<point4> nepPathTemp = Neptune.getOrbitMap();
    for(int i = 0; i < nepPathTemp.size(); i++) { nepPath[i] = nepPathTemp[i]; }
    nepPathTemp.clear();
}

void calcNormals(const vector<Moon*> moon, int a, int b, int c) {  //Calculate normals for each vertex in Moon object
    //int Index = 0;
    vector<point3> normals; // = moon[0]->getNormals();
    vector<point4> vertices = moon[0]->getVertices();
    
    for(int i = 0; c <= vertices.size(); i++) {
        vec4 u = vertices[b] - vertices[a];
        vec4 v = vertices[c] - vertices[b];
        
        vec3 normal = normalize(cross(u,v));
        
        normals.push_back(normal);   //Index++;
        normals.push_back(normal);   //Index++;
        normals.push_back(normal);   //Index++;
        a += 3; b += 3; c += 3;
    }
    for(int i = 0; i < moon.size(); i++) { moon[i]->setNormals(normals); }  //Copy normals to Moon->normals
}

void calcNormals(Planet* planet, int a, int b, int c) { //Calculate normals for each vertex in Planet object
    //int Index = 0;
    vector<point3> normals; // = planet->getNormals();
    vector<point4> vertices = planet->getVertices();
    
    for(int i = 0; c <= vertices.size(); i++) {
        vec4 u = vertices[b] - vertices[a];
        vec4 v = vertices[c] - vertices[b];
        
        vec3 normal = normalize(cross(u,v));
        
        normals.push_back(normal);    //Index++;
        normals.push_back(normal);    //Index++;
        normals.push_back(normal);    //Index++;
        a += 3; b += 3; c +=3;
    }
    planet->setNormals(normals); //Copy normals to Planet->normals
    if(planet->hasMoons()) {
        vector<Moon*> tempMoons = planet->getMoons();
        calcNormals(tempMoons, 0, 1, 2);
    }
}

// RGBA colors
color4 vertex_colors4[10] = { //Not used, but listed for color references
    color4( 0.0, 0.0, 0.0, 1.0),  // black
    color4( 1.0, 0.0, 0.0, 1.0),  // red
    color4( 1.0, 1.0, 0.0, 1.0),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0),  // green
    color4( 0.0, 0.0, 1.0, 1.0),  // blue
    color4( 1.0, 0.0, 1.0, 1.0),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0),  // white
    color4( 0.0, 1.0, 1.0, 1.0),  // cyan
    color4( 1.0, 0.84, 0.0, 1.0),  // golden yellow
    color4(0.529, 0.807, 0.92, 1.0)  // light blue
};

//RGB colors
color3 vertex_colors3[10] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.0, 1.0, 0.0),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0),  // cyan
    color3( 1.0, 0.84, 0.0),  // golden yellow
    color3(0.529, 0.807, 0.92)  // light blue
};

void setColors() {  //Set the inherent color of each Planet before calculating color param's
    theSun.setColor(vertex_colors4[8]);  //Golden Yellow
    Mercury.setColor(vertex_colors4[3]);  //Green
    Venus.setColor(vertex_colors4[1]);  //Red
    Earth.setColor(vertex_colors4[4]);  //Blue
    Mars.setColor(vertex_colors4[1]);  //Red
    Jupiter.setColor(vertex_colors4[5]);  //Magenta
    Saturn.setColor(vertex_colors4[2]);  //Yellow
    Uranus.setColor(vertex_colors4[9]);  //Light Blue
    Neptune.setColor(vertex_colors4[4]);  //Blue
}

void calcColors(vector<Planet*> planetList) {  //Calculate colors w/ lighting for each planet
    for(int i = 0; i < planetList.size(); i++) {  //Set Planet colors
        color4 planetColor = planetList[i]->getColor();
        color4 ambProd = planetColor * material_ambient;
        color4 diffProd = planetColor * material_diffuse;
        color4 specProd = planetColor * material_specular;
        
        planetList[i]->setAmbProd(ambProd);
        planetList[i]->setDiffProd(diffProd);
        planetList[i]->setSpecProd(specProd);
    }
    
    //Set Sun color:
    color4 sunColor = theSun.getColor();
    color4 ambProd = sunColor * material_ambient;
    color4 diffProd = sunColor * material_diffuse;
    color4 specProd = sunColor * material_specular;
    
    theSun.setAmbProd(ambProd);  theSun.setDiffProd(diffProd);  theSun.setSpecProd(specProd);
}

void genBufferData() {
    mercPoints = (point4 *) malloc(Mercury.getVertices().size() * sizeof(point4));
    vector<point4> mercTemp = Mercury.getVertices();
    for(int i = 0; i < mercTemp.size(); i++) { mercPoints[i] = mercTemp[i]; }
    mercTemp.clear();
    
    mercNorms = (vec3 *) malloc(Mercury.getNormals().size() * sizeof(vec3));
    vector<vec3> mTemp = Mercury.getNormals();
    for(int i = 0; i < mTemp.size(); i++) { mercNorms[i] = mTemp[i]; }
    mTemp.clear();
    
    venPoints = (point4 *) malloc(Venus.getVertices().size() * sizeof(point4));
    vector<point4> venTemp = Venus.getVertices();
    for(int i = 0; i < venTemp.size(); i++) { venPoints[i] = venTemp[i]; }
    venTemp.clear();
    
    venNorms = (vec3 *) malloc(Venus.getNormals().size() * sizeof(vec3));
    vector<vec3> vTemp = Venus.getNormals();
    for(int i = 0; i < vTemp.size(); i++) { venNorms[i] = vTemp[i]; }
    vTemp.clear();
    
    earthPoints = (point4 *) malloc(Earth.getVertices().size() * sizeof(point4));
    vector<point4> earthTemp = Earth.getVertices();
    for(int i = 0; i < earthTemp.size(); i++) { earthPoints[i] = earthTemp[i]; }
    earthTemp.clear();
    
    earthNorms = (vec3 *) malloc(Earth.getNormals().size() * sizeof(vec3));
    vector<vec3> eTemp = Earth.getNormals();
    for(int i = 0; i < eTemp.size(); i++) { earthNorms[i] = eTemp[i]; }
    eTemp.clear();
    
    marsPoints = (point4 *) malloc(Mars.getVertices().size() * sizeof(point4));
    vector<point4> marsTemp = Mars.getVertices();
    for(int i = 0; i < marsTemp.size(); i++) { marsPoints[i] = marsTemp[i]; }
    marsTemp.clear();
    
    marsNorms = (vec3 *) malloc(Mars.getNormals().size() * sizeof(vec3));
    vector<vec3> mTemp2 = Mars.getNormals();
    for(int i = 0; i < mTemp2.size(); i++) { marsNorms[i] = mTemp2[i]; }
    mTemp2.clear();
    
    jupPoints = (point4 *) malloc(Jupiter.getVertices().size() * sizeof(point4));
    vector<point4> jupTemp = Jupiter.getVertices();
    for(int i = 0; i < jupTemp.size(); i++) { jupPoints[i] = jupTemp[i]; }
    jupTemp.clear();
    
    jupNorms = (vec3 *) malloc(Jupiter.getNormals().size() * sizeof(vec3));
    vector<vec3> jTemp = Jupiter.getNormals();
    for(int i = 0; i < jTemp.size(); i++) { jupNorms[i] = jTemp[i]; }
    jTemp.clear();
    
    satPoints = (point4 *) malloc(Saturn.getVertices().size() * sizeof(point4));
    vector<point4> satTemp = Saturn.getVertices();
    for(int i = 0; i < satTemp.size(); i++) { satPoints[i] = satTemp[i]; }
    satTemp.clear();
    
    satNorms = (vec3 *) malloc(Saturn.getNormals().size() * sizeof(vec3));
    vector<vec3> sTemp = Saturn.getNormals();
    for(int i = 0; i < sTemp.size(); i++) { satNorms[i] = sTemp[i]; }
    sTemp.clear();
    
    urPoints = (point4 *) malloc(Uranus.getVertices().size() * sizeof(point4));
    vector<point4> urTemp = Uranus.getVertices();
    for(int i = 0; i < urTemp.size(); i++) { urPoints[i] = urTemp[i]; }
    urTemp.clear();
    
    urNorms = (vec3 *) malloc(Uranus.getNormals().size() * sizeof(vec3));
    vector<vec3> uTemp = Uranus.getNormals();
    for(int i = 0; i < uTemp.size(); i++) { urNorms[i] = uTemp[i]; }
    uTemp.clear();
    
    nepPoints = (point4 *) malloc(Neptune.getVertices().size() * sizeof(point4));
    vector<point4> nepTemp = Neptune.getVertices();
    for(int i = 0; i < nepTemp.size(); i++) { nepPoints[i] = nepTemp[i]; }
    nepTemp.clear();
    
    nepNorms = (vec3 *) malloc(Neptune.getNormals().size() * sizeof(vec3));
    vector<vec3> nTemp = Neptune.getNormals();
    for(int i = 0; i < nTemp.size(); i++) { nepNorms[i] = nTemp[i]; }
    nTemp.clear();
    
    sunPoints = (point4 *) malloc(theSun.getVertices().size() * sizeof(point4));
    vector<point4> sunTemp = theSun.getVertices();
    for(int i = 0; i < sunTemp.size(); i++) { sunPoints[i] = sunTemp[i]; }
    sunTemp.clear();
    
    sunNorms = (vec3 *) malloc(theSun.getNormals().size() * sizeof(vec3));
    vector<vec3> sunTemp2 = theSun.getNormals();
    for(int i = 0; i < sunTemp2.size(); i++) { sunNorms[i] = sunTemp2[i]; }
    sunTemp2.clear();
}

void init() {
    genBufferData();
    
    glGenBuffers(1, &mercBuf);  //Mercury VBO
    glBindBuffer(GL_ARRAY_BUFFER, mercBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mercPoints) + sizeof(mercNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mercPoints), mercPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mercPoints), sizeof(mercNorms), mercNorms);
    
    glGenBuffers(1, &venBuf);  //Venus VBO
    glBindBuffer(GL_ARRAY_BUFFER, venBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(venPoints) + sizeof(venNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(venPoints), venPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(venPoints), sizeof(venNorms), venNorms);
    
    glGenBuffers(1, &earthBuf);  //Earth VBO
    glBindBuffer(GL_ARRAY_BUFFER, earthBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(earthPoints) + sizeof(earthNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(earthPoints), earthPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(earthPoints), sizeof(earthNorms), earthNorms);
    
    glGenBuffers(1, &marsBuf);  //Mars VBO
    glBindBuffer(GL_ARRAY_BUFFER, marsBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(marsPoints) + sizeof(marsNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(marsPoints), marsPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(marsPoints), sizeof(marsNorms), marsNorms);
    
    glGenBuffers(1, &jupBuf);  //Jupiter VBO
    glBindBuffer(GL_ARRAY_BUFFER, jupBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(jupPoints) + sizeof(jupNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(jupPoints), jupPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(jupPoints), sizeof(jupNorms), jupNorms);
    
    glGenBuffers(1, &satBuf);  //Saturn VBO
    glBindBuffer(GL_ARRAY_BUFFER, satBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(satPoints) + sizeof(satNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(satPoints), satPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(satPoints), sizeof(satNorms), satNorms);
    
    glGenBuffers(1, &urBuf);  //Uranus VBO
    glBindBuffer(GL_ARRAY_BUFFER, urBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(urPoints) + sizeof(urNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(urPoints), urPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(urPoints), sizeof(urNorms), urNorms);
    
    glGenBuffers(1, &nepBuf);  //Neptune VBO
    glBindBuffer(GL_ARRAY_BUFFER, nepBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nepPoints) + sizeof(nepNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(nepPoints), nepPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(nepPoints), sizeof(nepNorms), nepNorms);
    
    glGenBuffers(1, &sunBuf);  //Sun VBO
    glBindBuffer(GL_ARRAY_BUFFER, sunBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunPoints) + sizeof(sunNorms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sunPoints), sunPoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(sunPoints), sizeof(sunNorms), sunNorms);
    
    program = InitShader("vshader.glsl", "fshader.glsl");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 512, 512);
    glEnable( GL_DEPTH_TEST );  //Enable z-buffer testing
    glDepthFunc(GL_LESS);
    glClearColor(0.132, 0.171, 1.0, 1.0);  //ClearColor is a dark blue
    glLineWidth(2.0);
}

void setUpLightingParams(mat4 mv, Planet* p) {
    
    color4 ambProd = p->getAmbProd();
    color4 diffProd = p->getDiffProd();
    color4 specProd = p->getSpecProd();
    
    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
                 1, ambProd);
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
                 1, diffProd);
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
                 1, specProd);
    
    // The Light Position in Eye Frame
    vec4 light_position_eyeFrame = mv * lightPos;
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
                 1, light_position_eyeFrame);
    
    glUniform1f(glGetUniformLocation(program, "ConstAtt"),
                const_att);
    glUniform1f(glGetUniformLocation(program, "LinearAtt"),
                linear_att);
    glUniform1f(glGetUniformLocation(program, "QuadAtt"),
                quad_att);
    glUniform1f(glGetUniformLocation(program, "Shininess"),
                material_shininess );
}

void drawObj(GLuint buffer, long numVertices) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    //----- Set up vertex attribute arrays for each vertex attribute -----/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * numVertices) );
    
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    
    //--- Disable each vertex attribute array being enabled ---/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vNormal);
}

void display( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram(program); // Use the shader program
    
    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );
    
    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, near, far); //use this to make a perspective projection, call from init()
    
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
    
    // Generate the model-view matrix
    vec4 at(0.0, 0.0, 0.0, 1.0);
    vec4 up(0.0, 1.0, 0.0, 0.0); //VUP
    mat4 mv = LookAt(eye, at, up); // model-view matrix using Correct LookAt() model-view matrix for the light position.
    
    setUpLightingParams(mv, &Mercury);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    mat3 normal_matrix = NormalMatrix(mv, 1);
    glUniformMatrix3fv(glGetUniformLocation(program, "normal_matrix"), 1, GL_TRUE, normal_matrix);
    
    if (wireFlag == 1) // Filled floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int m = 0;
    while(m+1 <= sizeof(mercPath)) {
        point4 mercCurr = mercPath[m];
        point4 mercNext = mercPath[m+1];  //Since loop only runs while mercNext isn't NULL, no need for a check to make sure within bounds
        vec4 mercTVec;  //Translation vector for Mercury
        vec4 mercRotVec;  //Rotation vector for Mercury
        double mercP = Mercury.getPerim();  //Length of Mercury's orbit
    }
    drawObj(mercBuf, Mercury.getNumVertices());
}

int main(int argc, const char * argv[]) {
    pairMoonPlanet();
    theSun.addPlanets(planets);
    createSunObj();
    createSphereObj(planets);
    createSphereObj(moons);
    
    calcOrbit(planets);
    for(int i = 0; i < planets.size(); i++) { calcNormals(planets[i], 0, 1, 2); }
    
    setColors();
    calcColors(planets);
    
    Mercury.getInfo();
    Venus.getInfo();
    Earth.getInfo();
    Mars.getInfo();
    Jupiter.getInfo();
    Saturn.getInfo();
    Uranus.getInfo();
    Neptune.getInfo();
}
