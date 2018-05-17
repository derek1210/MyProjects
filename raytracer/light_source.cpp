/***********************************************************
	
	Starter code for Assignment 3

	Implements light_source.h

***********************************************************/

#include <cmath>
#include <math.h>
#include "light_source.h"

void PointLight::shade(Ray3D& ray) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  
	Vector3D unitNorm = ray.intersection.normal;   

	Vector3D rayDir =  -1 * ray.dir;
	rayDir.normalize();    

	Vector3D light = pos - ray.intersection.point;
	light.normalize();

    Color ambient = ray.intersection.mat -> ambient;    
	Color diffuse = fmax(0.0, unitNorm.dot(light)) * ray.intersection.mat->diffuse; 

	Vector3D reflection = (-1 * rayDir + 2 * (unitNorm.dot(rayDir)) * unitNorm);

	Vector3D R = reflection;
	R.normalize();

	/*
	Vector3D u = R.cross(ray.intersection.normal); 
	u.normalize();
	Vector3D v = R.cross(u); 
	v.normalize();

	// Choose uniformly sampled random direction to send the ray in
	double theta = M_PI * (rand() % 10);
	double phi = M_PI * (rand() % 10); 
	double x = sin(theta) * cos(phi);
	double y = sin(theta) * sin(phi);
	double z = cos(theta);
	// Convert sample to world coordinates using the orthonormal basis
	R = x * u + y * v + z * R;
	R.normalize();	
	*/

	double specular_factor = pow(fmax(0.0, R.dot(light)), ray.intersection.mat->specular_exp);  
	Color specular = specular_factor * ray.intersection.mat->specular;

	if (!ray.intersection.mat->texture_map) {
		ray.col = ray.col + (col_ambient * ambient + col_diffuse * diffuse + col_specular * specular);
		ray.col.clamp();
	}
}

