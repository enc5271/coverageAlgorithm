
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Point_set_2.h>


#include <vector>
#include <iostream>
#include <fstream>
#include <climits>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <time.h>
#include <cctype>


//Defines pi and other math things
#define _USER_MATH_DEFINES
double SCALEUP = 1.0;

//Typedefs to make everyone happy
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef std::vector<Point_2> Points;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef Polygon_2::Vertex_iterator VertexIterator;
typedef Polygon_2::Edge_const_iterator EdgeIterator;
typedef CGAL::Point_set_2<K>::Vertex_handle Vertex_handle;


using namespace std;

//struct to see if grid section is in the polygon
struct Grid{
	Point_2 topLeft, topRight, botLeft, botRight;
	
	//Member functions
	bool is_bounded(Polygon_2 boundaryPoly){
		bool a,b,c,d;
		if(CGAL::ON_UNBOUNDED_SIDE!= boundaryPoly.bounded_side(topLeft)){
			a = true;
		}
		else{
			a = false;
		}
		if(CGAL::ON_UNBOUNDED_SIDE!= boundaryPoly.bounded_side(topRight)){
			b = true;
		}
		else{
			b = false;
		}
		if(CGAL::ON_UNBOUNDED_SIDE!= boundaryPoly.bounded_side(botLeft)){
			c = true;
		}
		else{
			c = false;
		}
		if(CGAL::ON_UNBOUNDED_SIDE != boundaryPoly.bounded_side(botRight)){
			d = true;
		}
		else{
			d = false;
		}
		if(a && b && c && d){
			return true;
		}
		else{
			return false;
		}


	};
	double randDouble(double min, double max){
		double randNum = (double)rand() / RAND_MAX;
		return min + randNum *(max - min);
	}

	Point_2 genRandPoint(){
		double randX = randDouble(botLeft.x(), botRight.x());
		double randY = randDouble(botLeft.y(),topLeft.y());
		return Point_2(randX,randY);
	};
	
	//Constructor uses the bottom left corner to build the Grid!
	Grid(Point_2 point, double sideLen){
		botLeft = point;
		botRight = Point_2((botLeft.x()+sideLen), botLeft.y());
		topLeft = Point_2((botLeft.x()), (botLeft.y()+sideLen));
		topRight = Point_2((botLeft.x()+sideLen), (botLeft.y()+sideLen));
	};
	void printGrid(){
		cout<<"Bottom:: Left: "<<botLeft<<" Right: "<<botRight<<'\n';
		cout<<"Top:: Left: "<<topLeft<<" Right: "<<topRight<<'\n';
	};
};

//**********************************************************************************
class PathPlanner{
private:
	//Metadata for running search 
	double WPBUFFER = 10;	//EDIT - Remove Idk if its being used
	double safetyBuffer =10;	//EDIT -Remove idk wtf this is...
	long double distBetweenPoints = 0.0001;	//IN USE
	double gridSideLen =10000.0;	//Used to set Grid for RRT search
	
	//Planner Data members
	Point_2 centroid;
	Points BPs;
	Polygon_2 BPpoly;
	Points path;
	long double pathLength =0;
	
	//RRT Search
	Points getRandGridPoints();


public:
	PathPlanner();

	//Non algorithm specific functions
	Points getHull(Points input);
	Points setBPs();
	Polygon_2 makePoly(Points points);
	Polygon_2 convertPoints2Poly(Points in);	//duplicate remove and refactor
	long double getDistance(Point_2 p1, Point_2 p2);
	long double getAnglebetweenPoints(Point_2 point, Point_2 centroid);
	Point_2 getCentroid(Polygon_2 poly);
	void printPoly(Polygon_2 poly);
	void printPoints(Points points);
	Point_2 getCentroid(){return centroid;};
	void setCentroid(Point_2 center){centroid = center;};
	void writeBPs();
	Points setPath(vector<Polygon_2> allPaths);
	Points setPath(Points path);
	void writePath();
	void initialize();
	void writeMetadata();
	void calcPathLength();

//**********************************************************************************
//		Path Algorithm Section	

	//Square Spiral Path Functions
	vector<int> genSpiralSequence(int seqSize);
	Points getSpiralPath();

	//Additive inner Trace - Thanks Dr.Shell!
	bool enscribedPoly(Polygon_2 poly);	//I have no idea how to do this...
	vector<Polygon_2> linearShrink();

	//Rapidly Exploring Random tree search - Shoutout to Dr.Muprhy!
	Points RRT();
	Points TSP(Points unorderedPoints);
	list<Point_2> vector2List(Points vPoints);

//**********************************************************************************
//**********************************************************************************
//		Interface Section
	int runTrace();
	int runSquareSpiral();
	int runRRT();
	void mainMenu();
//**********************************************************************************
};