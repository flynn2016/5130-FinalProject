#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "image_texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

vec3 color(const ray& r, hitable *world,int depth){
    hit_record rec;
    if(world->hit(r,0.0,MAXFLOAT,rec)){
        ray scattered;
        vec3 attenuation;
        if(depth<50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation * color (scattered, world, depth+1);
        }
        else{
            return vec3(0,0,0);
        }
    }
    else{
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y()+1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t * vec3 (0.5,0.7,1.0);
    }
}



int main(){
    std::ofstream myFile;
    myFile.open("sphere.ppm");
    //pixel number
    int nx = 800;
    int ny = 400;
    int ns = 10;
    myFile << "P3\n" << nx << " " << ny << "\n255\n";
    
    
    //objects
    hitable *list[7];
    float R = cos(M_PI/4);
    texture *checker = new checker_texture( new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    texture *stripe = new stripe_texture( new constant_texture(vec3(0.2,0.6, 0.6)), new constant_texture(vec3(0.6, 0.6, 0.9)));
    texture *test = new test_texture( new constant_texture(vec3(0.8,0.6, 0.6)), new constant_texture(vec3(0.9, 0.5, 0.5)));
    texture *pertext1 = new noise_texture(2);
    texture *pertext2 = new noise_texture(4);
    texture *pertext3 = new noise_texture(8);
    texture *pertext4 = new noise_texture(16);
    texture *pertext5 = new noise_texture(32);
    
    texture *diffuse = new constant_texture(vec3(0.5,0.5,0.5));
    
    int n_x, n_y, n_n;
    unsigned char *tex_data = stbi_load("texture.jpg", &n_x, &n_y, &n_n, 0);
    material *mat =  new lambertian(new image_texture(tex_data, n_x, n_y));
    
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(diffuse));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(checker));
    list[2] = new sphere(vec3(2,0,-2), 0.5, new metal(vec3(0.33, 0.9, 0.73), 0.0));
    list[3] = new sphere(vec3(-2,0,-2), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-2,0,-2), -0.45, new dielectric(1.5));
    list[5] = new sphere(vec3(1,0,-1.5), 0.5, new lambertian(test));
    list[6] = new sphere(vec3(-1,0,-1.5), 0.5, new lambertian(pertext1));
//    list[7] = new sphere(vec3(0,1, -1), 0.5, mat);
    hitable *world = new hitable_list(list,7);
    
//    list[0] = new sphere(vec3(-2,0,-1), 0.5, new lambertian(pertext5));
//    list[1] = new sphere(vec3(-1,0,-1), 0.5, new lambertian(pertext4));
//    list[2] = new sphere(vec3(0,0,-1), 0.5, new lambertian(pertext3));
//    list[3] = new sphere(vec3(1,0,-1), 0.5, new lambertian(pertext2));
//    list[4] = new sphere(vec3(2,0,-1), 0.5, new lambertian(pertext1));
//    list[5] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(checker));
//    hitable *world = new hitable_list(list,6);
    
    vec3 lookfrom(0,1,-10);
    vec3 lookat(0,0,-1);
    float dist_to_focus = 10.0;
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);
    
    for(int j = ny-1; j >=0; j--){
        for(int i = 0; i<nx; i++){
            vec3 col(0,0,0);
            for(int s=0; s<ns;s++){
                float u = float(i+drand48())/float(nx);
                float v = float(j+drand48())/float(ny);
                ray r =cam.get_ray(u,v);
                col += color(r,world,0);
            }
            col /=float(ns);
            col = vec3(sqrt(col.x()),sqrt(col.y()),sqrt(col.z()));
            int ir = int(255.99*col.x());
            int ig = int(255.99*col.y());
            int ib = int(255.99*col.z());
            myFile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myFile.close();
}
