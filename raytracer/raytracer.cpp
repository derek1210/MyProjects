	
/***********************************************************
	Starter code for Assignment 3

	Implementations of functions in raytracer.h, 
	and the main function which specifies the scene to be rendered.	

***********************************************************/
#define ANTIALIASING false
#define RAY_TRACING_DEPTH 4
#define GLOSSY_REFLECTION true
#define HARD_SHADOW true
#define ENV_MAPPING true
#define REFRACTION true //Not Working :(

#include "raytracer.h"
#include <cmath>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include "string.h"

void Raytracer::traverseScene(Scene& scene, Ray3D& ray)  {
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

	//Check for Env mapping
	if(node -> obj){
			if (node->obj->intersect(ray, node->worldToModel, node->modelToWorld)) {
				ray.intersection.mat = node->mat;
			}
		}
	}
}

void Raytracer::computeTransforms(Scene& scene) {
	// right now this method might seem redundant. But if you decide to implement 
	// scene graph this is where you would propagate transformations to child nodes
		
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

		node->modelToWorld = node->trans;
		node->worldToModel = node->invtrans; 
	}
}

void Raytracer::computeShading(Ray3D& ray, LightList& light_list) {
	for (size_t  i = 0; i < light_list.size(); ++i) {
		LightSource* light = light_list[i];
		
		// Each lightSource provides its own shading function.
		// Implement shadows here if needed.
		light->shade(ray);        
	}
}

//Helper function
int Raytracer::find_env(Scene& scene) {
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];
		if (!node->obj) {
			return i;
		}
	}
	return 0;
}

//Helper function
Color Raytracer::textureColor(Ray3D &ray){
	int texture_width = ray.intersection.mat->texture_width;
	int texture_height = ray.intersection.mat->texture_height;
	unsigned char *rarray = ray.intersection.mat->rarray;
	unsigned char *garray = ray.intersection.mat->garray;
	unsigned char *barray = ray.intersection.mat->barray;
			
	Point3D point(ray.intersection.modelPoint);
	double u;
	double v;
			
	if (!strcmp(ray.intersection.intersect_object, "UnitSphere")) {
		u = 0.5 + (atan2(point[2], point[0]))/(2*M_PI);
		v = 0.5 - (asin(point[1]))/M_PI;
	} else if (!strcmp(ray.intersection.intersect_object, "UnitSquare")) {
		u = 0.5 + point[0];
		v = 0.5 + point[1];
	}
	int x = (int) (u * texture_width);
	int y = (int) (v * texture_height);

	int index = (x * texture_width) + y;
	double r =  ((double) rarray[index])/255.0;
	double g = ((double) garray[index])/255.0;
	double b = ((double) barray[index])/255.0;
	return Color(r,g,b)	;
}

Color Raytracer::shadeRay(Ray3D& ray, Scene& scene, LightList& light_list, int depth) {
	Color col(0.0, 0.0, 0.0); 
	traverseScene(scene, ray); 

	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {
		computeShading(ray, light_list);
		
		if (HARD_SHADOW){
			//Casting shadows
			Ray3D shadowRay;
			for (size_t i = 0; i < light_list.size(); ++i) {
				LightSource* light = light_list[i];

				shadowRay.origin = ray.intersection.point + 0.0000000001 * ray.intersection.normal;

				//Randomized soft shadows
				double r = 4 * ((double) rand() / (RAND_MAX));
    			double theta = 2 * M_PI * rand();
    			double phi = 2 * M_PI * rand();
    			Point3D randomLightPos = light -> get_position() + 
				Vector3D(r*cos(theta)*sin(phi),r*sin(theta)*sin(phi),r*cos(phi));

				shadowRay.dir =  randomLightPos - shadowRay.origin;
				shadowRay.dir.normalize();
				shadowRay.intersection.none = true;
				traverseScene(scene, shadowRay);
				
				//Account for floating point error
				if (shadowRay.intersection.none || shadowRay.intersection.t_value < 0.00001) {
					if (ray.intersection.mat->texture_map) {
						col = col + textureColor(ray);
					} else {
						col = col + ray.col;
					}
				}
			}
		} else  { //Not hard shadow
			if (ray.intersection.mat->texture_map) {
				col = col + textureColor(ray);
			} else {
				col = col + ray.col;
			}
		}

		//Resursively do ray tracing for part B.
		if(depth > 1){
			
			if(GLOSSY_REFLECTION){
				Color colR(0.0, 0.0, 0.0);
				for(int k = 0; k < 10 ; k++){
					// Choose uniformly sampled random direction to send the ray in
					Ray3D reflectRay;
					reflectRay.intersection.none = true;
					reflectRay.origin = ray.intersection.point + 0.0000000001 * ray.intersection.normal;	
					ray.dir.normalize();
					Vector3D R = ray.dir + 2 * (ray.intersection.normal.dot(-1 * ray.dir)) * ray.intersection.normal;
					R.normalize();
					Vector3D u = R.cross(ray.intersection.normal); 
					u.normalize();
					Vector3D v = R.cross(u); 
					v.normalize();
					
					double std = 0.3;
					//Sampling Gassuian using Marsaglia polar method
					double v1, v2, s;	
					do {
						v1 = 2 * ((double) rand() / (RAND_MAX)) - 1;
						v2 = 2 * ((double) rand() / (RAND_MAX)) - 1;
						s = v1 * v1 + v2 * v2;
					} while (s >= 1.0 || s == 0);
					
					s = sqrt((-2.0 * log(s)) / s);
					double a = v1 * s * std;
					
					double b = ((double) rand() / (RAND_MAX));
					double roughness = 10;
					double theta =  a;//acos(pow(1 - a, 1/roughness));
					double phi = 2 * M_PI * b;
					double x = sin(theta) * cos(phi) / 5;
					double y = sin(theta) * sin(phi) / 5;
					double z = cos(theta);

					// Convert sample to world coordinates using the orthonormal basis
					R = x * u + y * v + z * R;
					
					R.normalize();
					reflectRay.dir = R;
					double reflection_factor = ray.intersection.mat -> specular_exp / 200;
					colR = colR + reflection_factor * shadeRay(reflectRay, scene, light_list, depth - 1); 
				}
				col = col + 0.1 * colR;
			} else {
				Ray3D reflectRay;
				reflectRay.intersection.none = true;
				reflectRay.origin = ray.intersection.point + 0.0000000001 * ray.intersection.normal;	
				ray.dir.normalize();
					
				Vector3D R = ray.dir + 2 * (ray.intersection.normal.dot(-1 * ray.dir)) * ray.intersection.normal;
				R.normalize();
				reflectRay.dir = R;
				double reflection_factor = ray.intersection.mat -> specular_exp / 200;
				col = col + reflection_factor * shadeRay(reflectRay, scene, light_list, depth - 1); 

				if(REFRACTION){
					Ray3D refractionRay;
					refractionRay.intersection.none = true;
					refractionRay.origin = ray.intersection.point + 0.0000001 * ray.dir;	
					double c2 = ray.intersection.mat -> refraction_index;
					if(c2){
						double costheta1 = ((-1 * ray.dir).dot(ray.intersection.normal));
						double theta1 = acos(costheta1);
						double theta2 = asin(c2 * sin(theta1));

						if(1){
							refractionRay.dir = c2 * ray.dir + (c2 * cos(theta1) - cos(theta2)) * ray.intersection.normal;
							refractionRay.dir.normalize();
							col = col +  shadeRay(refractionRay, scene, light_list, depth - 1); 
						}
					}
				}
			}	
		} 
	} else {
		if(ENV_MAPPING){
			int index;
			float u, v;
			unsigned long int env_width;
			long int env_height;
			unsigned char *rarray, *garray, *barray;
			bool bmp;
		
			int env = find_env(scene);
			SceneNode *env_map = scene[env];

			Vector3D dir(ray.dir);
			dir.normalize();
			convert_xyz_to_cube_uv(dir[0], dir[1], dir[2], &index, &u, &v);
			switch (index) {
				case 0: // POSITIVE X
					env_width = env_map->posx->texture_width;
					env_height = env_map->posx->texture_height;
					rarray = env_map->posx->rarray;
					garray = env_map->posx->garray;
					barray = env_map->posx->barray;
					break;	
				case 1: // NEGATIVE X
					env_width = env_map->negx->texture_width;
					env_height = env_map->negx->texture_height;
					rarray = env_map->negx->rarray;
					garray = env_map->negx->garray;
					barray = env_map->negx->barray;
					break;	
				case 2: // POSITIVE Y
					env_width = env_map->posy->texture_width;
					env_height = env_map->posy->texture_height;
					rarray = env_map->posy->rarray;
					garray = env_map->posy->garray;
					barray = env_map->posy->barray;
					break;	
				case 3: // NEGATIVE Y
					env_width = env_map->negy->texture_width;
					env_height = env_map->negy->texture_height;
					rarray = env_map->negy->rarray;
					garray = env_map->negy->garray;
					barray = env_map->negy->barray;
					break;	
				case 4: // POSITIVE Z
					env_width = env_map->posz->texture_width;
					env_height = env_map->posz->texture_height;
					rarray = env_map->posz->rarray;
					garray = env_map->posz->garray;
					barray = env_map->posz->barray;
					break;	
				case 5: // NEGATIVE Z
					env_width = env_map->negz->texture_width;
					env_height = env_map->negz->texture_height;
					rarray = env_map->negz->rarray;
					garray = env_map->negz->garray;
					barray = env_map->negz->barray;
					break;	
			}
			int x = (int) (v * env_width);
			int y = (int) (u * env_height);
			
			int i = (x * env_width) + y;
			double r =  ((double) rarray[i])/255.0;
			double g = ((double) garray[i])/255.0;
			double b = ((double) barray[i])/255.0;
		
			col = col + Color(r, g, b);
		}
	}
	col.clamp();
	return col; 
}	

void Raytracer::render(Camera& camera, Scene& scene, LightList& light_list, Image& image) {
	computeTransforms(scene);

	Matrix4x4 viewToWorld;
	double factor = (double(image.height)/2)/tan(camera.fov*M_PI/360.0);

	viewToWorld = camera.initInvViewMatrix();

	// Construct a ray for each pixel.
	#pragma omp parallel for
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) {
			// Sets up ray origin and direction in view space, 
			// image plane is at z = -1.
			Point3D origin(0, 0, 0);
			Point3D imagePlane;

			Color col;

			if(ANTIALIASING){
				//Anti-alising by taking average of 10 points each pixel with Jitter Method
				for (double k = 0; k < 10; k++)  {
					double xOffset = 2 * ((double) rand() / (RAND_MAX)) - 0.5; //Expected value is still 0.5
					double yOffset = 2 * ((double) rand() / (RAND_MAX)) - 0.5; 
					imagePlane[0] = (-double(image.width)/2 + xOffset + j)/factor;
					imagePlane[1] = (-double(image.height)/2 + yOffset + i)/factor;
					imagePlane[2] = -1;
					Ray3D ray;


					ray.origin = viewToWorld * origin;
					ray.dir = viewToWorld * (imagePlane - origin);
					ray.intersection.none = true;
					col = col + shadeRay(ray, scene, light_list, RAY_TRACING_DEPTH);
				}
				col = 0.1 * col;
			} else 
			{
				imagePlane[0] = (-double(image.width)/2 + 0.5 + j)/factor;
				imagePlane[1] = (-double(image.height)/2 + 0.5 + i)/factor;
				imagePlane[2] = -1;
				Ray3D ray;
						

				ray.origin = viewToWorld * origin;
				ray.dir = viewToWorld * (imagePlane - origin);
				ray.intersection.none = true;
				col = shadeRay(ray, scene, light_list, RAY_TRACING_DEPTH);
			}
			image.setColorAtPixel(i, j, col);			
		}
	}
}

