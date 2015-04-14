#include "coverageAlgorithm.h"

//Takes convex hull of input Point_2 and returns a vector<Point_2> 
Points PathPlanner::getHull(Points input){
	Points result;
	CGAL::convex_hull_2( input.begin(), input.end(), std::back_inserter(result) );
	this->BPs = result;
	cout<<"Hull Result\n";
	printPoints(result);
	return result;
}

//Get the BPs from a file called WP.txt
Points PathPlanner::setBPs(){
	Points wps;
	ifstream inputFile ("BP.txt");
	while(!inputFile.eof()) {			//reads input
		// stringstream ss; 
		// string s;
		// long double xval, yval;
		// string trash;
		// getline(inputFile , s);
		// ss << s;
		// if (s[s.size()-1] != ';') {
		// 	//The format of the text file is latitude, longitude
		// 	ss >> yval >> xval;
		// 	cout<<std::fixed;
		// 	cout<<xval<<" , "<<yval<<endl;
		// 	wps.push_back(Point_2(xval,yval));		
		// }

		long double xval, yval;
		inputFile>>yval>>xval;
		std::cout.precision(20);
		//cout<<xval<<" , "<<yval<<endl;
		wps.push_back(Point_2(xval,yval));
	}
	
	return wps;
}

//Sill function to make a polygon type.There are more intricate checks and functions that can be used with the ploygon
//class but for now it is simply an intermediate or unecessary step.
Polygon_2 PathPlanner::makePoly(Points points){
	Polygon_2 poly;
	for (int i =0 ; i < points.size(); i++){
		poly.push_back(points[i]);	
	}
	return poly;
}

//Euclidean distance calculation
long double PathPlanner::getDistance(Point_2 p1, Point_2 p2){
	long double dx = p1.x()-p2.x();
    long double dy = p1.y()-p2.y();
    long double dist = sqrt(dx*dx + dy*dy);
    return dist;
}

//Returns the angle between points in degrees
long double PathPlanner::getAnglebetweenPoints(Point_2 point, Point_2 centroid) {
    long double dx = centroid.x()-point.x();
    long double dy = centroid.y()-point.y();
    dx = abs(dx);
    dy = abs(dy);
    long double angle = atan(dy/dx)*(180/M_PI);
    cout<<point<<" deg: "<<angle<<'\n';
    return angle;
}

//Ohhh gosh. uhmm. gee what does this do?
Point_2 PathPlanner::getCentroid(Polygon_2 poly){	
	long double totalX =0;
	long double totalY =0;
	for (int i =0 ; i < poly.size(); i ++){
		totalX += poly[i].x();
		totalY += poly[i].y();
	}
	Point_2 centroid( (totalX/poly.size()) , (totalY/poly.size()) );
	return centroid;
}





void PathPlanner::printPoly(Polygon_2 poly){
	for ( int i =0 ; i < poly.size(); i++){
		cout<<std::fixed;
		cout<<"Point: "<<i<<" "<<poly[i].x()<<" , "<<poly[i].y()<<'\n';
	}
}

void PathPlanner::printPoints(Points points){
	for ( int i =0 ; i < points.size(); i++){
		cout<<"Point: "<<i<<" "<<points[i].x()<<" , "<<points[i].y()<<'\n';
	}
}



//Writes to file "Path.txt" creating a list of points
Points PathPlanner::setPath(vector<Polygon_2> allPaths){
	Points inPath;
	for(int i =0 ; i < allPaths.size(); i++){
		for(int j =0 ; j< allPaths[i].size(); j++){
			inPath.push_back(allPaths[i][j]);
		}
	}

	this->path = inPath;

	return inPath;
}

//Write to file "Path.txt" using vector<Point_2>
Points PathPlanner::setPath(Points inPath){

	this->path = inPath;
	return inPath;
}

void PathPlanner::writePath(){
	ofstream output("Path.txt");
	if(output.is_open()){
		
		for (int i =0 ; i <path.size(); i ++){
			output.precision(10);
			output<<path[i].y()/SCALEUP<<" , "<<path[i].x()/SCALEUP<<'\n';
		}
		output.close();
	}
}

void PathPlanner::writeMetadata(){
	ofstream output("Metadata.txt");
	if(output.is_open()){
		output.precision(10);
		output<<this->centroid.y()/SCALEUP<<" , "<<this->centroid.x()/SCALEUP<<'\n';
		for (int i =0 ; i <BPs.size(); i ++){
			//output<<std::fixed;
			output<<BPs[i].y()/SCALEUP<<" , "<<BPs[i].x()/SCALEUP<<'\n';
		}
		output.close();
	}
}

bool PathPlanner::enscribedPoly(Polygon_2 poly){

}

Polygon_2 PathPlanner::convertPoints2Poly(Points in){
	Polygon_2 poly;
	for (int i = 0; i < in.size(); ++i)
	{
		poly.push_back(in[i]);
	}
	return poly;
}

vector<Polygon_2> PathPlanner::linearShrink(){
	// Points dist2center;	//really lazy hack to store xcomp and ycomp in the point class
	// for(int i =0 ; i < BPs.size(); i++){
	// 	long  double distance = getDistance(this->centroid,BPs[i]);
		
	// 	cout<<"Dist: "<<distance<<'\n';
	// 	cout<<"distBetweenPoints "<< this->distBetweenPoints<<'\n';
	// 	//BUG - dividing by any decimal severly skews the results
	// 	long  double increment = distance - distBetweenPoints;
	// 	cout<<"increment: "<<increment<<'\n';
		
	// 	long  double angle = getAnglebetweenPoints(BPs[i], this->centroid);
	// 	cout<<"angle: "<<angle<<'\n';
	// 	long  double xcomp = cos(angle*M_PI/180.0)*increment;
	// 	cout<<"xcomp: "<<xcomp<<'\n';
	// 	long  double ycomp = sin(angle*M_PI/180.0)*increment;
	// 	cout<<"ycomp: "<<ycomp<<'\n';
	// 	dist2center.push_back(Point_2(xcomp,ycomp));
	// }
	// printPoints(dist2center);
	vector<Polygon_2> allTraces;
	Polygon_2 polyIterator = convertPoints2Poly(this->BPs);
	
	bool centroidFlag = true;
	while(centroidFlag){
		Polygon_2 holder;
		for (int i = 0; i < polyIterator.size(); ++i)
		{
			// long  double x,y;
			// if(polyIterator[i].x() <centroid.x() ){
			// 	x = polyIterator[i].x()+dist2center[i].x();
			// 	y = polyIterator[i].y()+dist2center[i].y();
			// }
			// else{
			// 	x = polyIterator[i].x()-dist2center[i].x();
			// 	y = polyIterator[i].y()-dist2center[i].y();
			// }
			cout<<"centroid: "<<this->centroid<<"\tpoint: "<<polyIterator[i]<<endl;
			long  double distance = getDistance(this->centroid,polyIterator[i]);
		
			cout<<"Dist: "<<distance<<'\n';
			cout<<"distBetweenPoints "<< this->distBetweenPoints<<'\n';
			long  double increment = distance - this->distBetweenPoints;
			cout<<"increment: "<<increment<<'\n';
			
			long  double angle = getAnglebetweenPoints(polyIterator[i], this->centroid);
			cout<<"angle: "<<angle<<'\n';
			long  double xcomp = cos(angle*M_PI/180.0)*increment;
			cout<<"xcomp: "<<xcomp<<'\n';
			long  double ycomp = sin(angle*M_PI/180.0)*increment;
			cout<<"ycomp: "<<ycomp<<'\n';
			
			long double x, y;
			x =this->centroid.x();
			y =this->centroid.y();
			
			
			//top right quadrant from centroid
			if((polyIterator[i].x()>centroid.x()) && (polyIterator[i].y()>centroid.y()) ){
				
				Point_2 newPoint(x+xcomp,y+ycomp);
				if(getDistance(newPoint, getCentroid() ) > this->distBetweenPoints){
					holder.push_back(newPoint);
				}
				else{
					cout<<"Point too close to center"<<endl;
				}
			}
			//top left quadrant from centroid
			else if((polyIterator[i].x()<centroid.x()) &&(polyIterator[i].y()>centroid.y())){
				Point_2 newPoint(x-xcomp,y+ycomp);
				if(getDistance(newPoint, getCentroid() ) > this->distBetweenPoints){
					holder.push_back(newPoint);
				}
				else{
					cout<<"Point too close to center"<<endl;
				}
			}
			//bottom right quadrant of centroid
			else if((polyIterator[i].x()>centroid.x()) && polyIterator[i].y()<centroid.y()){
				Point_2 newPoint(x+xcomp,y-ycomp);
				if(getDistance(newPoint, getCentroid() ) > this->distBetweenPoints){
					holder.push_back(newPoint);
				}
				else{
					cout<<"Point too close to center"<<endl;
				}
			}
			//Bottom left quadrant of centriod
			else{
				Point_2 newPoint(x-xcomp,y-ycomp);
				if(getDistance(newPoint, getCentroid() ) > this->distBetweenPoints){
					holder.push_back(newPoint);
				}
				else{
					cout<<"Point too close to center"<<endl;
				}
			}

			cout<<xcomp<<" , "<<ycomp<<'\n';
		 
		}
		if(holder.size() == 0){
			centroidFlag = false;
			holder.push_back(getCentroid());
			allTraces.push_back(holder);
		}
		else{
			allTraces.push_back(holder);
			polyIterator = holder;
		}
	}
	return allTraces;
	
}

//Puny constructor. It should probably load from some initialization file.
PathPlanner::PathPlanner(){

}


//Constructs the grid based on the polygon's bounding box and generates a random point if inside polygon
Points PathPlanner::getRandGridPoints(){
	//Grid
	CGAL::Bbox_2 box = this->BPpoly.bbox();
	long double xmini = box.xmin();
	long double ymini = box.ymin();
	long double xmaxi = box.xmax();
	long double ymaxi = box.ymax();
	cout<<xmini<<" "<<xmaxi<<" "<<ymini<<" "<<ymaxi<<'\n';
	Point_2 iterator;
	vector<Grid> grids;
	Points randGridPoints;
	for(long double i =ymini; i<ymaxi; i += this->gridSideLen ){
		for(long double j = xmini; j<xmaxi; j+= this->gridSideLen ){
			grids.push_back( Grid( Point_2(j,i) , this->gridSideLen) );
			cout<<"row: "<<i<<" column: "<<j<<'\n';
		}
	}
	for (int i =0 ; i <grids.size();i++){
		if(grids[i].is_bounded(BPpoly)){
			randGridPoints.push_back(grids[i].genRandPoint());
			grids[i].printGrid();
		}
	}
	return randGridPoints;

}

//Rapidly Exploring Random Tree Search
Points PathPlanner::RRT(){
	try{
		srand(time(NULL));
		Points randPoints = getRandGridPoints();
		printPoints(randPoints);
		randPoints = this->TSP(randPoints);
		setPath(randPoints);
		return randPoints;
	}
	catch(const bad_alloc&){
		cout<<"Something really bad happened!";
		//return NULL;
	}
	
}

list<Point_2> PathPlanner::vector2List(Points vPoints){
	list<Point_2> buffer;
	copy(vPoints.begin(), vPoints.end(), back_inserter(buffer));
	return buffer;
}

//Uses a greedy nearest neighbor algorithm
Points PathPlanner::TSP(Points unorderedPoints){
	list<Point_2> pList =vector2List(unorderedPoints);
	Points randPath;
	Point_2 firstPoint = unorderedPoints[0];
	CGAL::Point_set_2<K> pSet;
	pSet.insert(pList.begin(),pList.end());
	Vertex_handle handler = pSet.nearest_neighbor(firstPoint);
	//pSet.remove(firstPoint);
	randPath.push_back(firstPoint);
	while(pSet.number_of_vertices()!= 0){
		Point_2 iterator = handler->point();
		pSet.remove(handler);
		randPath.push_back(iterator);
		handler = pSet.nearest_neighbor(iterator);
	}
	return randPath;
}

//Silly, arrogant sequence {1,1,2,2,3,3,...,n,n}*distBetweenPoints
vector<int> PathPlanner::genSpiralSequence(int seqSize){
	vector<int> sequence;
	for(int i =1; i < seqSize ; i++){
		sequence.push_back(i*this->distBetweenPoints);
		sequence.push_back(i*this->distBetweenPoints);
	}
	return sequence;
}

//Generates a square spiral path using a silly number sequence {1,1,2,2,3,3,4,4,...,n,n}*distBetweenPoints
Points PathPlanner::getSpiralPath(){
	//It is unlikely that the user will request more that 4000 points
	vector<int> seq = genSpiralSequence(2000);
	Points spiral;
	Point_2 buffer= this->centroid;
	spiral.push_back(buffer);
	for(int i =0; i < INT_MAX; i++){
		//Establishes the order of spiral: up, left, down, right, up....
		int hashcode = i % 4;
		switch(hashcode){
			case 0:
				buffer =Point_2( buffer.x(),buffer.y()+seq[i]);
				break;
			case 1:
				buffer = Point_2(buffer.x()-seq[i],buffer.y());
				break;
			case 2:
				buffer = Point_2(buffer.x(), buffer.y()-seq[i]);
				break;
			case 3:
				buffer = Point_2(buffer.x()+seq[i],buffer.y());
				break;
		}
		if(CGAL::ON_UNBOUNDED_SIDE!= BPpoly.bounded_side(buffer)){
			spiral.push_back(buffer);
		}
		else{
			return spiral;
		}
	}
}

void PathPlanner::calcPathLength(){
	long double totalDist = 0;
	for(int i = 0; i < (path.size()-2); ++i){
		totalDist += getDistance(path[i],path[i+1]);
	}
	this->pathLength = totalDist;
}

//Initialize critical data members
void PathPlanner::initialize(){
	Points points, result;
	
	points = this->setBPs();
	
	result = this->getHull(points);
	this->BPs = result;
	this->BPpoly = makePoly(result);
	Polygon_2 test = this->makePoly(result);
	this->setCentroid(this->getCentroid(test));
	cout<<"\nCentroid: "<<this->centroid.x()<<" , "<<this->centroid.y()<<endl;
}


//*********************************************************************************
//		Interface Section

//In a perfect world these would have some kind of error checking. I like to live dangerously....
int PathPlanner::runTrace(){
	vector<Polygon_2> all = this->linearShrink();
	this->setPath(all);
	calcPathLength();
	writePath();
	writeMetadata();
	cout<<"Estimated path length: "<<this->pathLength<<"\n";
	return 0;
}

int PathPlanner::runSquareSpiral(){
	Points spiralpath = this->getSpiralPath();	
	this->setPath(spiralpath);
	calcPathLength();
	writePath();
	writeMetadata();
	cout<<"Estimated path length: "<<this->pathLength<<"\n";
	return 0;
}

int PathPlanner::runRRT(){
	this->RRT();
	calcPathLength();
	writePath();
	writeMetadata();
	cout<<"Estimated path length: "<<this->pathLength<<"\n";
	return 0;
}

void PathPlanner::mainMenu(){
	cout<<"\n\nWelcome to Eric's Caffeine Induced Path Planner!\n";
	
	bool choiceFlag= true;
	string choice;

	while(choiceFlag){
		cout<<"Please select from the following menu options.\n\n";
		cout<<"1: Run Inner Trace Algorithm\n";
		cout<<"2: Run a silly Square Spiral\n";
		cout<<"3: Run a Rapidly Exploring Random Tree Search Algorithm\n";

		cin>>choice;
		if(isdigit(choice[0])){
			//Really dirty hack to get the int value. Im sorry :(
			int option= choice[0] - '0';
			switch(option){
				case 1:
				runTrace();
				break;

				case 2:
				runSquareSpiral();
				break;
				
				case 3:
				runRRT();
				break;
			}
		}
		else if(choice[0] == 'q' || choice[0] =='Q'){
			cout<<"\n\nThanks for using Eric's Caffeine Induced Path Planner!\n";
			cout<<"Bye\n";
			break;
		}
		else{
			cout<<"Please enter a number listed on the options menu!\n";
		}
	}
}

int main()
{
	// Points points, result;
	PathPlanner planner;
	planner.initialize();
	planner.mainMenu();

	return 0;
}
	