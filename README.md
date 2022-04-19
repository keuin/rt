# rt: A C++11, STL-only, CPU-only, threaded, portable, brute-force ray-tracing image renderer

## Features

- Lock-free parallel rendering (no task queue)
- SSAA (Super-sampling anti-aliasing)
- Materials:
    + Diffusive (lambertian, hemispherical)
    + Reflective (fuzzy, perfectly-smooth)
    + Dielectric (reflective & refractive & total internal reflection)
- Shapes:
    + Sphere
- Misc:
    + Text printing (font data copied
      from [cse457](https://courses.cs.washington.edu/courses/cse457/98a/tech/OpenGL/font.c))
    + Benchmarking (PRNG, pixel filling)
    + Ray-Trace logging (very comprehensive log, examples are shown below)

## Tested platforms

- Linux
- Win32 (Windows 10)

## Ray-Trace logging

A piece of example ray:

```
+++ start tracing (limit=64)
    ray: [0.714360  ,-0.098532 ,-0.692807 ], decay=[1.000000  ,1.000000  ,1.000000  ]
    step start (remaining: 64)
    hit object 6sphere <0x192d090> at [0.662540  ,-0.091385 ,-0.642551 ], t=0.927460  , surface=-0.959413  (outside)
    reflect (perfectly smooth material)
    scattered ray: [-0.580692 ,-0.449236 ,0.678958  ], decay=[0.800000  ,0.600000  ,0.200000  ]
    step start (remaining: 63)
    hit object 6sphere <0x192cf00> at [0.128011  ,-0.504908 ,-0.017568 ], t=0.920504  , surface=-0.443287  (outside)
    reflect (diffusive, lambertian material)
    scattered ray: [-0.558489 ,0.559501  ,-0.612413 ], decay=[0.640000  ,0.480000  ,0.000000  ]
    step start (remaining: 62)
    hit object 6sphere <0x192cf70> at [-0.592631 ,0.217040  ,-0.807790 ], t=1.290343  , surface=-0.447577  (outside)
    refract (dielectric material, schlick, ri=0.666667  )
    scattered ray: [-0.783343 ,0.154017  ,-0.602206 ], decay=[0.640000  ,0.480000  ,0.000000  ]
    step start (remaining: 61)
    hit object 6sphere <0x192cf70> at [-1.221549 ,0.340694  ,-1.291280 ], t=0.802863  , surface=0.802863   (inside)
    refract (dielectric material, schlick, ri=1.500000  )
    scattered ray: [-0.839715 ,-0.284593 ,-0.462477 ], decay=[0.640000  ,0.480000  ,0.000000  ]
    step start (remaining: 60)
    hit object 6sphere <0x192cf00> at [-3.985005 ,-0.595887 ,-2.813265 ], t=3.290945  , surface=-0.242472  (outside)
    reflect (diffusive, lambertian material)
    scattered ray: [-0.618128 ,0.545380  ,0.566108  ], decay=[0.512000  ,0.384000  ,0.000000  ]
    step start (remaining: 59)
    hit infinity (light source)
--- finish (hit infinity)
 ^  apply to pixel: (79, 54), color: [20590.000000, 19331.000000, 0.000000] (8bit: [80, 75, 0], 8bit-gamma2: [142, 138, 0])
```