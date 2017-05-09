

    // pbrt source code is Copyright(c) 1998-2016
    //                     Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    // This file is part of pbrt.

    // Redistribution and use in source and binary forms, with or without
    // modification, are permitted provided that the following conditions are
    // met:

    // - Redistributions of source code must retain the above copyright
    //   notice, this list of conditions and the following disclaimer.

    // - Redistributions in binary form must reproduce the above copyright
    //   notice, this list of conditions and the following disclaimer in the
    //   documentation and/or other materials provided with the distribution.

    // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    // IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    // TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    // PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    // HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    // SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    // LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    // DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    // THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 


// shapes/sphere.cpp*
#include "shapes/customde.h"
#include "sampling.h"
#include "paramset.h"
#include "efloat.h"
#include "stats.h"

namespace pbrt {

// Sphere Method Definitions
Bounds3f CustomDE::ObjectBound() const {
    return Bounds3f(Point3f(-1.0, -1.0, -1.0),
                    Point3f(1.0, 1.0, 1.0));
}



Float CustomDE::Evaluate(const Point3f &p) const {
    const float bailout = 2.0f;
    const float Power = (float)mandelbulbPower;
    Point3f z = p;
    float dr = 1.0;
    float r = 0.0;
    for (int i = 0; i < fractalIters; i++) {
        r = (z-Point3f(0,0,0)).Length();
        if (r>bailout) break;

        // convert to polar coordinates
        float theta = acos(z.z/r);
        float phi = atan2(z.y,z.x);
        dr =  pow( r, Power-1.0)*Power*dr + 1.0;

        // scale and rotate the point
        float zr = pow( r,Power);
        theta = theta*Power;
        phi = phi*Power;

        // convert back to cartesian coordinates
        z = zr*Point3f(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z += p;
    }

    float sphereEval = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z) - 1;
    float mandEval = 0.5*log(r)*r/dr;
    return std::max(sphereEval, mandEval);
}

// bool MandelbulbDE::IntersectP(const Ray &r, bool testAlphaTexture) const {
//     return false;
// }

Float CustomDE::Area() const { return 1000000; }

Interaction CustomDE::Sample(const Point2f &u, Float *pdf) const {
    LOG(FATAL) << "Cone::Sample not implemented.";
    return Interaction();
}



std::shared_ptr<Shape> CreateCustomDEShape(const Transform *o2w,
                                         const Transform *w2o,
                                         bool reverseOrientation,
                                         const ParamSet &params) {
    int mandelbulbPower = params.FindOneInt("mandelbulbPower", 8);
    int fractalIters = params.FindOneInt("fractalIters", 1000);
    int maxiters = params.FindOneInt("maxiters", 1000);
    float hitEpsilon = params.FindOneFloat("hitEpsilon", 0.00001f);
    float rayEpsilonMultiplier = params.FindOneInt("rayEpsilonMultiplier", 10);
    float normalEpsilon = params.FindOneFloat("normalEpsilon",0.00001f);

    return std::make_shared<CustomDE>(o2w, w2o, reverseOrientation, mandelbulbPower, fractalIters, maxiters, hitEpsilon, rayEpsilonMultiplier, normalEpsilon);
}

}  // namespace pbrt
