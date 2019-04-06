#include "Angel-yjc.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <vector>

typedef Angel::vec3     color3;
typedef Angel::vec3     point3;
typedef Angel::vec4     color4;
typedef Angel::vec4     point4;

using namespace std;

class Moon;

/***
 NOTES:
 Constructor has been changed to set renderRadius instead of radius
    - renderRadius is what is used when rendering the sphere objects to the screen
    - radius should contain accurate radial information of the appropriate planet
        - goal is to display this information when user clicks on a planet
 ***/
class Planet {
public:
    //Constructor for planet class:
    Planet();
    Planet(string pName, float pRad, float m, float e, float o, float rotS, bool pMoons) { name = pName;   renderRadius = pRad*10; radius = pRad; major = m; eccentricity = e; orbPeriod = o; rotSpeed = rotS; moons = pMoons; }
    //Destructor for planet class:
    ~Planet() { cout << name << " is destructing." << endl; }
    
    void setName(const string pName) { name = pName; }
    
    void setRadius(const float pRad) { radius = pRad; }
    
    void setMoonFlag(bool hasMoons) { moons = hasMoons; }
    
    void setNumMoons(const int number) {
        if(moons) numMoons = number;
        else {
            cerr << "Error: tried to set number of moons on object with moons = false\n";
            exit(-1);
        }
    }
    
    void addMoon(Moon* moonObj) {
        if(moons) { moonList.push_back(moonObj); numMoons++; }
        else {
            cerr << "Error: tried to add moon on object with moons = false\n";
            exit(-1);
        }
    }
    void addMoons(vector <Moon*> moonObjList) {
        if(moons) {
            for(int i = 0; i < moonObjList.size(); i++) { addMoon(moonObjList[i]); }
        }
        else {
            cerr << "Error: tried to add list of moons to object with moons = false\n";
            exit(-1);
        }
    }
    
    void setNormals(const vector<vec3> norms) {
        for(int i = 0; i < norms.size(); i++) { normals.push_back(norms[i]); }
    }
    
    void setVertices(const vector<point4> &vertList) {
        numVertices = vertList.size();
        for(int i = 0; i < numVertices; i++) { vertices.push_back(vertList[i]); }
    }
    
    void setCenter(const point4 point) { center = point; }
    
    void setRotSpeed(const float speed) { rotSpeed = speed; }
    
    void setRenderRadius(const float newRad) { renderRadius = newRad; }
    
    void setMajorAxis(const float m) { major = m; }
    void setMinorAxis(const float m) { minor = m; }
    
    void setEccentricity(const float e) { eccentricity = e; }
    
    void setOrbitMap(const vector<point4> orbMap) {
        for(int i = 0; i < orbMap.size(); i++) {
            orbitMap.push_back(orbMap[i]);
        }
    }
    
    void setOrbSpeed(const double s) { orbSpeed = s; }
    
    void setOrbPeriod(const float p) { orbPeriod = p; }
    
    void setPerim(const float p) { orbPerimeter = p; }
    
    void setColor(const color4 color) { inherentColor = color; }
    
    void setAmbProd(const color4 color) { ambProd = color; }
    void setDiffProd(const color4 color) { diffuseProd = color; }
    void setSpecProd(const color4 color) { specProd = color; }

    string getName() { return name; }
    
    void getInfo();
    
    bool hasMoons() { return moons; }
    
    vector<Moon*> getMoons() { return moonList; }
    
    vector<vec3> getNormals() { return normals; }
    
    GLuint getBuf() { return buf; }
    
    vector<point4> getVertices() { return vertices; }
    
    long getNumVertices() { return numVertices; }
    
    float getRadius() { return radius; }
    
    point4 getCenter() { return center; }
    
    float getRotSpeed() { return rotSpeed; }
    
    float getRenderRadius() { return renderRadius; }
    
    float getMajorAxis() { return major; }
    float getMinorAxis() { return minor; }
    
    float getEccentricity() { return eccentricity; }
    
    vector<point4> getOrbitMap() { return orbitMap; }
    
    float getOrbPeriod() { return orbPeriod; }
    
    double getOrbSpeed() { return orbSpeed; }
    
    float getPerim() { return orbPerimeter; }

    float getrotSpeed() { return rotSpeed; }
    
    color4 getColor() { return inherentColor; }
    
    color4 getAmbProd() { return ambProd; }
    color4 getDiffProd() { return diffuseProd; }
    color4 getSpecProd() { return specProd; }
    
private:
    string name;
    float radius, renderRadius;  //radius: will contain actual radial data of Planet; renderRadius: radius of rendered Planet object relative to other rendered Planet objects
    float major, minor;  //major: semi-major axis of orbit; minor: semi-minor axis of orbit
    float eccentricity;  //eccentricity of orbit
    float orbPeriod, orbPerimeter;
    bool moons;
    int numMoons;
    long numVertices;
    vector <Moon*> moonList;  //All objects in this list will have the same center of orbit
    GLuint buf;
    float rotSpeed, materialShininess;
    color4 inherentColor;  //Inherent color of material
    color4 ambProd, diffuseProd, specProd;  //Color product of material with light source
    vector<point4> vertices;
    vector<vec3> normals;
    point4 center = {0.0, 0.0, 0.0, 0.0};
    vector<point4> orbitMap;
    double orbSpeed;
};

class Moon {
public:
    Moon();
    Moon(string mName, float mRad, Planet* hostPlanet) {
        name = mName; renderRadius = mRad*10; radius = mRad; orbPlanet = hostPlanet;
    }
    ~Moon() { cout << name << " is destructing." << endl; }
    
    void setNormals(const vector<vec3> norms) {
        for(int i = 0; i < norms.size(); i++) { normals.push_back(norms[i]); }
    }
    
    void setVertices(const vector<point4> &vertList) {
        numVertices = vertList.size();
        for(long i = 0; i < numVertices; i++) { vertices.push_back(vertList[i]); }
    }
    
    void setCenter() { center = orbPlanet->getCenter(); }
    
    void setCenter(const point4 point) { center = point; }
    
    void setRotSpeed(const float speed) { rotSpeed = speed; }
    
    void setRenderRadius(const float newRad) { renderRadius = newRad; }
    
    void setMajorAxis(const float m) { major = m; }
    
    void setMinorAxis(const float m) { minor = m; }
    
    void setEccentricity(const float e) { eccentricity = e; }
    
    void setOrbitSpeed(const float s) { orbitSpeed = s; }

    string getName() { return name; }

    vector<vec3> getNormals() { return normals; }

    GLuint getBuf() { return buf; }
    
    vector<point4> getVertices() { return vertices; }
    
    long getNumVertices() { return numVertices; }
    
    float getRadius() { return radius; }
    
    point4 getCenter() { return center; }
    
    Planet* getOrbPlanet() { return orbPlanet; }
    
    float getRotSpeed() { return rotSpeed; }
    
    float getRenderRadius() { return renderRadius; }
    
    float getMajorAxis() { return major; }
    
    float getMinorAxis() { return minor; }
    
    float getEccentricity() { return eccentricity; }
    
    float getOrbSpeed() { return orbitSpeed; }
    
private:
    string name;
    float radius, renderRadius;  //radius: will contain actual radial data of Planet; renderRadius: radius of rendered Planet object relative to other rendered Planet objects
    float major, minor;
    float eccentricity;
    long numVertices;
    Planet* orbPlanet;
    GLuint buf;
    float orbitSpeed, rotSpeed, materialShininess;
    color4 ambColor, diffuseColor, specColor;  //Inherent color of material
    color4 ambProd, diffuseProd, specProd;  //Color product of material with light source
    vector<point4> vertices;
    vector<vec3> normals;
    point4 center;
};


void Planet::getInfo() {
    cout << "Planet: " << name << endl;
    cout << "\tRadius Compared to Earth: " << radius << endl;
    cout << "\tSemi-Major Axis of Orbit Compared to Earth: " << getMajorAxis() << endl;
    cout << "\tSemi-Minor Axis of Orbit Compared to Earth: " << getMinorAxis() << endl;
    cout << "\tEccentricity: " << getEccentricity() << endl;
    cout << "\tPeriod of Orbit Compared to Earth: " << getOrbPeriod() << endl;
    cout << "\tRotation Speed Compared to Earth: " << getrotSpeed() << endl;
    if(moons) {
        cout << "\tNumber of moons: " << numMoons << endl;
        cout << "\tMoons: " << endl;
        for(int i = 0; i < moonList.size(); i++) {
            cout << "\t\t -" << moonList[i]->getName() << endl;
        }
    }
}

class Sun {
public:
    void addPlanets(vector<Planet*> orbPlans) {
        for(int i = 0; i < orbPlans.size(); i++) {
            orbitingPlanets.push_back(orbPlans[i]);
        }
    }
    
    void setRotSpeed(const float speed) { rotSpeed = speed; }
    
    void setVertices(const vector<point4> &vertList) {
        for(long i = 0; i < vertList.size(); i++) { vertices.push_back(vertList[i]); }
    }
    
    void setRenderRadius(const float newRad) { renderRadius = newRad; }
    
    void setNormals(const vector<vec3> temp) {
        for(int i = 0; i < temp.size(); i++) { normals.push_back(temp[i]); }
    }
    
    void setColor(const color4 color) { sunColor = color; }
    void setAmbProd(const color4 color) { ambProd = color; }
    void setDiffProd(const color4 color) { diffuseProd = color; }
    void setSpecProd(const color4 color) { specProd = color; }

    float getRadius() { return radius; }
    
    float getRotSpeed() { return rotSpeed; }
    
    vector<point4> getVertices() { return vertices; }
    
    long getNumVertices() { return numVertices; }
    
    float getRenderRadius() { return renderRadius; }
    
    point4 getCenter() { return sunPos; }
    
    vector<vec3> getNormals() { return normals; }
    
    color4 getColor() { return sunColor; }
    color4 getAmbProd() { return ambProd; }
    color4 getDiffProd() { return diffuseProd; }
    color4 getSpecProd() { return specProd; }
    
    GLuint getBuf() { return buf; }
    
private:
    string name = "The Sun";
    float radius = 109.0;
    float renderRadius = 109.0 * 10;
    long numVertices;
    vector<Planet*> orbitingPlanets;  //Planets in this list will have same center of orbit
    vector<point4> vertices;
    vector<vec3> normals;
    GLuint buf;
    point4 sunPos = (0.0, 0.0, 0.0, 1,0);  //Position of point source light
    color4 sunColor, ambProd, diffuseProd, specProd;
    color4 whiteAmbLight = (1.0, 1.0, 1.0, 1.0);
    color4 whiteDiffuseLight = (1.0, 1.0, 1.0, 1.0);
    color4 whiteSpecLight = (1.0, 1.0, 1.0, 1.0);
    color4 blackAmbLight = (0.0, 0.0, 0.0, 1.0);
    color4 blackDiffuseLight = (0.8, 0.8, 0.8, 1.0);
    color4 blackSpecLight = (0.2, 0.2, 0.2, 0.1);
    float rotSpeed;
};
