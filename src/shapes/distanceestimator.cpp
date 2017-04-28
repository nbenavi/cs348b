
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


// shapes/sphere.cpp*
#include "shapes/distanceestimator.h"
#include "sampling.h"
#include "paramset.h"
#include "efloat.h"
#include "stats.h"

namespace pbrt {



Vector3f DistanceEstimator::CalculateNormal(const Point3f& pos, float eps, 
const Vector3f& defaultNormal) const {
    const Vector3f v1 = Vector3f( 1.0,-1.0,-1.0);
    const Vector3f v2 = Vector3f(-1.0,-1.0, 1.0);
    const Vector3f v3 = Vector3f(-1.0, 1.0,-1.0);
    const Vector3f v4 = Vector3f( 1.0, 1.0, 1.0);

    const Vector3f normal = v1 * Evaluate( pos + v1*eps ) +
                 v2 * Evaluate( pos + v2*eps ) +
                 v3 * Evaluate( pos + v3*eps ) +
                 v4 * Evaluate( pos + v4*eps );
    const Float length = normal.Length();

    return length > 0 ? (normal/length) : defaultNormal;
}


bool DistanceEstimator::Intersect(const Ray &r, Float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersect);
    Float phi;
    Point3f pHit;
    // Transform _Ray_ to object space
    Vector3f oErr, dErr;
    Ray ray = (*WorldToObject)(r, &oErr, &dErr);


    // Calculate Distance Evaluation
    float evalDistance = Evaluate(ray.o);




    // distance estimator main algorithm
    float timeStep = 0;
    float distance;
    int iters = 0;
    Point3f rayPoint = ray.o;
    Point3f intersectionPoint;
    while (timeStep < ray.tMax && iters < maxiters){
       distance = Evaluate(rayPoint);
       

       if (distance < hitEpsilon){
            
            if(tHit){
              *tHit = (Float)timeStep;
            }
            
            intersectionPoint = rayPoint;
            Normal3f dndu = Normal3f(0,0,0);
            Normal3f dndv = Normal3f(0,0,0);
            

            Vector3f surfaceNormal  = CalculateNormal(intersectionPoint, normalEpsilon, -ray.d);
            Vector3f pError         = Vector3f(hitEpsilon * rayEpsilonMultiplier, hitEpsilon * rayEpsilonMultiplier, hitEpsilon * rayEpsilonMultiplier);

            Vector3f dpdu, dpdv; 
            if (std::abs(surfaceNormal.x) > std::abs(surfaceNormal.y)){
                dpdu = Vector3f(-surfaceNormal.z, 0, surfaceNormal.x) / std::sqrt(surfaceNormal.x * surfaceNormal.x + surfaceNormal.z * surfaceNormal.z);
            } else {
                dpdu = Vector3f(0, surfaceNormal.z, -surfaceNormal.y) / std::sqrt(surfaceNormal.y * surfaceNormal.y + surfaceNormal.z * surfaceNormal.z);
            }

           dpdv = Cross(surfaceNormal, dpdu);
           
           if(isect){
            *isect = (*ObjectToWorld)(SurfaceInteraction(intersectionPoint, pError, Point2f(0, 0),
                                                 -ray.d, dpdu, dpdv, dndu, dndv,
                                                 ray.time, this));
           }
            return true;
       

       }

       timeStep +=  (distance / ray.d.Length());
       rayPoint = ray.o + ray.d * timeStep;
       ray.time = timeStep;
       iters++;
    }

    return false;
    
}



}  // namespace pbrt
