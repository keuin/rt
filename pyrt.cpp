//
// Created by keuin on 10/15/25.
//

#ifndef RT_PYRT_H
#define RT_PYRT_H

#include "viewport.h"
#include "aa.h"
#include "final_scene.h"
#include "timer.h"
#include <pybind11/pybind11.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace py = pybind11;

PYBIND11_MODULE(pyrt, m, py::mod_gil_not_used()) {
  m.doc() = R"pbdoc(
        Keuin's C++ ray tracer
        -----------------------

        .. currentmodule:: pyrt

        .. autosummary::
           :toctree: _generate

           FinalScene
    )pbdoc";

  py::class_<final_scene<uint16_t, double>>(m, "FinalScene")
    .def(py::init<uint64_t>())
    .def("render", &final_scene<u_int16_t, double>::render);

  py::class_<bitmap<u_int16_t>>(m, "BitMapU16")
    .def(py::init<unsigned, unsigned>());

  py::class_<hitlist>(m, "Hitlist")
    .def(py::init<>())
    .def("add_object", &hitlist::add_object);

  py::class_<aa_viewport<u_int16_t, double>>(m, "AAViewportU16")
    .def(
      py::init<
      vec3d, vec3d,
      uint16_t, uint16_t,
      double, double,
      double, double,
      hitlist,
      unsigned,
      int>(),
      py::arg("threads") = -1);

  // objects
  py::class_<object, std::shared_ptr<object>>(m, "RtObject")
    .def("hit",
      static_cast<bool(object::*)(const ray3d &r, double &t, double t1, double t2) const>(&object::hit))
    .def("hit_inf",
      static_cast<bool(object::*)(const ray3d &r, double &t, double t1) const>(&object::hit))
    .def("is_on", &object::is_on)
    .def("normal_vector", &object::normal_vector)
    .def("color", &object::color)
    .def("get_material", &object::get_material);

  py::class_<sphere, object>(m, "Sphere")
    .def(py::init<vec3d, double, material&>());

  // material objects
  py::class_<material>(m, "Material")
    .def("scatter", &material::scatter);

  py::class_<material_dielectric, material>(m, "MaterialDielectric")
    .def(py::init<double>());

  py::class_<material_diffuse_hemispherical, material>(m, "MaterialDiffuseHemispherical")
    .def(py::init<double>())
    .def(py::init<vec3d>());

  py::class_<material_diffuse_lambertian, material>(m, "MaterialDiffuseLambertian")
    .def(py::init<double>())
    .def(py::init<vec3d>());

  py::class_<material_diffuse_simple, material>(m, "MaterialDiffuseSimple")
    .def(py::init<double>())
    .def(py::init<vec3d>());

  py::class_<material_reflective, material>(m, "MaterialReflective")
    .def(py::init<double>())
    .def(py::init<vec3d>());

  py::class_<material_fuzzy_reflective, material>(m, "MaterialFuzzyReflective")
    .def(py::init<vec3d, double>())
    .def(py::init<double, double>());

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}


#endif // RT_PYRT_H
