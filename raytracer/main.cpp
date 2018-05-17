/***********************************************************
	
	Starter code for Assignment 3

***********************************************************/
#define MLTI_LIGHT false

#include <cstdlib>
#include "raytracer.h"

int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;
	LightList light_list;
	Scene scene;   

	int width = 320 * 3;
	int height = 240 * 3;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}
	
	// Define materials for shading.
	Material gold(Color(0.3, 0.3, 0.3), Color(0.75164,0.60648,0.22648),
		Color(0.628281, 0.555802, 0.366065),
		51.2);
	gold.texture_map = false;
	gold.refraction_index = 0;
	Material jade(Color(0, 0, 0), Color(0.54,0.89,0.63),
		Color(0.316228,0.316228,0.316228),
		12.8);
	jade.texture_map = false;
	jade.refraction_index = 0;
	Material mirror(Color(0, 0, 0), Color(0.0,0.0,0.0),
		//Color(0.316228,0.316228,0.316228),
		Color(0, 0, 0),
		200);
	mirror.texture_map = false;
	mirror.refraction_index = 0;

	Material mirrorRefract(Color(0, 0, 0), Color(0.0,0.0,0.0),
		//Color(0.316228,0.316228,0.316228),
		Color(0, 0, 0),
		10);
	mirrorRefract.texture_map = false;
	mirrorRefract.refraction_index = 1.2;

	Material mirrorRefractStrong(Color(0, 0, 0), Color(0.0,0.0,0.0),
		//Color(0.316228,0.316228,0.316228),
		Color(0, 0, 0),
		200);
	mirrorRefractStrong.texture_map = false;
	mirrorRefractStrong.refraction_index = 1.8;

	unsigned long int texture_width, plane_texture_width;
	long int texture_height, plane_texture_height;
	unsigned char *rarray, *garray, *barray;
	unsigned char *plane_rarray, *plane_garray, *plane_barray;		
	bool bmp = bmp_read("./textures/bricks.bmp", &texture_width, &texture_height, &rarray, &garray, &barray);
	
	bool plane_bmp = bmp_read("./textures/chess1.bmp", &plane_texture_width, &plane_texture_height, &plane_rarray, &plane_garray, &plane_barray);

	Material textureMap(texture_width, texture_height, rarray, garray, barray);
	Material plane_textureMap(plane_texture_width, plane_texture_height, plane_rarray, plane_garray, plane_barray);
	
	//Load Env Mapping
	unsigned long int posx_width, negx_width, posy_width, negy_width,  posz_width, negz_width;
	long int posx_height, negx_height, posy_height, negy_height, posz_height, negz_height;
	
	unsigned char *posx_r, *posx_g, *posx_b;
	unsigned char *negx_r, *negx_g, *negx_b;
	unsigned char *posy_r, *posy_g, *posy_b;
	unsigned char *negy_r, *negy_g, *negy_b;
	unsigned char *posz_r, *posz_g, *posz_b;
	unsigned char *negz_r, *negz_g, *negz_b;

	bool posx_bmp = bmp_read("./cubemap/posx.bmp", &posx_width, &posx_height, &posx_r, &posx_g, &posx_b);
	bool negx_bmp = bmp_read("./cubemap/negx.bmp", &negx_width, &negx_height, &negx_r, &negx_g, &negx_b);
	bool posy_bmp = bmp_read("./cubemap/posy.bmp", &posy_width, &posy_height, &posy_r, &posy_g, &posy_b);
	bool negy_bmp = bmp_read("./cubemap/negy.bmp", &negy_width, &negy_height, &negy_r, &negy_g, &negy_b);
	bool posz_bmp = bmp_read("./cubemap/posz.bmp", &posz_width, &posz_height, &posz_r, &posz_g, &posz_b);
	bool negz_bmp = bmp_read("./cubemap/negz.bmp", &negz_width, &negz_height, &negz_r, &negz_g, &negz_b);

	Material posx(posx_width, posx_height, posx_r, posx_g, posx_b);
	Material negx(negx_width, negx_height, negx_r, negx_g, negx_b);
	Material posy(posy_width, posy_height, posy_r, posy_g, posy_b);
	Material negy(negy_width, negy_height, negy_r, negy_g, negy_b);
	Material posz(posz_width, posz_height, posz_r, posz_g, posz_b);
	Material negz(negz_width, negz_height, negz_r, negz_g, negz_b);

	// Defines a point light source.
	PointLight* pLight = new PointLight(Point3D(0,0,5), Color(0.9,0.9,0.9));
	light_list.push_back(pLight);


	if(MLTI_LIGHT){
		PointLight* pLight2 = new PointLight(Point3D(0,2,0), Color(0.5,0.5,0.5));
		light_list.push_back(pLight2);

		PointLight* pLight3 = new PointLight(Point3D(0,0,3), Color(0.5,0.5,0.5));
		light_list.push_back(pLight3);
	}
	
	SceneNode* plane = new SceneNode(new UnitSquare(), &plane_textureMap);
	scene.push_back(plane);
	
	SceneNode* plane2= new SceneNode(new UnitSquare(), &gold);
	//scene.push_back(plane2);

	// Add a unit square into the scene with material mat.
	SceneNode* sphere = new SceneNode(new UnitSphere(), &textureMap);
	scene.push_back(sphere);
	
	SceneNode* env = new SceneNode(&posx, &negx, &posy, &negy, &posz, &negz);
	scene.push_back(env);

	// Apply some transformations to the sphere and unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	sphere->translate(Vector3D(0, 0, -6));
	sphere->rotate('x', -45);
	sphere->rotate('z', 45);
	sphere->scale(Point3D(0, 0, 0), factor1);

	SceneNode* sphere2 = new SceneNode(new UnitSphere(), &mirrorRefractStrong);
	scene.push_back(sphere2);
	sphere2->translate(Vector3D(3, 0, -5));
	double factor3[3] = {0.5,0.5,0.5};
	sphere2->scale(Point3D(0, 0, 0), factor3);

	double factor2[3] = { 6.0, 6.0, 6.0 };
	plane->translate(Vector3D(0, 0, -7));
	plane->rotate('z', 45);
	plane->scale(Point3D(0, 0, 0), factor2);

	plane2->translate(Vector3D(-1, 2, -5 ));
	plane2->rotate('z', 45);
	plane2->rotate('x', 90);
	plane2->rotate('y', 0);
	
	plane2->scale(Point3D(0, 0, 0), factor2);

	SceneNode* sphere3 = new SceneNode(new UnitSphere(), &mirrorRefract);
	scene.push_back(sphere3);
	sphere3->translate(Vector3D(2, -2, -5));
	sphere3->scale(Point3D(0, 0, 0), factor3);

	SceneNode* sphere4 = new SceneNode(new UnitSphere(), &mirrorRefract);
	scene.push_back(sphere4);
	sphere4->translate(Vector3D(-3, 3, -5));
	sphere4->scale(Point3D(0, 0, 0), factor3);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	Camera camera1(Point3D(5, -5, -5), Vector3D(-1, 1, 0), Vector3D(0, 0, 1), 60.0);
	Image image1(width, height);
	raytracer.render(camera1, scene, light_list, image1); //render 3D scene to image
	image1.flushPixelBuffer("view1.bmp"); //save rendered image to file

	// Render it from a different point of view.
	Camera camera2(Point3D(4, 2, 1), Vector3D(-4, -2, -6), Vector3D(0, 1, 0), 60.0);
	Image image2(width, height);
	raytracer.render(camera2, scene, light_list, image2);
	image2.flushPixelBuffer("view2.bmp");

	// Free memory
	for (size_t i = 0; i < scene.size(); ++i) {
		delete scene[i];
	}

	for (size_t i = 0; i < light_list.size(); ++i) {
		delete light_list[i];
	}

	return 0;
}