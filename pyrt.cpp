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

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}


#endif // RT_PYRT_H
