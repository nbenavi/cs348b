
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// cameras/perspective.cpp*
#include "cameras/lightfield.h"
#include "paramset.h"
#include "sampler.h"
#include "sampling.h"
#include "light.h"
#include "stats.h"

namespace pbrt {

// // PerspectiveCamera Method Definitions
LightFieldCamera::LightFieldCamera(const AnimatedTransform &CameraToWorld,
                                     const Bounds2f &screenWindow,
                                     Float shutterOpen, Float shutterClose,
                                     Float lensRadius, Float focalDistance,
                                     Float fov, Film *film,
                                     const Medium *medium)
    : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),fov(fov){
     camera = new PerspectiveCamera(CameraToWorld, screenWindow, shutterOpen, shutterClose,
                                 lensRadius, focalDistance, fov, film, medium);
}

Float LightFieldCamera::GenerateRay(const CameraSample &sample,
                                     Ray *ray) const {
  
    ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute environment camera ray direction

    // Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    // Point3f pCamera = RasterToCamera(pFilm);
    // *ray = Ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));
    // ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    // ray->medium = medium;
    // *ray = CameraToWorld(*ray);
    // return 1;
    // std::cout << sample.pFilm.x << std::endl;

    int numPerRow = 2;

    float fovScale = std::tan(fov/2 * 3.1459 / 180) * 2;

    float y = 0;
    float x = 0;


    x = 2 * fovScale * sample.pFilm.x / film->fullResolution.x - fovScale;
    y = fovScale - 2 * fovScale * sample.pFilm.y / film->fullResolution.y;

    if(sample.pFilm.x < film->fullResolution.x/2){
        x = x + fovScale/2;

    }else {
        x = x + fovScale/2 - fovScale;

    }

    
    if(sample.pFilm.y < film->fullResolution.y/2){
        y = y - fovScale/2;

    }else {
        y = y - fovScale/2 + fovScale;
    }  
 

    

    Vector3f dir(x,y,1);
    
    *ray = Ray(Point3f(0, 0, 0), Normalize(dir));
    ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return 1;

}

// Float LightFieldCamera::GenerateRayDifferential(const CameraSample &sample,
//                                                  RayDifferential *ray) const {
//     return camera->GenerateRayDifferential(sample, ray);
// }

LightFieldCamera *CreateLightFieldCamera(const ParamSet &params,
                                           const AnimatedTransform &cam2world,
                                           Film *film, const Medium *medium) {
  // Extract common camera parameters from _ParamSet_
    Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
    Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
    if (shutterclose < shutteropen) {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
                shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }
    Float lensradius = params.FindOneFloat("lensradius", 0.f);
    Float focaldistance = params.FindOneFloat("focaldistance", 1e6);
    Float frame = params.FindOneFloat(
        "frameaspectratio",
        Float(film->fullResolution.x) / Float(film->fullResolution.y));
  
    Bounds2f screen;
    if (frame > 1.f) {
        screen.pMin.x = -frame;
        screen.pMax.x = frame;
        screen.pMin.y = -1.f;
        screen.pMax.y = 1.f;
    } else {
        screen.pMin.x = -1.f;
        screen.pMax.x = 1.f;
        screen.pMin.y = -1.f / frame;
        screen.pMax.y = 1.f / frame;
    }
    int swi;
    const Float *sw = params.FindFloat("screenwindow", &swi);
    if (sw) {
        if (swi == 4) {
            screen.pMin.x = sw[0];
            screen.pMax.x = sw[1];
            screen.pMin.y = sw[2];
            screen.pMax.y = sw[3];
        } else
            Error("\"screenwindow\" should have four values");
    }
    
    std::cout << "screen params" << std::endl;
    std::cout << screen.pMin.x << std::endl;
    std::cout << screen.pMin.y << std::endl;
    std::cout << screen.pMax.x << std::endl;
    std::cout << "resolution" << std::endl;
    std::cout << film->fullResolution.y << std::endl;
 


    Float fov = params.FindOneFloat("fov", 90.);
    Float halffov = params.FindOneFloat("halffov", -1.f);
    if (halffov > 0.f)
        // hack for structure synth, which exports half of the full fov
        fov = 2.f * halffov;



    return new LightFieldCamera(cam2world, screen, shutteropen, shutterclose,
                                 lensradius, focaldistance, fov, film, medium);
}

}  // namespace pbrt
