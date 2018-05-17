/***********************************************************
	
	Starter code for Assignment 3
	
	Implements scene_object.h

***********************************************************/

#include <cmath>
#include <math.h>
#include <algorithm> //for useage of min
#include "scene_object.h"
#include "string.h"

bool UnitSquare::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Ray3D rayModelSpace = ray;
	rayModelSpace.origin = worldToModel * ray.origin;
	rayModelSpace.dir = worldToModel * ray.dir;
	rayModelSpace.dir.normalize();

	double origin_z = rayModelSpace.origin[2];
	double dir_z = rayModelSpace.dir[2];
	double factor = (-1 * origin_z)/ dir_z;
	if(factor < 0 || dir_z == 0){
		return false;
	} else {
		
		// (-1 * origin_z)/ dir_z  * dir = (-1 * origin_z) 
		// (-1 * origin_z) + origin_z = 0
		Point3D intersectionPoint = (rayModelSpace.origin +  factor * rayModelSpace.dir);

		if(abs(intersectionPoint[0]) < 0.5 && abs(intersectionPoint[1]) < 0.5){
			
			
			//Calculate t value again in world space.
			double t = (modelToWorld * intersectionPoint - ray.origin)[2] / ray.dir[2];

			if((!ray.intersection.none) && (ray.intersection.t_value < t)){
			return false;
			}
			ray.intersection.modelPoint = intersectionPoint;

			ray.intersection.point = modelToWorld * intersectionPoint;
			ray.intersection.normal = worldToModel.transpose() * Vector3D(0,0,1);
			ray.intersection.t_value = t;
			ray.intersection.none = false;
			ray.intersection.normal.normalize();

			ray.intersection.intersect_object = (char *) malloc(sizeof(char) * strlen("UnitSquare"));
			strcpy(ray.intersection.intersect_object, "UnitSquare");
			return true;	
		} else {
			return false;
		}
	}
}

bool UnitSphere::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Ray3D rayModelSpace = ray;
	rayModelSpace.origin = worldToModel * ray.origin;
	rayModelSpace.dir = worldToModel * ray.dir; 
	rayModelSpace.dir.normalize();

	//Base on slides
	double A = rayModelSpace.dir[0] * rayModelSpace.dir[0] + 
			   	rayModelSpace.dir[1] * rayModelSpace.dir[1] +
			   rayModelSpace.dir[2] * rayModelSpace.dir[2];

	double B = 2 * (rayModelSpace.origin[0] * rayModelSpace.dir[0] + 
			   rayModelSpace.origin[1] * rayModelSpace.dir[1] +
			   rayModelSpace.origin[2] * rayModelSpace.dir[2]);

	double C = rayModelSpace.origin[0] * rayModelSpace.origin[0] + 
			   rayModelSpace.origin[1] * rayModelSpace.origin[1] +
			   rayModelSpace.origin[2] * rayModelSpace.origin[2] - 1 ;
	
	if ((B*B - (4 * A * C)) < 0){
		return false;
	} else {
		double t_1 = (-B + sqrt(B*B - 4 * A * C)) / (2 * A);
		double t_2 = (-B - sqrt(B*B - 4 * A * C)) / (2 * A);

		double t = fmin(t_1, t_2);
		if(t < 0){
			return false;
		}
		
		Point3D intersectionPoint =  (rayModelSpace.origin + (t  * rayModelSpace.dir));

		t = ((modelToWorld * intersectionPoint) - ray.origin)[2] / ray.dir[2];
		if((!ray.intersection.none) && (ray.intersection.t_value < t)){
			return false;
		}

		ray.intersection.point = intersectionPoint;
		ray.intersection.normal = worldToModel.transpose() * Vector3D(ray.intersection.point[0],
											ray.intersection.point[1],ray.intersection.point[2]);
		ray.intersection.modelPoint = ray.intersection.point;							
		ray.intersection.point = modelToWorld * ray.intersection.point;
		
		ray.intersection.none = false;
		ray.intersection.t_value = t;
		ray.intersection.normal.normalize();

		ray.intersection.intersect_object = (char *) malloc(sizeof(char) * strlen("UnitSphere"));
		strcpy(ray.intersection.intersect_object, "UnitSphere");
		
		return true;
	}
}

void SceneNode::rotate(char axis, double angle) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
			this->trans = this->trans*rotation; 	
			angle = -angle;
		} 
		else {
			this->invtrans = rotation*this->invtrans; 
		}	
	}
}

void SceneNode::translate(Vector3D trans) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	this->trans = this->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	this->invtrans = translation*this->invtrans; 
}

void SceneNode::scale(Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	this->trans = this->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	this->invtrans = scale*this->invtrans; 
}


