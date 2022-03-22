#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector> //std::vector
#include <cmath> //sqrt
#include <pthread.h>

using namespace std; //std::

//#define THREAD_NUM 4

/*----------------- 2D point structure (x,y) ------------------*/
class Point{
    public:
        double x, y;
};
/*------------------------------------------------------------*/

//typedef struct Task {
//    void (*taskFunction)(vector<Point>& , vector<Point>, Point, Point){};
//    vector<Point> arg1;
//	vector<Point> arg2;
//	Point arg3;
//	Point arg4;
//
//} Task;
//
//
//Task taskQueue[256];
//int taskCount = 0;
//
//pthread_mutex_t mutexQueue;
//pthread_cond_t condQueue;
//
//void executeTask(Task* task) {
//    task->taskFunction(task->arg1, task->arg2, task->arg3, task->arg4);
//}
//
//void submitTask(Task task) {
//    pthread_mutex_lock(&mutexQueue);
//    taskQueue[taskCount] = task;
//    taskCount++;
//    pthread_mutex_unlock(&mutexQueue);
//    pthread_cond_signal(&condQueue);
//}
//
//[[noreturn]] void* startThread(void* args) {
//    while (true) {
//        Task task;
//
//        pthread_mutex_lock(&mutexQueue);
//        while (taskCount == 0) {
//            pthread_cond_wait(&condQueue, &mutexQueue);
//        }
//
//        task = taskQueue[0];
//        int i;
//        for (i = 0; i < taskCount - 1; i++) {
//            taskQueue[i] = taskQueue[i + 1];
//        }
//        taskCount--;
//        pthread_mutex_unlock(&mutexQueue);
//        executeTask(&task);
//    }
//}

/*-------------------- input and output ----------------------*/
void get_input(vector<Point> &p){
    Point aux{}; //does the pushback
    //fill the vector from an input file (syntax of a line: double double)
    while (fscanf(stdin, "%lf", &aux.x) != EOF) {    // read a double: x coordinate
	    getc(stdin);                             // read a single space character
	    fscanf(stdin, "%lf", &aux.y);            // read a double: y coordinate
	    p.push_back(aux);
    }
}

void print_vector_of_points(vector<Point> &p){
	for(auto & i : p){
		cout << i.x << " " << i.y << "\n";
	}
	cout << "=================================\n";
}
/*-------------------------------------------------------------*/

/*--------------------- Point operations ----------------------*/
// sums two vectors
Point vector_sum(Point a, Point b){
	Point sum{};
	sum.x = a.x + b.x;
	sum.y = a.y + b.y;
	return sum;
}

// the product of a vector by a scalar
Point scalar_product(double l, Point a){
	Point product{};
	product.x = l * a.x;
	product.y = l * a.y;
	return product;
}

// cross product (only Z-axis value)
double cross_product(Point a, Point b){
    return (((a.x)*(b.y)) - ((b.x)*(a.y)));
}

// dot product
double dot_product(Point a, Point b){
    return (((a.x)*(b.x)) + ((a.y)*(b.y)));
}

// euclidean norm: || . ||
double norm(Point a){
    return (sqrt(dot_product(a,a)));
}

// euclidean distance: || b - a ||
double euclidean_distance(Point a, Point b){
	return(norm(vector_sum(b,scalar_product(-1,a))));
}

// the shortest distance between a point and a line (defined by two points)
// see references (1)
double line_distance(Point a, Point p1, Point p2){
	return(abs(((p2.y - p1.y)*a.x)-((p2.x - p1.x)*a.y)+(p2.x * p1.y)-(p2.y * p1.x))/euclidean_distance(p1,p2));
}
/*-------------------------------------------------------------*/

/*-------------------- auxiliary functions --------------------*/
// says if a point an is on the left side, right side or ir collinear to the oriented line p1->p2
// see references (2)
int set_local(Point a, Point p1, Point p2){
	//calculates:	p1->p2 x p1->a (the cross product between the vectors)
	double position = cross_product((vector_sum(p2,scalar_product(-1,p1))),(vector_sum(a,scalar_product(-1,p1))));
	if(position > 0){
		return -1; //point c is at the left of the line p2->p1
	}
	else if(position < 0){
		return 1;  //point c is at the left of the line p1->p2
	}
	return 0; 	   //the point c is collinear to the line p1->p2
}

//returns the farthest point from a Set of points and a line (defined by two points)
Point farthest_point(const vector<Point>& S, Point p1, Point p2){
	double max_value = 0, aux;
	Point max_point{};
	// calculates and compares distance of points
	for(auto & i : S){
		aux = line_distance(i,p1,p2);
		if(max_value <= aux){
			max_value = aux;
			max_point = i;
		}
	}
	return max_point;
}

//find the set of points on the left side of a line (defined by two points)
void find_left_set(const vector<Point>& P, vector<Point>& S, Point p1, Point p2){
	int flag;
	for(auto & i : P){
		flag = set_local(i,p1,p2);
		if(flag==1){
			S.push_back(i);
		}
	}
}
/*-------------------------------------------------------------*/

/*-------------------- The QuickHull Algorithm Functions --------------------*/
/*---------------------------- see reference (0) ----------------------------*/
void find_hull(vector<Point>& CH, const vector<Point>&S, Point p1, Point p2){
	//creates two regions
	vector<Point> S1,S2;
	//the farthest point
	Point c{};

	//if S is empty there's no hull to be found
	if(S.empty()) return;

	//finds the farthest point from the line p1->p2
	c = farthest_point(S,p1,p2);

	//separates the set S in three regions
	find_left_set(S,S1,p1,c);
	find_left_set(S,S2,c,p2);

	find_hull(CH,S1,p1,c);
	CH.push_back(c); //important to be here due to printing order
	find_hull(CH,S2,c,p2);
}

void quick_hull(vector<Point>& CH, vector<Point> P){
	// Find points with smallest and biggest value for x (leftmost and rightmost points)
	Point min = *(P.begin());
	Point max = *(P.begin());

	for (auto & i : P) {
		if (i.x < min.x) {
			min = i;
		}
		if (i.x > max.x) {
			max = i;
		}
	}

	vector<Point> S1,S2;

	//S1: points at the left of line min->max
	find_left_set(P,S1,min,max);
	//S2: points at the left of line max->min
	find_left_set(P,S2,max,min);

	//adds left most point to the convex hull
	CH.push_back(min); //important to be here due to printing order

	find_hull(CH,S1,min,max);

    CH.push_back(max); //important to be here due to printing order

    //adds right most point to the convex hull
    find_hull(CH,S2,max,min);

    CH.push_back(min); //important to be here due to printing order

//    pthread_t th[THREAD_NUM];
//    pthread_mutex_init(&mutexQueue, nullptr);
//    pthread_cond_init(&condQueue, nullptr);

//    int i;
//    for (i = 0; i < THREAD_NUM; i++) {
//        if (pthread_create(&th[i], nullptr, &startThread, nullptr) != 0) {
//            perror("Failed to create the thread");
//        }
//    }

//    for (i = 0; i < 100; i++) {
//        Task t = {
//                //.taskFunction = i % 2 == 0 ? &sum : &product,
//                .taskFunction = &find_hull,
//                .arg1 = CH,
//                .arg2 = S1,
//                .arg3 = min,
//                .arg4 = max
//        };
//        submitTask(t);
//    }

//    Task t2 = {
//            //.taskFunction = i % 2 == 0 ? &sum : &product,
//            .taskFunction = &find_hull,
//            .arg1 = CH,
//            .arg2 = S2,
//            .arg3 = max,
//            .arg4 = min
//
//    };
// submitTask(t2);

//    for (i = 0; i < THREAD_NUM; i++) {
//        if (pthread_join(th[i], nullptr) != 0) {
//            perror("Failed to join the thread");
//        }
//    }

//    pthread_mutex_destroy(&mutexQueue);
//    pthread_cond_destroy(&condQueue);

}
/*-------------------------------------------------------------*/


int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]){
	//the input
	vector<Point> input;
	//the convex hull (output)
	vector<Point> convex_hull;

	get_input(input);
	cout << "\n------------- INPUT -------------\n";
	print_vector_of_points(input);

	quick_hull(convex_hull, input);

	cout << "\n========== CONVEX HULL ==========\n";
	print_vector_of_points(convex_hull);

	//cleans memory
	convex_hull.clear();
	input.clear();

	//ends program
	return 0;
}
